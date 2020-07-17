#include "team.h"

int main(int argc, char ** argv){

    initialize_team();

    //release_resources();    
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
    bool existNewReady = false;

    for(int i = 0; i < pokemonsOnMapCount; i++){
        pokemonOnMapAux = (t_pokemon_on_map*)list_get(pokemonsOnMap, i);
        if(pokemonOnMapAux->state == P_FREE){
            threadTrainerAux = getClosestTrainer(pokemonOnMapAux->position, threadTrainerState);
            if(threadTrainerAux != NULL){
                threadTrainerAux->state = READY;
                existNewReady = true;
            }
        }
    }

    if(existNewReady){
        calculateTrainerFromReadyToExec();
    }
}

t_threadTrainer* getClosestTrainer(t_position position, enum_process_state threadTrainerState){
    uint32_t threadsTrainersCount = list_size(threadsTrainers);
    t_threadTrainer* threadTrainerAux;
    uint32_t minimunDistance = -1;
    uint32_t indexClosestThreadTrainer = -1;
    t_trainer* trainerAux;

    for(int i=0; i < threadsTrainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->state == threadTrainerState){
            trainerAux = (t_trainer*)list_get(trainers, threadTrainerAux->idTrainer - 1);
            if(minimunDistance == -1 || minimunDistance < calculateDistance(trainerAux->position, position)){
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
    uint32_t posX = positionTo.posx - positionFrom.posx;
    uint32_t posY = positionTo.posy - positionFrom.posy;
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
    //Method EXEC
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
    uint32_t indexFirstTrainer = -1;
    time_t lowestTime = time(NULL);
    uint32_t trainersCount = list_size(trainers);
    t_threadTrainer* threadTrainerAux;

    for(int i=0; i < trainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->incomingTime < lowestTime){
            lowestTime = threadTrainerAux->incomingTime;
            indexFirstTrainer = i;
        }
    }

    threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, indexFirstTrainer);
    threadTrainerAux->state = EXEC;
}

void setTrainerToExec_SJF(){
    //SJF
    //Estimador = Ti-1*alpha + Ri-1*(1-alpha)
    //previousIncomingTime-1*config_values.alpha + incomingTime-1*(1-config_values.alpha)
    //revisar
    uint32_t indexFirstTrainer = -1;
    uint32_t estimator;
    uint32_t trainersCount = list_size(trainers);
    t_threadTrainer* threadTrainerAux;

    for(int i=0; i < trainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        uint32_t previusEstimator = threadTrainerAux->previousIncomingTime -config_values.alpha;
        uint32_t incomingEstimator = threadTrainerAux->incomingTime-(1-config_values.alpha);
        estimator =  previusEstimator + incomingEstimator;
        threadTrainerAux->valueEstimator = estimator;
    
        if(threadTrainerAux->valueEstimator <= config_values.estimacion_inicial){
            indexFirstTrainer = i;
        }       
    }
    threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, indexFirstTrainer);
    threadTrainerAux->state = EXEC;
}

//Called when a pokemon appear, on deadlock thread, and on message "caught pokemon"
void calculateLeaveBlockedFromAppear(){
    calculateTrainerToReady(BLOCKED);
}

void calculateLeaveBlockedFromDeadlock(uint32_t idTrainer, t_position positionTo){
    t_threadTrainer* threadTrainerAux;
    uint32_t threadsTrainersCount = list_size(threadsTrainers);
    bool continueFor = true;

    for(int i=0; i<threadsTrainersCount && continueFor; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->idTrainer == idTrainer){
            continueFor = false;
            threadTrainerAux->positionTo.posx = positionTo.posx;
            threadTrainerAux->positionTo.posy = positionTo.posy;
            threadTrainerAux->state = READY;
            calculateTrainerToReady(READY);
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
            if(list_size(trainerAux->pokemonNeeded) == list_size(trainerAux->pokemonOwned)){
                t_list* copyOfNeeded = list_duplicate(trainerAux->pokemonNeeded);
                t_list* copyOfOwned = list_duplicate(trainerAux->pokemonOwned);
                bool listsAreIdenticals = true;

                list_sort(copyOfNeeded, compareStrings);
                list_sort(copyOfOwned, compareStrings);

                for(int j=0; j<list_size(copyOfNeeded) && listsAreIdenticals; j++){
                    if(strcmp((char*)list_get(copyOfNeeded, j), (char*)list_get(copyOfOwned, j)) != 0){
                        listsAreIdenticals = false;
                    }
                }

                if(listsAreIdenticals){
                    //Needed and owned same size and same contents, go to exit
                    threadTrainerAux->state = EXIT;
                    calculateTrainerInExit(idTrainer);
                }
            }
        }
    }
}

