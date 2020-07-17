#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "inicializacion.h"

typedef struct{
    uint32_t idTrainer;
    char* pokemon;
} t_cycleDeadlock;

char* pokemonCompareDeadlock; //variable used ONLY to detect pokemon needed in deadlock
t_list* cycleDeadLock;

void testDeadlock();
void detectDeadlock();
t_list* getPokemonsNeeded(t_trainer* trainerAux);
bool comparePokemonDeadlock(void* pokemonOwn);
bool isCandidateDeadlock(t_trainer* blockedTrainer);
bool completeCycleDeadlock();
bool trainerHasPokemonNoNeeded(t_trainer* trainerAux, char* pokemonNeeded);
bool existsDeadlock();
void setInterchangePokemon();

#endif