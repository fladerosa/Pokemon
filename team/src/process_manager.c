#include "process_manager.h"

void inicialize_process_queue(){

    create_ready_list();
    create_running_list();
    create_blocked_list();
    create_finished_list();

}
void close_process_queue(){
        
    close_ready_list();
    close_running_list();
    close_blocked_list();
    close_finished_list();

}