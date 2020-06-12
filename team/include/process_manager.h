#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <commons/collections/list.h>
#include "ready_trainers.h"
#include "blocked_trainers.h"
#include "finished_trainers.h"


void inicialize_process_queue();
void close_process_queue();

#endif