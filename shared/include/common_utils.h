#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/process.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "common_connections.h"
#include "exit_status.h"

t_log* obligatory_logger,* optional_logger;
t_config* config;
pthread_t client_listener;

void funcionABorrar();


#endif