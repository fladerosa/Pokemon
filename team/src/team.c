#include "team.h"

int main(int argc, char ** argv){

    initialize_team();

    //release_resources();    
    return 0;
}

//--------States transition--------
//Called when appear a pokemon
void calculateTrainerFromNewToReady(){
	log_info(optional_logger, "calculateTrainerFromNewToReady");
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
                //existNewReady = true;
				threadTrainerAux->incomingTime = time(NULL);

				if(threadTrainerState == NEW){
					log_info(obligatory_logger, "Entrenador %d, cambia de NEW a READY, porque es el más cercano para realizar la captura", threadTrainerAux->idTrainer);
				}else{
					//TODO verificar que hacer con deadlock
					log_info(obligatory_logger, "Entrenador %d, cambia de BLOCKED a READY, porque es el más cercano para realizar la captura", threadTrainerAux->idTrainer);
				}
				//pthread_mutex_unlock(&(threadTrainerAux->mutexAction));
            }
        }
    }
/*
    if(existNewReady){
        calculateTrainerFromReadyToExec();
    }*/
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
	log_info(optional_logger, "calculateTrainerFromReadyToExec");
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

	//executeAlgorithm();
}

void setTrainerToExec_FirstCome(){
    int32_t indexFirstTrainer = -1;
    time_t lowestTime = time(NULL);
    t_threadTrainer* threadTrainerAux;

    for(int i=0; i < list_size(trainers); i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
		//log_info(optional_logger, "Entrenador %d - incomingTime: %ld, %lld - lowestTime %ld, %lld", threadTrainerAux->idTrainer, threadTrainerAux->incomingTime,threadTrainerAux->incomingTime,lowestTime,lowestTime);
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
		//log_info(optional_logger, "Entrenador %d - incomingTime: %lld", threadTrainerAux->idTrainer, threadTrainerAux->incomingTime);
	}
}

void setTrainerToExec_SJF(){
    //SJF
    //Estimador = Ti-1*alpha + Ri-1*(1-alpha)
    //previousIncomingTime-1*config_values.alpha + incomingTime-1*(1-config_values.alpha)
    //revisar
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
			log_info(optional_logger, "Previous estimator: %f, new estimator: %f", previusEstimator, threadTrainerAux->valueEstimator);
		
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
            //calculateTrainerFromReadyToExec(READY);
			//pthread_mutex_unlock(&plannerMutex);
			sem_post(&plannerSemaphore);
        }
    }
}

void calculateLeaveBlockedFromCaught(uint32_t idTrainer){
    //If the trainer accomplish his goal goes to exit, else, stays in blocked
    t_threadTrainer* threadTrainerAux;
    uint32_t threadsTrainersCount = list_size(threadsTrainers);
    bool continueFor = true;
    t_trainer* trainerAux;

    for(int i=0; i<threadsTrainersCount && continueFor; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        trainerAux = (t_trainer*)list_get(trainers, i);
        
		if(threadTrainerAux->idTrainer == idTrainer){
			if(trainerCompleteOwnObjetives(trainerAux)){
				threadTrainerAux->state = EXIT;
				log_info(obligatory_logger, "Entrenador %d, cambia de BLOCKED a EXIT, porque cumplió sus objetivos individuales", threadTrainerAux->idTrainer);
				calculateTrainerInExit(idTrainer);
			}
        }
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

void calculateTrainerInExit(uint32_t idTrainer){
    writeTrainerMetrics(idTrainer);
	t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, idTrainer-1);
//	pthread_mutex_lock(&(threadTrainerAux->mutexAction));
	//pthread_mutex_unlock(&plannerMutex);
	sem_post(&plannerSemaphore);
	pthread_cancel(threadTrainerAux->threadTrainer);
	pthread_exit(NULL);
	
	
    /*if(list_all_satisfy(threadsTrainers, trainerStateIsExit)){
        writeTeamMetrics();
        finishTeam();
    }
	calculateTrainerFromReadyToExec();
	*/
}

void writeTrainerMetrics(uint32_t idTrainer){
    //write trainer metrics
    t_threadTrainer* threadTrainerAux;

	threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, idTrainer-1);

	log_info(obligatory_logger, "Trainer: %d", idTrainer);
	log_info(obligatory_logger, "Total Cycle CPU by Trainer: %d", threadTrainerAux->cpuCycleCount);
	log_info(optional_logger, "Total context switch by Trainer: %d", threadTrainerAux->contextSwitchCount);
}

bool trainerStateIsExit(void* threadTrainer){
    return ((t_threadTrainer*)threadTrainer)->state == EXIT;
}

void writeTeamMetrics(){
    //write team metrics
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
}

void finishTeam(){
    //finish team
    release_resources();
	exit(1);
}


