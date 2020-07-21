#include "team.h"

int main(int argc, char ** argv){

    initialize_team();

    return 0;
}

//--------States transition--------
//Called when appear a pokemon
void calculateTrainerFromNewToReady(){
    calculateTrainerToReady(NEW);
}

void calculateTrainerToReady(enum_process_state threadTrainerState){
    //From the list of pokemons on the map i seek wich trainer can look up for it
    uint32_t pokemonsOnMapCount = list_size(pokemonsOnMap);
    t_pokemon_on_map* pokemonOnMapAux;
    t_threadTrainer* threadTrainerAux;
    bool continueFor = true;

    for(int i = 0; i < pokemonsOnMapCount && continueFor; i++){
        pokemonOnMapAux = (t_pokemon_on_map*)list_get(pokemonsOnMap, i);
        if(pokemonOnMapAux->state == P_FREE){
            threadTrainerAux = getClosestTrainer(pokemonOnMapAux->position, threadTrainerState);
            if(threadTrainerAux != NULL){
                threadTrainerAux->state = READY;
				threadTrainerAux->positionTo.posx = pokemonOnMapAux->position.posx;
				threadTrainerAux->positionTo.posy = pokemonOnMapAux->position.posy;
				pokemonOnMapAux->state = P_CHASING;
				continueFor = false;
				threadTrainerAux->incomingTime = time(NULL);

				if(threadTrainerState == NEW){
					log_info(obligatory_logger, "Entrenador %d, cambia de NEW a READY, porque es el más cercano para realizar la captura", threadTrainerAux->idTrainer);
				}else{
					threadTrainerAux->contextSwitchCount++;
					log_info(obligatory_logger, "Entrenador %d, cambia de BLOCKED a READY, porque es el más cercano para realizar la captura", threadTrainerAux->idTrainer);
				}
				sem_post(&plannerSemaphore);
            }
        }
    }
}

t_threadTrainer* getClosestTrainer(t_position position, enum_process_state threadTrainerState){
    uint32_t threadsTrainersCount = list_size(threadsTrainers);
    t_threadTrainer* threadTrainerAux;
    int32_t minimunDistance = -1;
    int32_t indexClosestThreadTrainer = -1;
    t_trainer* trainerAux;

    for(int i=0; i < threadsTrainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->state == threadTrainerState){
            trainerAux = (t_trainer*)list_get(trainers, threadTrainerAux->idTrainer - 1);
            if(minimunDistance == -1 || minimunDistance > calculateDistance(trainerAux->position, position)){
                minimunDistance = calculateDistance(trainerAux->position, position);
                indexClosestThreadTrainer = i;
            }
        }
    }

    if(indexClosestThreadTrainer == -1){
        return NULL;
    }
    return (t_threadTrainer*)list_get(threadsTrainers, indexClosestThreadTrainer);
}

uint32_t calculateDistance(t_position positionFrom, t_position positionTo){
    int32_t posX = positionTo.posx - positionFrom.posx;
    int32_t posY = positionTo.posy - positionFrom.posy;
    if(posX < 0) posX = posX * -1;
    if(posY < 0) posY = posY * -1;
    return posX + posY;
}

//Called always that a trainer its ready
void calculateTrainerFromReadyToExec(){
    if(list_any_satisfy(threadsTrainers, existsThreadTrainerInExec)){
        return;
    }

    setTrainerToExec();
}

bool existsThreadTrainerInExec(void* threadTrainer){
    return ((t_threadTrainer*)threadTrainer)->state == EXEC;
}

void setTrainerToExec(){
    if (strcmp(config_values.algoritmo_planificacion, "FIFO") == 0) setTrainerToExec_FirstCome();
    else if (strcmp(config_values.algoritmo_planificacion, "RR") == 0) setTrainerToExec_FirstCome();
    else if (strcmp(config_values.algoritmo_planificacion, "SJF-SD") == 0) setTrainerToExec_SJF();
    else if (strcmp(config_values.algoritmo_planificacion, "SJF-CD") == 0) setTrainerToExec_SJF();
}

void setTrainerToExec_FirstCome(){
    int32_t indexFirstTrainer = -1;
    time_t lowestTime = time(NULL);
    t_threadTrainer* threadTrainerAux;

    for(int i=0; i < list_size(trainers); i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->incomingTime <= lowestTime && threadTrainerAux->state == READY){
            lowestTime = threadTrainerAux->incomingTime;
            indexFirstTrainer = i;
        }
    }

	if(indexFirstTrainer != -1){
		threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, indexFirstTrainer);
		threadTrainerAux->state = EXEC;
		pthread_mutex_unlock(&(threadTrainerAux->mutexAction));
		log_info(obligatory_logger, "Entrenador %d, cambia de READY a EXEC, porque es el siguiente a ejecutar", threadTrainerAux->idTrainer);
	}
}

