#include "deadlock.h"

void testDeadlock(){
    //Simple deadlock
    t_trainer* trainer1 = malloc(sizeof(t_trainer));
    t_trainer* trainer2 = malloc(sizeof(t_trainer));
    t_trainer* trainer3 = malloc(sizeof(t_trainer));
    t_trainer* trainer4 = malloc(sizeof(t_trainer));
    t_trainer* trainer5 = malloc(sizeof(t_trainer));
    t_threadTrainer* threadTrainer1 = malloc(sizeof(t_threadTrainer));
    t_threadTrainer* threadTrainer2 = malloc(sizeof(t_threadTrainer));
    t_threadTrainer* threadTrainer3 = malloc(sizeof(t_threadTrainer));
    t_threadTrainer* threadTrainer4 = malloc(sizeof(t_threadTrainer));
    t_threadTrainer* threadTrainer5 = malloc(sizeof(t_threadTrainer));
    threadsTrainers = list_create();
    trainers = list_create();

    threadTrainer1->state = BLOCKED;
    threadTrainer1->idTrainer = 1;
    list_add(threadsTrainers, threadTrainer1);

    threadTrainer2->state = BLOCKED;
    threadTrainer2->idTrainer = 2;
    list_add(threadsTrainers, threadTrainer2);

    threadTrainer3->state = BLOCKED;
    threadTrainer3->idTrainer = 3;
    list_add(threadsTrainers, threadTrainer3);

    threadTrainer4->state = READY;
    threadTrainer4->idTrainer = 4;
    list_add(threadsTrainers, threadTrainer4);

    threadTrainer5->state = READY;
    threadTrainer5->idTrainer = 5;
    list_add(threadsTrainers, threadTrainer5);

    trainer1->id_trainer = 1;
    trainer1->position.posx = 0;
    trainer1->position.posy = 0;
    trainer1->pokemonOwned = list_create();
    trainer1->pokemonNeeded = list_create();
    list_add(trainer1->pokemonOwned, (void*)"Squirtle");
    list_add(trainer1->pokemonOwned, (void*)"Tortoise");
    list_add(trainer1->pokemonNeeded, (void*)"Vulpix");
    list_add(trainer1->pokemonNeeded, (void*)"Charmander");
    list_add(trainers, trainer1);

    trainer2->id_trainer = 2;
    trainer2->position.posx = 1;
    trainer2->position.posy = 1;
    trainer2->pokemonOwned = list_create();
    trainer2->pokemonNeeded = list_create();
    list_add(trainer2->pokemonOwned, (void*)"Pikachu");
    list_add(trainer2->pokemonOwned, (void*)"Charmander");
    list_add(trainer2->pokemonNeeded, (void*)"Squirtle");
    list_add(trainer2->pokemonNeeded, (void*)"Tortoise");
    list_add(trainers, trainer2);

    trainer3->id_trainer = 3;
    trainer3->position.posx = 2;
    trainer3->position.posy = 2;
    trainer3->pokemonOwned = list_create();
    trainer3->pokemonNeeded = list_create();
    list_add(trainer3->pokemonOwned, (void*)"Vulpix");
    list_add(trainer3->pokemonOwned, (void*)"Charmander");
    list_add(trainer3->pokemonNeeded, (void*)"Pikachu");
    list_add(trainer3->pokemonNeeded, (void*)"Charmander");
    list_add(trainers, trainer3);

    trainer4->id_trainer = 4;
    trainer4->position.posx = 3;
    trainer4->position.posy = 3;
    trainer4->pokemonOwned = list_create();
    trainer4->pokemonNeeded = list_create();
    list_add(trainer4->pokemonOwned, (void*)"Pidgey");
    list_add(trainer4->pokemonOwned, (void*)"Pidgey");
    list_add(trainer4->pokemonNeeded, (void*)"Pidgey");
    list_add(trainer4->pokemonNeeded, (void*)"Pidgey");
    list_add(trainers, trainer4);

    trainer5->id_trainer = 5;
    trainer5->position.posx = 4;
    trainer5->position.posy = 4;
    trainer5->pokemonOwned = list_create();
    trainer5->pokemonNeeded = list_create();
    list_add(trainer5->pokemonOwned, (void*)"Snorlax");
    list_add(trainer5->pokemonOwned, (void*)"Snorlax");
    list_add(trainer5->pokemonNeeded, (void*)"Snorlax");
    list_add(trainer5->pokemonNeeded, (void*)"Snorlax");
    list_add(trainers, trainer5);

    detectDeadlock_do();
}

