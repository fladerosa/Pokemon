#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "common_utils.h"
#include "initializer.h"
#include "finalizer.h"

void process_request(uint32_t cod_op, uint32_t size, uint32_t cliente_fd);

#endif