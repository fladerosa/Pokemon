#ifndef NEWPOKEMON_H_
#define NEWPOKEMON_H_

#include "common_utils.h"
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "fileSystemTallGrass.h"
#include <math.h>

char* crearBloque(new_pokemon* newPokemon);

typedef struct{
    uint32_t posicionX; 
    uint32_t posicionY;
    uint32_t cantidad;
}positionQuantity;

void newPokemonTallGrass(new_pokemon* newPokemon);
void createMetadataPokemon(char* directory, new_pokemon* newPokemon);
char* crearBloque(new_pokemon* newPokemon);
void configMetadataCreate(char* metadata);
void addBlockMetadata(char* metadata, char* block);
void abrirMetadata(char* metadata);
void cerrarMetadata(char* metadata);
int metadataBlocks(char* metadata);
void agregarDatosYOrdenarBloques(char* metadata, new_pokemon* newPokemon);
t_list* levantarBloquesAMemoria(char** bloques, int cantidadBloques);
bool coincidePosicion(void* elem, void* args);
void* structALinea(void* posicion);
void* concatenarStrings(void* str1, void* str2);

#endif