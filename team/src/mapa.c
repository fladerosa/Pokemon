#include "mapa.h"
#include <string.h>

//despues de  setTrainerToExec_FirstCome() obtengo el threadTrainerChosen y el pokemonOnMap
void execThreadTrainerSetedFCFS(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){

uint32_t success;
uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
//calculo rafaga de cpu FCFS con la distancia calculada
calculate_cpu_burst_FF_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
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
			// falta
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
		}

	//falta
	//verifico si cumpli el objetivo?

}

uint32_t calculateDistance(t_position positionFrom, t_position positionTo){
    uint32_t posX = positionTo.posx - positionFrom.posx;
    uint32_t posY = positionTo.posy - positionFrom.posy;

    if(posX < 0) posX = posX * -1;
    if(posY < 0) posY = posY * -1;
    return posX + posY;
}
void calculate_cpu_burst_FF_threadTrainerOnExec(t_threadTrainer* threadTrainerChosen, uint32_t distanceToMove) {

time_t time_initial = time(0);
uint32_t cpuBurstFF = time_initial + distanceToMove;
threadTrainerChosen->cpuCycleCount = cpuBurstFF;

log_info(obligatory_logger, "Thread Trainer on state EXEC %d", threadTrainerChosen->idTrainer);
log_info(obligatory_logger, "CPU BURST %d", cpuBurstFF);
}

void move_to_objetive_FCFS(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){

threadTrainerChosen->positionTo.posx = pokemon_on_map.position.posx;
threadTrainerChosen->positionTo.posy = pokemon_on_map.position.posy;
pokemon_on_map.state = P_CHASING;

//al estar en P_CHASING, debe inhabilitarse de futuras capturas ?
}

void send_catch_pokemon(uint32_t client_fd, t_pokemon_on_map pokemon_on_map){

uint32_t* id_message = malloc(sizeof(uint32_t));
catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon));
void* stream =catch_pokemon_to_stream(catchPokemonMessage, id_message);

 send(client_fd, stream, sizeof(pokemon_on_map), MSG_WAITALL);
    log_info(optional_logger, "Sending Message Catch pokemon.");
	log_info(optional_logger, "Pokemon %s: ", pokemon_on_map.pokemon);

}

void execThreadTrainerSetedRR(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){

uint32_t success;
uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
//la rafaga total de cpu va a estar temporizada por el quantum
calculate_cpu_burst_RR_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
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
			// falta
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
		}

	//falta
	//verifico si cumpli el objetivo?

}

void move_to_objetive_RR(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map, uint32_t distanceToMove){

uint32_t quantumRR = config_values.quantum;

	if(distanceToMove <= quantumRR ) {
		threadTrainerChosen->positionTo.posx = pokemon_on_map.position.posx;
		threadTrainerChosen->positionTo.posy = pokemon_on_map.position.posy;
	}
		else{
			threadTrainerChosen->positionTo.posx += quantumRR;
			threadTrainerChosen->positionTo.posy += quantumRR;
		}
		pokemon_on_map.state = P_CHASING;
//al estar en P_CHASING, debe inhabilitarse de futuras capturas ?
	threadTrainerChosen->state = READY;
	threadTrainerChosen->contextSwitchCount++;
	
}

void calculate_cpu_burst_RR_threadTrainerOnExec(t_threadTrainer* threadTrainerChosen, uint32_t distanceToMove) {

time_t time_initial = time(0);
uint32_t cpuBurstRR;

	while(distanceToMove!=0) {
		threadTrainerChosen->cpuCycleCount++;
		cpuBurstRR = time_initial + config_values.quantum;
		distanceToMove-=config_values.quantum;
	}

log_info(obligatory_logger, "Thread Trainer on state EXEC %d", threadTrainerChosen->idTrainer);
log_info(obligatory_logger, "CPU BURST %d", cpuBurstRR);
}

void execThreadTrainerSetedSJF_SD(t_threadTrainer* threadTrainerChosen, t_pokemon_on_map pokemon_on_map){
//una vez que se hizo el set del trainer elegido para el EXEC, 
// la ejecucion de la rafaga es igual que  con FCFS?
// de ser asi, reuso funciones de move y de calculo de rafaga de cpu
uint32_t success;
uint32_t distanceToMove = calculateDistance(threadTrainerChosen->positionTo, pokemon_on_map.position);
//calculo rafaga de cpu SJF-SD con la distancia calculada
calculate_cpu_burst_FF_threadTrainerOnExec(threadTrainerChosen, distanceToMove);
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
			// falta
		//agrego el pokemon capturado a la lista de pokemon Owned	
		//sacarlo de la lista de pokemons en mapa 
		pokemon_on_map.state = P_CATCHED;
		log_info(optional_logger, "State catched %d", success);	
		log_info(optional_logger, "Adding to pokemon owned");
		}

	//falta
	//verifico si cumpli el objetivo?

}

void execThreadTrainerSetedSJF_CD(){};


