#ifndef SUSCRIPCION_H
#define SUSCRIPCION_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"
#include "inicializacion.h"

typedef struct
{
    uint32_t idQueue;
    uint32_t socket;
    uint32_t idConnection;

} threadSubscribe;
//hilos colas de suscripcion
pthread_t suscripcionAppearedPokemon;
pthread_t suscripcionCaughtPokemon;
pthread_t suscripcionLocalizedPokemon;

pthread_mutex_t reconnectAppearedPokemon;
pthread_mutex_t reconnectCaughtPokemon;
pthread_mutex_t reconnectLocalizedPokemon;

threadSubscribe* structAppearedPokemon;
threadSubscribe* structCaughtPokemon;
threadSubscribe* structLocalizedPokemon;

t_list* threadSubscribeList; 

op_code code;
uint32_t socket_team;
uint32_t socket_broker;
on_request request;

//hilo puerto escucha gameboy
pthread_t listening_gameboy;

void connection_broker_global_suscribe();
void connection_broker_suscribe_to_appeared_pokemon(op_code, pthread_t);
void connection_broker_suscribe_to_caught_pokemon(op_code, pthread_t);
void connection_broker_suscribe_to_localized_pokemon(op_code, pthread_t);
args_pthread* thread_suscribe_arguments(op_code, uint32_t);
void suscribeOnThreadList(args_pthread*);
void listen_to_gameboy();
void reception_message_queue_subscription(uint32_t, uint32_t, uint32_t);
bool compareSockets(void*, void*);
void close_sockets();
void retry_on_x_time();
caught_pokemon* caught_default();
uint32_t localized_default(char*);
void send_message_queue_init(uint32_t, uint32_t, uint32_t);





#endif