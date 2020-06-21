#ifndef MAPA_H
#define MAPA_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "common_utils.h"
#include "inicializacion.h"


typedef struct {
            t_list* time_cpu_trainer;
            uint32_t total_time_cpu_trainer;

}t_metrics;

//Determina si un entrenador de la lista esta mas cerca del pokemon que devuelve la cola localized 
bool is_trainer_more_closer(t_list*, t_pokemon *);
//calcular la distancia al objetivo
//se debe agregar a la cola de procesos ready (eso determina el orden fifo)

void move_to_objetivo(t_queue* , t_pokemon*);
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
void planificarProcesoFIFO();
void planificarProcesoRR();
void planificarProcesoSJF_SD();
void planificarProcesoSJF_CD();
void create_state_queue();
t_queue* ready_queue;
t_queue** block_queue;
t_queue* finish_queue;

#endif