void* detectDeadlock(){
    while(true){
        sleep(4);
        detectDeadlock_do();
    }
    return NULL;
}

void detectDeadlock_do(){
    //If exists a cycle of blocked trainers without disponible space for more pokemon, exists deadlock
    int threadsTrainersCount = list_size(threadsTrainers);
    t_threadTrainer* threadTrainerAux;
    t_trainer* trainerAux;
    t_cycleDeadlock* deadlockNode;
    char* pokemonNeededAux;
    bool continueAnalize = true;

    if(flagExistsDeadlock) return;

    log_info(obligatory_logger, "Inicia algoritmo de detección de deadlock.");
    for(int i = 0; i < threadsTrainersCount && continueAnalize; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        trainerAux = (t_trainer*)list_get(trainers, i);
        //log_info(optional_logger, "DL - Trainer %d captured pokemon: %s", trainerAux->id_trainer, (char*)list_get(trainerAux->pokemonOwned, 0));
	    //log_info(optional_logger, "DL - Trainer %d needs pokemon: %s", trainerAux->id_trainer, (char*)list_get(trainerAux->pokemonNeeded, 0));
        if(threadTrainerAux->state == BLOCKED && isCandidateDeadlock(trainerAux)){
            t_list* pokemonsNeeded = getPokemonsNeeded(trainerAux);
            for(int j = 0; j < list_size(pokemonsNeeded); j++){
                pokemonNeededAux = (char*)list_get(pokemonsNeeded, j);
                cycleDeadLock = list_create();
                deadlockNode = malloc(sizeof(t_cycleDeadlock));
                deadlockNode->idTrainer = trainerAux->id_trainer;
                deadlockNode->pokemon = malloc(strlen(pokemonNeededAux));
                strcpy(deadlockNode->pokemon, pokemonNeededAux);
                list_add(cycleDeadLock, (void*)deadlockNode);
                log_info(optional_logger, "Inicia analisis DL - Trainer %d, needed pokemon: %s", deadlockNode->idTrainer, deadlockNode->pokemon);
                if(!completeCycleDeadlock()){
                    list_remove(cycleDeadLock, 0);
                    list_destroy(cycleDeadLock);
                    log_info(optional_logger, "Lista descartada 1");
                }else{
                    if(existsDeadlock()){
                        log_info(obligatory_logger, "Existe deadlock.");
                        log_info(optional_logger, "Existe deadlock");
                        flagExistsDeadlock = true;
                        setInterchangePokemon();
                        continueAnalize = false;
                    }else{
                        log_info(optional_logger, "Lista descartada 2");
                        list_destroy(cycleDeadLock);
                    }
                }
            }
        }
    }

    if(continueAnalize){
        log_info(obligatory_logger, "No se detectó deadlock.");
    }else{
        log_info(obligatory_logger, "Se detectó deadlock.");
    }
}

t_list* getPokemonsNeeded(t_trainer* trainerAux){
    uint32_t pokemonsNeededCount = list_size(trainerAux->pokemonNeeded);
    char* pokemonNeededAux;
    t_list* pokemonsNeeded = list_create();

    for (int i=0; i<pokemonsNeededCount; i++){
        pokemonNeededAux = (char*)list_get(trainerAux->pokemonNeeded, i);
        pokemonCompareDeadlock = malloc(strlen(pokemonNeededAux));
        strcpy(pokemonCompareDeadlock, pokemonNeededAux);
        if(!list_any_satisfy(trainerAux->pokemonOwned, comparePokemonDeadlock)){
            list_add(pokemonsNeeded, pokemonNeededAux);
        }
        free(pokemonCompareDeadlock);
    }

    return pokemonsNeeded;
}

