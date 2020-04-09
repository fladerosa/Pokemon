#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/process.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "exit_status.h"

t_log* obligatory_logger,* optional_logger;
t_config* config;

typedef enum
{
	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,
	ERROR = 6
	
}op_code;

typedef struct{
	char* pokemon;
	uint32_t posx;
	uint32_t posy;
	uint32_t quantity;
}new_pokemon;

typedef struct{
	char* pokemon;
	uint32_t posx;
	uint32_t posy;
	uint32_t id_message;
}appeared_pokemon;

typedef struct{
	char* pokemon;
	uint32_t posx;
	uint32_t posy;
}catch_pokemon;

typedef struct{
	uint32_t id_message;
	bool caught;
}caught_pokemon;


typedef struct{
	char* pokemon;
}get_pokemon;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void funcionABorrar();
int create_connection(char *ip, char* puerto);
void* serializar_paquete(t_paquete* paquete, int *bytes);


#endif