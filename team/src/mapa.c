#include "mapa.h"
#include <string.h>

void planificarProceso(){
	while(1){
		if (strcmp(config_values.algoritmo_planificacion, "FIFO") == 0) planificarProcesoFIFO();
		else if (strcmp(config_values.algoritmo_planificacion, "RR") == 0) planificarProcesoRR();
		else if (strcmp(config_values.algoritmo_planificacion, "SJF-SD") == 0) planificarProcesoSJF_SD();
		else if (strcmp(config_values.algoritmo_planificacion, "SJF-CD") == 0) planificarProcesoSJF_CD();
		else exit(1);
	}
}

void create_state_queue() {
ready_queue = queue_create();
finish_queue = queue_create();

}

bool is_trainer_more_closer(t_list* trainers, t_pokemon *pokemon) {

}

void move_to_objetivo(t_queue* ready_queue, t_pokemon* pokemon) {

}

bool get_objetive(t_list* trainers, t_list* toCaught) {

}
void planificarProcesoFIFO() {

}

void planificarProcesoRR() {
	
}

void planificarProcesoSJF_SD() {

}

void planificarProcesoSJF_CD() {

}