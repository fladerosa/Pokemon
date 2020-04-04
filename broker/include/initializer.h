#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "common_utils.h"

typedef enum exit_status {
    CONFIG_FAIL,
    LOG_FAIL


} exit_status;

void initialize();
t_log* createLogFromConfig(char* path_key, char* name, char* show_key);

#endif