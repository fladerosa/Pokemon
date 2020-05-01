#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <commons/collections/list.h>
#include "ready_coaches.h"
#include "running_coaches.h"
#include "blocked_coaches.h"
#include "finished_coaches.h"


void inicialize_process_queue();
void close_process_queue();

#endif