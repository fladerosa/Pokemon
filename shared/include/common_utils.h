#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/process.h>
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
bool server_running;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posx;
	uint32_t posy;
	uint32_t quantity;
}new_pokemon;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posx;
	uint32_t posy;
	uint32_t id_message;
}appeared_pokemon;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posx;
	uint32_t posy;
}catch_pokemon;

typedef struct{
	uint32_t id_message;
	uint32_t sizeCaught;
	char* caught;
}caught_pokemon;


typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
}get_pokemon;

typedef struct{
	char* colaMensajes;
	uint32_t sizeColaMensajes;
}subscriptor;

#endif