void setTrainerToExec_SJF(){
    //SJF: Estimador = Ti-1*alpha + Ri-1*(1-alpha)
    int32_t indexFirstTrainer = -1;
    double estimator = -1;
    uint32_t trainersCount = list_size(trainers);
	uint32_t cyclesNeeded;
	double previusEstimator;
    t_threadTrainer* threadTrainerAux;
	t_trainer* trainerAux;

    for(int i=0; i < trainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
		trainerAux = (t_trainer*)list_get(trainers, i);
		if(threadTrainerAux->state == READY){	
			previusEstimator = threadTrainerAux->valueEstimator;
			cyclesNeeded = calculateDistance(trainerAux->position, threadTrainerAux->positionTo);

			threadTrainerAux->valueEstimator = previusEstimator * config_values.alpha + cyclesNeeded * (1 - config_values.alpha);
		
			if((estimator == -1 || estimator > threadTrainerAux->valueEstimator) && threadTrainerAux->state == READY){
				estimator = threadTrainerAux->valueEstimator;
				indexFirstTrainer = i;
			}
		}
    }

	if(indexFirstTrainer != -1){
		threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, indexFirstTrainer);
		threadTrainerAux->state = EXEC;
		pthread_mutex_unlock(&(threadTrainerAux->mutexAction));
		log_info(obligatory_logger, "Entrenador %d, cambia de READY a EXEC, porque es el siguiente a ejecutar", threadTrainerAux->idTrainer);
	}
}

//Called when a pokemon appear, on deadlock thread, and on message "caught pokemon"
void calculateLeaveBlockedFromAppear(){
    calculateTrainerToReady(BLOCKED);
}

void calculateLeaveBlockedFromDeadlock(uint32_t idTrainer){
    t_threadTrainer* threadTrainerAux;
    uint32_t threadsTrainersCount = list_size(threadsTrainers);
    bool continueFor = true;

    for(int i=0; i<threadsTrainersCount && continueFor; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->idTrainer == idTrainer){
            continueFor = false;
            threadTrainerAux->state = READY;
			threadTrainerAux->incomingTime = time(NULL);
			log_info(obligatory_logger, "Entrenador %d, cambia de BLOCKED a READY, porque intercambiará por deadlock", threadTrainerAux->idTrainer);
			sem_post(&plannerSemaphore);
        }
    }
}

void calculateLeaveBlockedFromCaught(uint32_t idTrainer){
    //If the trainer accomplish his goal goes to exit, else, stays in blocked
    t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, idTrainer - 1);
    t_trainer* trainerAux = (t_trainer*)list_get(trainers, idTrainer - 1);

	if(trainerCompleteOwnObjetives(trainerAux)){
		threadTrainerAux->state = E_P_EXIT;
		log_info(obligatory_logger, "Entrenador %d, cambia de BLOCKED a EXIT, porque cumplió sus objetivos individuales", threadTrainerAux->idTrainer);
	}
}

bool trainerCompleteOwnObjetives(t_trainer* trainerAux){
	bool result = false;
	if(list_size(trainerAux->pokemonNeeded) == list_size(trainerAux->pokemonOwned)){
		t_list* copyOfNeeded = list_duplicate(trainerAux->pokemonNeeded);
		t_list* copyOfOwned = list_duplicate(trainerAux->pokemonOwned);
		result = true;

		list_sort(copyOfNeeded, compareStrings);
		list_sort(copyOfOwned, compareStrings);

		for(int j=0; j<list_size(copyOfNeeded) && result; j++){
			if(strcmp((char*)list_get(copyOfNeeded, j), (char*)list_get(copyOfOwned, j)) != 0){
				result = false;
			}
		}
		list_destroy(copyOfNeeded);
		list_destroy(copyOfOwned);
	}

	return result;
}

bool compareStrings(void* string1, void* string2){
    return(strcmp((char*)string1, (char*)string2) < 0);
}

void calculateTrainersInExit(){
	for(int i=0; i<list_size(threadsTrainers); i++){
		t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
		if(threadTrainerAux->state == E_P_EXIT) pthread_mutex_unlock(&threadTrainerAux->mutexAction);
	}
}

