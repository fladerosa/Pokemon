#ifndef SUSCRIPCION_H
#define SUSCRIPCION_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"
#include "inicializacion.h"

/*
 estructura para definir pokemon y cantidad a capturar por entrenador / team
 ESTA FALTANDO COMPARAR AMBOS ARRAYS (pokemon* y g_pokemon*) 
 PARA DEFINIR LOS OBJETIVOS A CUMPLIR EN LA PLANIFICACION
*/
typedef struct{
    char* pokemon;
    uint32_t total_init_quantity_pokemon;
    uint32_t total_global_quantity_pokemon;
    uint32_t dif_init_team_quantity_pokemon;   
}pokemon_team;

op_code code;
uint32_t socket_team;
uint32_t socket_broker;
on_request request;

void listen_to_gameboy(t_configuration*);
void retry_on_x_time();
uint32_t caught_default(caught_pokemon*);
void localized_default(localized_pokemon*, pokemon_team*);

void connection_broker_global_suscribe();
void connection_broker_suscribe_to_appeared_pokemon();
void connection_broker_suscribe_to_caught_pokemon();
void connection_broker_suscribe_to_localized_pokemon();

void reception_message_queue_subscription(uint32_t, uint32_t, uint32_t);
void close_socket(void);
void new_thread_connection_broker_global_suscribe(void);

#endif