#ifndef NEWPOKEMON_H_
#define NEWPOKEMON_H_

#include "common_utils.h"
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "fileSystemTallGrass.h"
#include "commons_filesystem.h"

void newPokemonTallGrass(new_pokemon* newPokemon);
void createMetadataPokemon(char* directory, new_pokemon* newPokemon);
void configMetadataCreate(char* metadata);
void addBlockMetadata(char* metadata, char* block, new_pokemon* newPokemon);
void agregarDatosYOrdenarBloques(char* metadata, new_pokemon* newPokemon);


#endif