void calculateTrainerInExit(uint32_t idTrainer){
	t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, idTrainer-1);

	if(threadTrainerAux->state == E_P_EXIT){
		writeTrainerMetrics(idTrainer);
		sem_post(&plannerSemaphore);
		pthread_cancel(threadTrainerAux->threadTrainer);
		pthread_exit(NULL);
	}
}

void writeTrainerMetrics(uint32_t idTrainer){
    t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, idTrainer-1);

	log_info(obligatory_logger, "Trainer: %d", idTrainer);
	log_info(obligatory_logger, "Total Cycle CPU by Trainer %d: %d", idTrainer, threadTrainerAux->cpuCycleCount);
}

bool trainerStateIsExit(void* threadTrainer){
    return ((t_threadTrainer*)threadTrainer)->state == E_P_EXIT;
}

void writeTeamMetrics(){
    t_threadTrainer* threadTrainerAux;
    uint32_t totalCycleCpuTeam = 0;
    uint32_t totalContextSwitch = 0;

    for(int i = 0; i<list_size(threadsTrainers); i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        totalCycleCpuTeam += threadTrainerAux->cpuCycleCount;
        totalContextSwitch += threadTrainerAux->contextSwitchCount;
               
    }
	log_info(obligatory_logger, "Algorithm: %s ", config_values.algoritmo_planificacion); 
	log_info(obligatory_logger, "Total Cycle CPU by TEAM: %d", totalCycleCpuTeam);
	log_info(obligatory_logger, "Total Context Switch by Team: %d", totalContextSwitch);
	log_info(obligatory_logger, "Total Deadlocks by Team: %d", deadlockCount);
}

void finishTeam(){
    release_resources();
	exit(1);
}

void executeAlgorithm() {
	t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_find(threadsTrainers, threadTrainerInExec);

	if(threadTrainerAux != NULL && threadTrainerAux->state == EXEC){
		if (strcmp(config_values.algoritmo_planificacion, "FIFO") == 0) execThreadTrainerSetedFCFS(threadTrainerAux);
		else if (strcmp(config_values.algoritmo_planificacion, "RR") == 0) execThreadTrainerSetedRR(threadTrainerAux);
		else if (strcmp(config_values.algoritmo_planificacion, "SJF-SD") == 0) execThreadTrainerSetedSJF_SD(threadTrainerAux);
		else if (strcmp(config_values.algoritmo_planificacion, "SJF-CD") == 0) execThreadTrainerSetedSJF_CD(threadTrainerAux);
	}
}

bool threadTrainerInExec(void* threadTrainerAux){
	return ((t_threadTrainer*)threadTrainerAux)->state == EXEC;
}

t_pokemon_on_map* getPokemonByPosition(t_position pokemonPosition){
	positionCompare.posx = pokemonPosition.posx;
	positionCompare.posy = pokemonPosition.posy;

	return (t_pokemon_on_map*)list_find(pokemonsOnMap, pokemonsOnMapComparePosition);
}
bool pokemonsOnMapComparePosition(void* pokemonOnMapAux){
	return ((t_pokemon_on_map*)pokemonOnMapAux)->position.posx == positionCompare.posx && ((t_pokemon_on_map*)pokemonOnMapAux)->position.posy == positionCompare.posy;
}
bool move_to_objetive(t_trainer* trainerAux, t_position positionTo){
	bool reachDestiny = false;
	t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, trainerAux->id_trainer - 1);
	t_position currentPosition = trainerAux->position;
	if(trainerAux->position.posx != positionTo.posx){
		if(trainerAux->position.posx < positionTo.posx){
			trainerAux->position.posx++;
		}else{
			trainerAux->position.posx--;
		}
	}else{
		if(trainerAux->position.posy != positionTo.posy){
			if(trainerAux->position.posy < positionTo.posy){
				trainerAux->position.posy++;
			}else{
				trainerAux->position.posy--;
			}
		}	
	}
	threadTrainerAux->cpuCycleCount++;
	log_info(obligatory_logger, "Entrenador %d - Posicion actual: (%d,%d) - Posicion destino: (%d,%d)", trainerAux->id_trainer, currentPosition.posx, currentPosition.posy, trainerAux->position.posx, trainerAux->position.posy);
	sleep(config_values.retardo_ciclo_cpu);

	if(trainerAux->position.posx == threadTrainerAux->positionTo.posx && trainerAux->position.posy == threadTrainerAux->positionTo.posy){
		if(threadTrainerAux->destinyIsTrainer){
			//My goal was a trainer, then, interchange
			if(threadTrainerAux->interchangeCycleCount == 5){
				reachDestiny = true;
				threadTrainerAux->destinyIsTrainer = false;
				threadTrainerAux->interchangeCycleCount = 0;
				interchangePokemon(trainerAux);
			}
			threadTrainerAux->interchangeCycleCount++;
		}else{
			reachDestiny = true;
			//My goal was a pokemon, then, message catch
			t_pokemon_on_map* pokemonDestiny = getPokemonByPosition(trainerAux->position);
			if(pokemonDestiny != NULL){
				if(!sendCatch(pokemonDestiny)){
					catch_succesfull(trainerAux->id_trainer);
				}
			}
		}
	}
	return reachDestiny;
}

