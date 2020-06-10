#ifndef FILESYSTEMTALLGRASS_H_
#define FILESYSTEMTALLGRASS_H_

#include "common_utils.h"

typedef struct{
    uint32_t blocks;
    uint32_t blockSize;
}configMetadata;

configMetadata configM;

void crearBloques();
void obtenerConfig();

#endif