void executeAlgorithm() {
	t_threadTrainer* threadTrainerAux = (t_threadTrainer*)list_find(threadsTrainers, threadTrainerInExec);

	if(threadTrainerAux != NULL){
		if (strcmp(config_values.algoritmo_planificacion, "FIFO") == 0) execThreadTrainerSetedFCFS(threadTrainerAux);
		else if (strcmp(config_values.algoritmo_planificacion, "RR") == 0) execThreadTrainerSetedRR(threadTrainerAux);
		else if (strcmp(config_values.algoritmo_planificacion, "SJF-SD") == 0) execThreadTrainerSetedSJF_SD(threadTrainerAux);
		else if (strcmp(config_values.algoritmo_planificacion, "SJF-CD") == 0) execThreadTrainerSetedSJF_CD(threadTrainerAux);
	}
}

bool threadTrainerInExec(void* threadTrainerAux){
	return ((t_threadTrainer*)threadTrainerAux)->state == EXEC;
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

	log_info(optional_logger, "Team FIFO - Trainer %d captured pokemon: %s", trainerAux->id_trainer, (char*)list_get(trainerAux->pokemonOwned, 0));
	log_info(optional_logger, "Team FIFO - Trainer %d needs pokemon: %s", trainerAux->id_trainer, (char*)list_get(trainerAux->pokemonNeeded, 0));
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
		t_pokemon_on_map* pokemonDestiny = getPokemonByPosition(trainerAux->position);
		if(pokemonDestiny == NULL){
			//My goal was a trainer, then, interchange
			if(threadTrainerAux->interchangeCycleCount == 5){
				reachDestiny = true;
				threadTrainerAux->interchangeCycleCount = 0;
				interchangePokemon(trainerAux);
			}
			threadTrainerAux->interchangeCycleCount++;
		}else{
			reachDestiny = true;
			//My goal was a pokemon, then, message catch
			if(!sendCatch(pokemonDestiny)){
				catch_succesfull(trainerAux->id_trainer);
				log_info(optional_logger, "Caught by Default.");
				log_info(optional_logger, "Pokemon %s", pokemonDestiny->pokemon);
			}
		}
	}
	return reachDestiny;
}

void interchangePokemon(t_trainer* trainerFrom){
	t_trainer* trainerTo;
	t_threadTrainer* threadTrainerTo;
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
					trainerFrom->state = EXIT;
					log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a EXIT, porque cumplió sus objetivos individuales", trainerFrom->id_trainer);
					calculateTrainerInExit(trainerFrom->id_trainer);
					calculateLeaveBlockedFromCaught(trainerTo->id_trainer);
				}
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
	log_info(optional_logger, "Team - Trainer %d captured pokemon: %s", trainerAux->id_trainer, (char*)list_get(trainerAux->pokemonOwned, 0));
	log_info(optional_logger, "Team - Trainer %d needs pokemon: %s", trainerAux->id_trainer, (char*)list_get(trainerAux->pokemonNeeded, 0));

	//Validate trainer objetive complete
	if(trainerCompleteOwnObjetives(trainerAux)){
		threadTrainerAux->state = EXIT;
		log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a EXIT, porque cumplió sus objetivos individuales", threadTrainerAux->idTrainer);
		calculateTrainerInExit(trainerAux->id_trainer);
	}else{
		threadTrainerAux->state = BLOCKED;
		log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a BLOCKED, porque le falta capturar más pokemon", threadTrainerAux->idTrainer);
		//pthread_mutex_unlock(&plannerMutex);
		sem_post(&plannerSemaphore);
		//calculateTrainerFromReadyToExec();
	}
}

void execThreadTrainerSetedRR(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	uint32_t distanceToMove = calculateDistance(trainerAux->position, threadTrainerChosen->positionTo);
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	bool continueMoving = true;
	bool reachDestiny = false;
	uint32_t usedCycle = 0;
	
	pokemonOnMap->state = P_CHASING;
	
	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; i<distanceToMove && continueMoving && !reachDestiny; i++){
		usedCycle++;
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);

		if(!reachDestiny && usedCycle == config_values.quantum){
			threadTrainerChosen->state = READY;
			threadTrainerChosen->contextSwitchCount++;
			threadTrainerChosen->incomingTime = time(NULL);
			log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a READY, porque se le acabó el QUANTUM", threadTrainerChosen->idTrainer);
			//calculateTrainerFromReadyToExec();
			continueMoving = false;
			//pthread_mutex_unlock(&plannerMutex);
			sem_post(&plannerSemaphore);
		}
	}
}
void execThreadTrainerSetedSJF_SD(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	uint32_t distanceToMove = calculateDistance(trainerAux->position, threadTrainerChosen->positionTo);
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	pokemonOnMap->state = P_CHASING;
	bool reachDestiny = false;
	
	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; i<distanceToMove && !reachDestiny; i++){
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);
	}
}