void interchangePokemon(t_trainer* trainerFrom){
	t_trainer* trainerTo;
	t_threadTrainer* threadTrainerTo;
	t_threadTrainer* threadTrainerFrom = (t_threadTrainer*)list_get(threadsTrainers, trainerFrom->id_trainer - 1);
	bool continueFor = true;

	for(int i=0; i<list_size(trainers) && continueFor; i++){
		trainerTo = (t_trainer*)list_get(trainers, i);
		threadTrainerTo = (t_threadTrainer*)list_get(threadsTrainers, i);
		if(trainerTo->position.posx == trainerFrom->position.posx 
			&& trainerTo->position.posy == trainerFrom->position.posy && trainerTo->id_trainer != trainerFrom->id_trainer
			&& threadTrainerTo->state == BLOCKED){
				t_cycleDeadlock* pokemonInDeadlock = (t_cycleDeadlock*)list_get(cycleDeadLock, 0);

				//Remove from my owned any i dont need
				char* pokemonOwnedTrainerFrom = getPokemonNotNeeded(trainerFrom);
				char* pokemonOwnedTrainerTo = getPokemonSpecify(trainerTo, pokemonInDeadlock->pokemon);

				list_add(trainerFrom->pokemonOwned, pokemonOwnedTrainerTo);
				list_add(trainerTo->pokemonOwned, pokemonOwnedTrainerFrom);

				list_destroy(cycleDeadLock);
				flagExistsDeadlock = false;
				continueFor = false;
				if(trainerCompleteOwnObjetives(trainerFrom)){
					threadTrainerFrom->state = E_P_EXIT;
					log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a EXIT, porque cumplió sus objetivos individuales luego de intercambio", trainerFrom->id_trainer);
				}
				calculateLeaveBlockedFromCaught(trainerTo->id_trainer);
				sem_post(&plannerSemaphore);
			}
	}
}

char* getPokemonNotNeeded(t_trainer* trainerAux){
	char* result;
	bool continueFor = true;
	for(int j = 0; j < list_size(trainerAux->pokemonOwned) && continueFor; j++){
		result = (char*)list_get(trainerAux->pokemonOwned, j);
		pokemonCompareGlobalObjetive = malloc(strlen(result));
		strcpy(pokemonCompareGlobalObjetive, result);
		if(!list_any_satisfy(trainerAux->pokemonNeeded, analyzePokemonInGlobal)){
			list_remove(trainerAux->pokemonOwned, j);
			continueFor = false;
		}
		free(pokemonCompareGlobalObjetive);
	}
	return result;
}
char* getPokemonSpecify(t_trainer* trainerAux, char* pokemon){
	char* result;
	pokemonCompareGlobalObjetive = malloc(strlen(pokemon));
	strcpy(pokemonCompareGlobalObjetive, pokemon);
	result = (char*)list_remove_by_condition(trainerAux->pokemonOwned, analyzePokemonInGlobal);
	free(pokemonCompareGlobalObjetive);

	return result;
}

