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
	t_list positions; //al serializar se tiene que agregar la cantidad de posiciones
}localized_pokemon;

typedef struct{
	uint32_t posx; 
	uint32_t posy; 
}t_position;

typedef struct{
	uint32_t sizeColaMensajes;
	char* colaMensajes;
}subscriptor;

#endif