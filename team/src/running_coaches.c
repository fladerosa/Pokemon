#include "running_coaches.h"

t_list *runnings = NULL;

void create_running_list(){

        if(!runnings)
        runnings = list_create();
}
void close_running_list(){

    if(runnings)
    list_destroy(runnings);
}