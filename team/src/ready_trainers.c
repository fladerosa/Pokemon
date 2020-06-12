#include "ready_trainers.h"

t_list *readys = NULL;

void create_ready_list(){

    if(!readys)
    readys = list_create();

}

void close_ready_list(){
        if(readys)
        list_destroy(readys);

}


