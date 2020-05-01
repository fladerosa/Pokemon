#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/process.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdint.h>
#include "common_connections.h"
#include "exit_status.h"

t_log* obligatory_logger,* optional_logger;
t_config* config;
pthread_t server;

typedef struct{
	uint32_t posx; 
	uint32_t posy; 
}t_position;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	t_position position; 
	uint32_t quantity;
	uint32_t id_message; 
}new_pokemon;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	t_position position; 
	uint32_t id_message;
	uint32_t id_correlational;
}appeared_pokemon;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	t_position position; 
	uint32_t id_message; 
}catch_pokemon;

typedef struct{
	uint32_t id_message;
	uint32_t id_correlational;
	uint32_t success;
}caught_pokemon;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t id_message;
}get_pokemon;

typedef struct{
	uint32_t sizePokemon; 
	char* pokemon; 
	uint32_t id_message; 
	uint32_t id_correlational; 
	t_list* positions; //al serializar se tiene que agregar la cantidad de posiciones
}localized_pokemon;

typedef struct{
}new_connection;

typedef struct{
	uint32_t id_connection;
}reconnect;

typedef struct{
	uint32_t id_message;
}ack;

typedef struct{
	uint32_t id_connection; 
}connection;

typedef struct{
	uint32_t colaMensajes;
}subscribe;


new_pokemon* stream_to_new_pokemon(void* stream);
void* new_pokemon_to_stream(new_pokemon* newPokemonMessage);
appeared_pokemon* stream_to_appeared_pokemon(void* stream);
void* appeared_pokemon_to_stream(appeared_pokemon* appearedPokemonMessage);
catch_pokemon* stream_to_catch_pokemon(void* stream);
void* catch_pokemon_to_stream(catch_pokemon* catchPokemonMessage);
caught_pokemon* stream_to_caught_pokemon(void* stream);
void* caught_pokemon_to_stream(caught_pokemon* caughtPokemonMessage);
get_pokemon* stream_to_get_pokemon(void* stream);
void* get_pokemon_to_stream(get_pokemon* getPokemonMessage);
localized_pokemon* stream_to_localized_pokemon(void* stream);
void* localized_pokemon_to_stream(localized_pokemon* localizedPokemonMessage);
subscribe* stream_to_subscribe(void* stream);
void* subscribe_to_stream(subscribe* subscribeMessage);
new_connection* stream_to_new_connection(void* stream);
void* new_connection_to_stream(new_connection* newConnectionMessage);
reconnect* stream_to_reconnect(void* stream);
void* reconnect_to_stream(reconnect* reconnectMessage);
connection* stream_to_connection(void* stream);
void* connection_to_stream(connection* connectionMessage);
ack* stream_to_ack(void* stream);
void* ack_to_stream(ack* acknowledgementMessage);
new_pokemon* init_new_pokemon(char* nombre, uint32_t posx, uint32_t posy, uint32_t quantity, uint32_t id_message);
appeared_pokemon* init_appeared_pokemon(char* nombre, uint32_t posx, uint32_t posy, uint32_t id_message, uint32_t id_correlational);
catch_pokemon* init_catch_pokemon(char* nombre, uint32_t posx, uint32_t posy, uint32_t id_message);
caught_pokemon* init_caught_pokemon(uint32_t id_message, uint32_t id_correlational, bool success);
get_pokemon* init_get_pokemon(char* nombre, uint32_t id_message);
localized_pokemon* init_localized_pokemon(char* nombre, uint32_t id_message, uint32_t id_correlational, t_list* positions);
new_connection* init_new_connection();
reconnect* init_reconnect(uint32_t id_connection);
connection* init_connection(uint32_t id_connection);
ack* init_ack(uint32_t id_message);
subscribe* init_subscribe(uint32_t id_queue);
void free_ack(ack* acknowledgement);
void free_connection(connection* conn);
void free_reconnect(reconnect* reconn);
void free_new_connection(new_connection* conn);
void free_subscribe(subscribe* subscriber);
void free_localized_pokemon(localized_pokemon* pokemon);
void free_get_pokemon(get_pokemon* pokemon);
void free_caught_pokemon(caught_pokemon* pokemon);
void free_catch_pokemon(catch_pokemon* pokemon);
void free_appeared_pokemon(appeared_pokemon* pokemon);
void free_new_pokemon(new_pokemon* pokemon);


#endif