#ifndef NEWPOKEMON_H_
#define NEWPOKEMON_H_

#include "common_utils.h"
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "fileSystemTallGrass.h"
#include <math.h>
#include <commons/string.h>
#include <ctype.h>
#include <sys/file.h>

char* crearBloque(new_pokemon* newPokemon);

typedef struct{
    uint32_t posicionX; 
    uint32_t posicionY;
    uint32_t cantidad;
}positionQuantity; 

typedef struct{

    char* nombreDirectorio; 
    pthread_mutex_t mutex;
}mutexDirectory;

pthread_mutex_t mutexthreadSubscribeList;

pthread_mutex_t mutexBitmap;

t_list* mutexListDirectory; 
pthread_mutex_t mutexthreadSubscribeList;
pthread_mutex_t mutexListOfMutex;


void newPokemonTallGrass(new_pokemon* newPokemon);
void createMetadataPokemon(char* directory, new_pokemon* newPokemon);
char* crearBloque(new_pokemon* newPokemon);
void configMetadataCreate(char* metadata);
void addBlockMetadata(char* metadata, char* block, new_pokemon* newPokemon);
void abrirMetadata(char* metadata, char* pokemon);
void cerrarMetadata(char* metadata, char* pokemon);
int metadataBlocks(char* metadata, char* pokemon);
void agregarDatosYOrdenarBloques(char* metadata, new_pokemon* newPokemon);
t_list* levantarBloquesAMemoria(char** bloques, int cantidadBloques);
bool coincidePosicion(void* elem, void* args);
void* structALinea(void* posicion);
char* concatenarStrings(t_list* lista);
char* bajarBloquesADisco(t_list* lista, char** bloques, int cantidadBloques, char* nombrePokemon, uint32_t posx, uint32_t posy, uint32_t quantity, char* metadata);
bool esPokemon(void* elem, void* args);
void bloquearMetadata(char* pokemon);
void desbloquearMetadata(char* pokemon);

#endif