bool compareStrings(void* string1, void* string2){
    return(strcmp((char*)string1, (char*)string2) < 0);
}

void calculateTrainerInExit(uint32_t idTrainer){
    writeTrainerMetrics(idTrainer);
    if(list_all_satisfy(threadsTrainers, trainerStateIsExit)){
        writeTeamMetrics();
        finishTeam();
    }
}

void writeTrainerMetrics(uint32_t idTrainer){
    //write trainer metrics
    t_threadTrainer* threadTrainerAux;

    for(int i = 0; i<list_size(threadsTrainers); i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);

        if(threadTrainerAux->idTrainer == idTrainer) {
            log_info(obligatory_logger, "Trainer %d", idTrainer);
            log_info(obligatory_logger, "Total Cycle CPU by Trainer", threadTrainerAux->cpuCycleCount);
        }
        else{
            log_info(obligatory_logger, "Trainer %d not valid", idTrainer);
        }
    }
}

bool trainerStateIsExit(void* threadTrainer){
    return ((t_threadTrainer*)threadTrainer)->state == EXIT;
}

void writeTeamMetrics(){
    //write team metrics
    t_threadTrainer* threadTrainerAux;
    uint32_t totalCycleCpuTeam;
    uint32_t totalContextSwitch;

    for(int i = 0; i<list_size(threadsTrainers); i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        totalCycleCpuTeam += threadTrainerAux->cpuCycleCount;
        totalContextSwitch += threadTrainerAux->contextSwitchCount;
               
    }
      log_info(obligatory_logger, "Algorithm %s ", config_values.algoritmo_planificacion); 
      log_info(obligatory_logger, "Total Cycle CPU by TEAM %d", totalCycleCpuTeam);
      log_info(obligatory_logger, "Total Context Switch by Team %d", totalContextSwitch);
}

void finishTeam(){
    //finish team
    release_resources();
}