bool comparePokemonDeadlock(void* pokemonOwn){
    return (strcmp((char*)pokemonOwn, pokemonCompareDeadlock) == 0);
}

bool isCandidateDeadlock(t_trainer* blockedTrainer){
    if(list_size(blockedTrainer->pokemonOwned) != list_size(blockedTrainer->pokemonNeeded)){
        return false;
    }

    //If its on blocked state, and cant get another pokemon we assume that needs some pokemon that doesnot have
    return true;
}

bool completeCycleDeadlock(){
    //the cycle already has the elements, so i look for the next to add
    t_cycleDeadlock* deadlockLastNode = (t_cycleDeadlock*)list_get(cycleDeadLock, list_size(cycleDeadLock) - 1);
    int threadsTrainersCount = list_size(threadsTrainers);
    t_threadTrainer* threadTrainerAux;
    t_trainer* trainerAux;
    for(int i = 0; i<threadsTrainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        trainerAux = (t_trainer*)list_get(trainers, i);
        if(threadTrainerAux->state == BLOCKED && isCandidateDeadlock(trainerAux) && trainerHasPokemonNoNeeded(trainerAux, deadlockLastNode->pokemon)){
            t_list* pokemonsNeeded = getPokemonsNeeded(trainerAux);
            for(int j = 0; j < list_size(pokemonsNeeded); j++){
                char* pokemonNeededAux = (char*)list_get(pokemonsNeeded, j);
                t_cycleDeadlock* deadlockNode = malloc(sizeof(t_cycleDeadlock));
                deadlockNode->idTrainer = trainerAux->id_trainer;
                deadlockNode->pokemon = malloc(strlen(pokemonNeededAux));
                strcpy(deadlockNode->pokemon, pokemonNeededAux);
                list_add(cycleDeadLock, (void*)deadlockNode);
                log_info(optional_logger, "Agrega DL - Trainer %d, needed pokemon: %s", deadlockNode->idTrainer, deadlockNode->pokemon);
                if(existsDeadlock()){
                    return true;
                }else{
                    if(!completeCycleDeadlock()){
                        list_remove(cycleDeadLock, list_size(cycleDeadLock)-1);
                    }else{
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool trainerHasPokemonNoNeeded(t_trainer* trainerAux, char* pokemonNeeded){
    pokemonCompareDeadlock = malloc(strlen(pokemonNeeded));
    strcpy(pokemonCompareDeadlock, pokemonNeeded);
    bool result = (list_any_satisfy(trainerAux->pokemonOwned, comparePokemonDeadlock)) && (!list_any_satisfy(trainerAux->pokemonNeeded, comparePokemonDeadlock));
    free(pokemonCompareDeadlock);
    return result;
}

bool existsDeadlock(){
    t_cycleDeadlock* firstNode = (t_cycleDeadlock*)list_get(cycleDeadLock, 0);
    t_cycleDeadlock* lastNode = (t_cycleDeadlock*)list_get(cycleDeadLock, list_size(cycleDeadLock)-1);

    return firstNode->idTrainer == lastNode->idTrainer && strcmp(firstNode->pokemon, lastNode->pokemon) == 0;
}

void setInterchangePokemon(){
    t_cycleDeadlock* cycleDeadlockAux = (t_cycleDeadlock*)list_get(cycleDeadLock, 0);
    t_threadTrainer* threadTrainerToMove = (t_threadTrainer*)list_get(threadsTrainers, cycleDeadlockAux->idTrainer - 1);
    cycleDeadlockAux = (t_cycleDeadlock*)list_get(cycleDeadLock, 1);
    t_trainer* trainerWithDestiny = (t_trainer*)list_get(trainers, cycleDeadlockAux->idTrainer - 1);

    log_info(obligatory_logger, "El entrenador %d intercambiará con el entrenador %d", threadTrainerToMove->idTrainer, trainerWithDestiny->id_trainer);

    threadTrainerToMove->positionTo.posx = trainerWithDestiny->position.posx;
    threadTrainerToMove->positionTo.posy = trainerWithDestiny->position.posy;

    calculateLeaveBlockedFromDeadlock(threadTrainerToMove->idTrainer);
}