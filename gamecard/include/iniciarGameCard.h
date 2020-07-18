#ifndef INICIARGAMECARD_H_
#define INICIARGAMECARD_H_

#include "common_utils.h"
#include "fileSystemTallGrass.h"
#include "newPokemon.h"
#include <dirent.h>
 //#include <sys/types.h>

typedef struct
{
    uint32_t idCola;
    uint32_t socket;
    uint32_t idConnection;

} threadSubscribe;

uint32_t socket_broker;
on_request request;
char* PUERTO_BROKER;
char* IP_BROKER;

pthread_t suscripcionNewPokemon;
pthread_t suscripcionCatchPokemon;
pthread_t suscripcionGetPokemon;

threadSubscribe* structNewPokemon;
threadSubscribe* structCatchPokemon;
threadSubscribe* structGetPokemon;

t_list* threadSubscribeList;


void iniciarGameCard();
void suscribirseATodo();
void finalizarGameCard();
void connect_client();
void crearSuscripcion(uint32_t socket,op_code codeOperation, pthread_t* threadName);
void subscribeAndConnect(args_pthread* arguments);
bool compareSockets(void* element, void* args);
#endif