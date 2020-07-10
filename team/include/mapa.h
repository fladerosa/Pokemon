#ifndef MAPA_H
#define MAPA_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "common_utils.h"
#include "inicializacion.h"

typedef struct {
            t_list* time_cpu_trainer;
            uint32_t total_time_cpu_team;
            uint32_t total_context_switch;

}t_metrics;

//Determina si un entrenador de la lista esta mas cerca del pokemon que devuelve la cola localized 
bool its_more_closer(t_list*);
//calcular la distancia al objetivo
uint32_t distance_to_pokemon(t_list*);
//se debe agregar a la cola de procesos ready (eso determina el orden fifo)
t_list* trainer_more_closer();
void move_to_objetive(t_queue*);
//enviar mensaje caught a broker
//paso trainer a estado EXEC
//calculo el tiempo de cpu hasta ducha posicion

//esperando_respuesta_catch
// paso trainer a estado BLOCKED
// si pasa x cantidad de tiempo, mando reconexion y uso al funcion por default
// agrego ese poke a la lista de owned

bool get_objetive(t_list*, t_list*);
//compara si los objetivos del entrenador se cumplieron (ambas listas, PokemonNeeded y toCaught, son iguales)
//se encarga de determinar el orden de los procesos(entrenadores) a ready
void planificarProceso();
void planificarProcesoFIFO();
void planificarProcesoRR();
void planificarProcesoSJF_SD();
void planificarProcesoSJF_CD();
void create_state_queue();
t_queue* ready_queue;
t_queue** block_queue;
t_queue* finish_queue;

#endif