bool sendCatch(t_pokemon_on_map* pokemon){
	//TODO
	/*uint32_t* id_message = malloc(sizeof(uint32_t));
	catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon));
	void* stream =catch_pokemon_to_stream(catchPokemonMessage, id_message);

 	send(client_fd, stream, sizeof(pokemon_on_map), MSG_WAITALL);
	 */
	bool result = false;
	log_info(obligatory_logger, "Atrapar pokemon: %s, en posicion: (%d,%d)", pokemon->pokemon, pokemon->position.posx, pokemon->position.posy);

	if(!result){
		log_info(obligatory_logger, "Falló conexión con broker, se ejecutará función por default de appeared");
	}
	return result;
}
void catch_succesfull(uint32_t id_trainer){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, id_trainer-1);
	t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, id_trainer-1);

	//Set the pokemon as catched
	t_pokemon_on_map* pokemonOnMapAux = getPokemonByPosition(threadTrainerAux->positionTo);
	t_pokemon_on_map* pokemonOnMapToRemove;
	bool continueFor = true;
	pokemonOnMapAux->state = P_CATCHED;
	for(int i=0; i<list_size(pokemonsOnMap) && continueFor; i++){
		pokemonOnMapToRemove = (t_pokemon_on_map*)list_get(pokemonsOnMap, i);
		if(pokemonOnMapToRemove->id == pokemonOnMapAux->id){
			list_remove(pokemonsOnMap, i);
			continueFor = false;
		}
	}

	//Remove the global objetive
	pokemonCompareGlobalObjetive = malloc(strlen(pokemonOnMapAux->pokemon));
	strcpy(pokemonCompareGlobalObjetive, pokemonOnMapAux->pokemon);
	list_remove_by_condition(globalObjetive, analyzePokemonInGlobal);
	free(pokemonCompareGlobalObjetive);

	//Add pokemon owned
	char* newPokemonOwned = malloc(strlen(pokemonOnMapAux->pokemon));
	strcpy(newPokemonOwned, pokemonOnMapAux->pokemon);
	list_add(trainerAux->pokemonOwned, newPokemonOwned);

	//Validate trainer objetive complete
	if(trainerCompleteOwnObjetives(trainerAux)){
		threadTrainerAux->state = E_P_EXIT;
		log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a EXIT, porque cumplió sus objetivos individuales", threadTrainerAux->idTrainer);
		calculateTrainerInExit(trainerAux->id_trainer);
	}else{
		threadTrainerAux->state = BLOCKED;
		threadTrainerAux->contextSwitchCount++;
		log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a BLOCKED, porque le falta capturar más pokemon", threadTrainerAux->idTrainer);
		sem_post(&plannerSemaphore);
	}
}
//// algoritmos de planifiacion
//despues de  setTrainerToExec_FirstCome() obtengo el threadTrainerChosen y el pokemonOnMap
void execThreadTrainerSetedFCFS(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	uint32_t distanceToMove = calculateDistance(trainerAux->position, threadTrainerChosen->positionTo);
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	bool reachDestiny = false;

	if(pokemonOnMap != NULL){
		pokemonOnMap->state = P_CHASING;
	}
	
	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; i<=distanceToMove || !reachDestiny; i++){
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);
	}
}
void execThreadTrainerSetedRR(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	bool continueMoving = true;
	bool reachDestiny = false;
	uint32_t usedCycle = 0;
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	
	if(pokemonOnMap != NULL){
		pokemonOnMap->state = P_CHASING;
	}

	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; continueMoving && !reachDestiny; i++){
		usedCycle++;
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);

		if(!reachDestiny && usedCycle == config_values.quantum){
			threadTrainerChosen->state = READY;
			threadTrainerChosen->contextSwitchCount++;
			threadTrainerChosen->incomingTime = time(NULL);
			log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a READY, porque se le acabó el QUANTUM", threadTrainerChosen->idTrainer);
			continueMoving = false;
			sem_post(&plannerSemaphore);
		}
	}
}
void execThreadTrainerSetedSJF_SD(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	if(pokemonOnMap != NULL){
		pokemonOnMap->state = P_CHASING;
	}
	
	bool reachDestiny = false;
	
	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; !reachDestiny; i++){
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);
	}
}

void execThreadTrainerSetedSJF_CD(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	if(pokemonOnMap != NULL){
		pokemonOnMap->state = P_CHASING;
	}
	int countPokemonOnReady = calculatePokemonsOnReady();
	bool reachDestiny = false;
	bool continueMoving = true;
	
	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; !reachDestiny && continueMoving; i++){
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);

		if(!reachDestiny){
			if(countPokemonOnReady != calculatePokemonsOnReady()){
				threadTrainerChosen->state = READY;
				threadTrainerChosen->contextSwitchCount++;
				threadTrainerChosen->incomingTime = time(NULL);
				log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a READY, porque apareció un entrenador con estimador menor que está en READY", threadTrainerChosen->idTrainer);
				sem_post(&plannerSemaphore);
				continueMoving = false;
			}
		}
	}
}

int calculatePokemonsOnReady(){
	int count = 0;
	t_threadTrainer* threadTrainerAux;

	for(int i=0; i<list_size(threadsTrainers);i++){
		threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
		if(threadTrainerAux->state == READY) count++;
	}

	return count;
}
