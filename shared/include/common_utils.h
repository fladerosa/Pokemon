#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/process.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "exit_status.h"

t_log* obligatory_logger,* optional_logger;
t_config* config;

void funcionABorrar();


#endif