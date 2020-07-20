#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "inicializacion.h"
#include "team.h"

char* pokemonCompareDeadlock; //variable used ONLY to detect pokemon needed in deadlock
bool flagExistsDeadlock;

void testDeadlock();
void* detectDeadlock();
void detectDeadlock_do();
t_list* getPokemonsNeeded(t_trainer* trainerAux);
bool comparePokemonDeadlock(void* pokemonOwn);
bool isCandidateDeadlock(t_trainer* blockedTrainer);
bool completeCycleDeadlock();
bool trainerHasPokemonNoNeeded(t_trainer* trainerAux, char* pokemonNeeded);
bool existsDeadlock();
void setInterchangePokemon();

#endif