//// algoritmos de planifiacion
//despues de  setTrainerToExec_FirstCome() obtengo el threadTrainerChosen y el pokemonOnMap
void execThreadTrainerSetedFCFS(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){
	uint32_t success;
	void* pokemon;
	uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
//calculo rafaga de cpu FCFS con la distancia calculada
	calculate_cpu_burst_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
	move_to_objetive_FCFS(threadTrainerChosen, pokemon_on_map);

	connection* connectionMessage = malloc(sizeof(connection));
	if(connectionMessage != NULL) {
	//envio send catch_pokemon al broker, una vez que estoy en la posicion del pokemon
		send_catch_pokemon(socket_team, pokemon_on_map);
		threadTrainerChosen->state = BLOCKED; 
	//espero  rta del broker
		reconnect* reconnectMessage = malloc(sizeof(reconnect));
		send_reconnect(socket_broker, reconnectMessage->id_connection);
		pthread_t reconnection_broker_caught_pokemon;
        pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
	}
	else {
		success = caught_default();
		log_info(optional_logger, "Caught by Default.")	;
		log_info(optional_logger, "Pokemon %s", pokemon_on_map.pokemon);	
		}
		if(success == 0) {
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon= list_find(pokemonsOnMap, isPokemonOnMapChasing);
		pokemon_on_map.pokemon = (char*)pokemon;
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
		}
	//verifico si cumpli el objetivo
	calculateLeaveBlockedFromCaught(threadTrainerChosen->idTrainer);
	threadTrainerChosen->contextSwitchCount++;
}
void execThreadTrainerSetedRR(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){
	uint32_t success;
	void* pokemon;
	uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
//la rafaga total de cpu va a estar temporizada por el quantum
	calculate_cpu_burst_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
	move_to_objetive_RR(threadTrainerChosen, pokemon_on_map, distanceToMove);

connection* connectionMessage = malloc(sizeof(connection));
	if(connectionMessage != NULL) {
	//envio send catch_pokemon al broker, una vez que estoy en la posicion del pokemon
		send_catch_pokemon(socket_team, pokemon_on_map);
		threadTrainerChosen->state = BLOCKED; 
	//espero  rta del broker
		reconnect* reconnectMessage = malloc(sizeof(reconnect));
		send_reconnect(socket_broker, reconnectMessage->id_connection);
		pthread_t reconnection_broker_caught_pokemon;
        pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
	}
	else {
		success = caught_default();
		log_info(optional_logger, "Caught by Default.")	;
		log_info(optional_logger, "Pokemon %s", pokemon_on_map.pokemon);	
		}
		if(success == 0) {
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon= list_find(pokemonsOnMap, isPokemonOnMapChasing);
		pokemon_on_map.pokemon = (char*)pokemon;
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
		}
	//verifico si cumpli el objetivo?
	calculateLeaveBlockedFromCaught(threadTrainerChosen->idTrainer);
	threadTrainerChosen->contextSwitchCount++;
}
void execThreadTrainerSetedSJF_SD(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){
//una vez que se hizo el set del trainer elegido para el EXEC, 
// la ejecucion de la rafaga es igual que  con FCFS?
// de ser asi, reuso funciones de move y de calculo de rafaga de cpu
uint32_t success;
void* pokemon;
uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
//calculo rafaga de cpu SJF-SD con la distancia calculada
calculate_cpu_burst_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
move_to_objetive_FCFS(threadTrainerChosen, pokemon_on_map);

connection* connectionMessage = malloc(sizeof(connection));
	if(connectionMessage != NULL) {
	//envio send catch_pokemon al broker, una vez que estoy en la posicion del pokemon
		send_catch_pokemon(socket_team, pokemon_on_map);
		threadTrainerChosen->state = BLOCKED; 
	//espero  rta del broker
		reconnect* reconnectMessage = malloc(sizeof(reconnect));
		send_reconnect(socket_broker, reconnectMessage->id_connection);
		pthread_t reconnection_broker_caught_pokemon;
        pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
	}
	else {
		success = caught_default();
		log_info(optional_logger, "Caught by Default.")	;
		log_info(optional_logger, "Pokemon %s", pokemon_on_map.pokemon);	
		}
		if(success == 0) {
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon= list_find(pokemonsOnMap, isPokemonOnMapChasing);
		pokemon_on_map.pokemon = (char*)pokemon;
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
		}
	//verifico si cumpli el objetivo?
	calculateLeaveBlockedFromCaught(threadTrainerChosen->idTrainer);
	threadTrainerChosen->contextSwitchCount++;
}

void execThreadTrainerSetedSJF_CD(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){
	uint32_t success;
	void* pokemon;
	uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
	//calculo rafaga de cpu SJF-SD con la distancia calculada
	calculate_cpu_burst_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
	move_to_objetive_SJF_CD(threadTrainerChosen, pokemon_on_map, distanceToMove);
	//funcion mover de SJF_CD
	connection* connectionMessage = malloc(sizeof(connection));
	if(connectionMessage != NULL) {
	//envio send catch_pokemon al broker, una vez que estoy en la posicion del pokemon
		send_catch_pokemon(socket_team, pokemon_on_map);
		threadTrainerChosen->state = BLOCKED; 
	//espero  rta del broker
		reconnect* reconnectMessage = malloc(sizeof(reconnect));
		send_reconnect(socket_broker, reconnectMessage->id_connection);
		pthread_t reconnection_broker_caught_pokemon;
        pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
	}
	else {
		success = caught_default();
		log_info(optional_logger, "Caught by Default.")	;
		log_info(optional_logger, "Pokemon %s", pokemon_on_map.pokemon);	
		}
		if(success == 0) {
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon= list_find(pokemonsOnMap, isPokemonOnMapChasing);
		pokemon_on_map.pokemon = (char*)pokemon;
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
	}
	//verifico si cumpli el objetivo?
	calculateLeaveBlockedFromCaught(threadTrainerChosen->idTrainer);
	threadTrainerChosen->contextSwitchCount++;
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