void execThreadTrainerSetedSJF_CD(t_threadTrainer* threadTrainerChosen){
	t_trainer* trainerAux = (t_trainer*)list_get(trainers, threadTrainerChosen->idTrainer-1);
	uint32_t distanceToMove = calculateDistance(trainerAux->position, threadTrainerChosen->positionTo);
	t_pokemon_on_map* pokemonOnMap = getPokemonByPosition(threadTrainerChosen->positionTo);
	pokemonOnMap->state = P_CHASING;
	int countPokemonOnReady = calculatePokemonsOnReady();
	bool reachDestiny = false;
	bool continueMoving = true;
	log_info(optional_logger, "Cantidad de readies: %d", countPokemonOnReady);
	
	threadTrainerChosen->contextSwitchCount++;
	for(int i=0; i<distanceToMove && !reachDestiny && continueMoving; i++){
		reachDestiny = move_to_objetive(trainerAux, threadTrainerChosen->positionTo);

		if(!reachDestiny){
			if(countPokemonOnReady != calculatePokemonsOnReady()){
				threadTrainerChosen->state = READY;
				threadTrainerChosen->contextSwitchCount++;
				threadTrainerChosen->incomingTime = time(NULL);
				log_info(obligatory_logger, "Entrenador %d, cambia de EXEC a READY, porque apareció un entrenador con estimador menor que está en READY", threadTrainerChosen->idTrainer);
				//pthread_mutex_unlock(&plannerMutex);
				sem_post(&plannerSemaphore);
				continueMoving = false;
				//calculateTrainerFromReadyToExec();
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

void calculate_cpu_burst_threadTrainerOnExec(t_threadTrainer* threadTrainerChosen, uint32_t distanceToMove) {	
	uint32_t time_delay = config_values.retardo_ciclo_cpu;
	uint32_t cpuBurst = distanceToMove * time_delay;
	threadTrainerChosen->cpuCycleCount = cpuBurst;

	log_info(obligatory_logger, "Thread Trainer on state EXEC %d", threadTrainerChosen->idTrainer);
	log_info(obligatory_logger, "Cpu burst with time delay %d", cpuBurst);
}
void move_to_objetive_FCFS(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){
	void* pokemon;
	threadTrainerChosen->positionTo.posx = pokemon_on_map.position.posx;
	threadTrainerChosen->positionTo.posy = pokemon_on_map.position.posy;

	pokemon= list_find(pokemonsOnMap, isPokemonOnMapFree);
	pokemon_on_map.pokemon = (char*)pokemon;
	pokemon_on_map.state = P_CHASING;
	
}
bool isPokemonOnMapFree(void * pokemon_on_map) {
	return ((t_pokemon_on_map*)pokemon_on_map)->state == P_FREE;
}
bool isPokemonOnMapChasing(void * pokemon_on_map) {
	return ((t_pokemon_on_map*)pokemon_on_map)->state == P_CHASING;
}

void send_catch_pokemon(uint32_t client_fd, t_pokemon_on_map pokemon_on_map){
	uint32_t* id_message = malloc(sizeof(uint32_t));
	catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon));
	void* stream =catch_pokemon_to_stream(catchPokemonMessage, id_message);

 	send(client_fd, stream, sizeof(pokemon_on_map), MSG_WAITALL);
    log_info(optional_logger, "Sending Message Catch pokemon.");
	log_info(optional_logger, "Pokemon %s: ", pokemon_on_map.pokemon);

}
void move_to_objetive_RR(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map, uint32_t distanceToMove){
	uint32_t quantumRR = config_values.quantum;
	void* pokemon;

		if(distanceToMove <= quantumRR ) {
		threadTrainerChosen->positionTo.posx = pokemon_on_map.position.posx;
		threadTrainerChosen->positionTo.posy = pokemon_on_map.position.posy;
		} 
		else {
			distanceToMove -= quantumRR;
			if(distanceToMove == 0)	{
			//al estar en P_CHASING, debe inhabilitarse de futuras capturas ?
			threadTrainerChosen->positionTo.posx = pokemon_on_map.position.posx;
		 	threadTrainerChosen->positionTo.posy = pokemon_on_map.position.posy;

			pokemon= list_find(pokemonsOnMap, isPokemonOnMapFree);
			pokemon_on_map.pokemon = (char*)pokemon;
			pokemon_on_map.state = P_CHASING;
			}else{
			threadTrainerChosen->remainingDistance = distanceToMove;
			}
		}
	pokemon= list_find(pokemonsOnMap, isPokemonOnMapFree);
	pokemon_on_map.pokemon = (char*)pokemon;
	pokemon_on_map.state = P_CHASING;
	//threadTrainerChosen->state = READY;
	//calculateTrainerFromReadyToExec();
	threadTrainerChosen->contextSwitchCount++;
} 
void move_to_objetive_SJF_CD(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map, uint32_t distanceToMove){	
	void* pokemon;
	//falta
	//en cada instante de tiempo se debe verificar si exite otro thread  con mejor estimador, que este en ready	
	//esta significa un context switch y ademas cortar la rafaga en ejecucion
	pokemon= list_find(pokemonsOnMap, isPokemonOnMapFree);
	pokemon_on_map.pokemon = (char*)pokemon;
	pokemon_on_map.state = P_CHASING;
	//threadTrainerChosen->state = READY;
	calculateTrainerFromReadyToExec();
	threadTrainerChosen->contextSwitchCount++;
}