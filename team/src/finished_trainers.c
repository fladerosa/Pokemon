#include "finished_trainers.h"

t_list* finishis = NULL;

void create_finished_list(){
    if(!finishis)
    finishis = list_create();
}

void close_finished_list(){
    if(finishis)
    list_destroy(finishis);
}