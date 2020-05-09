#ifndef MEMORY_H
#define MEMORY_H

#include <commons/collections/list.h>
#include "common_utils.h"

typedef enum {
	FREE = 1, //Partición libre
	OCCUPIED = 2, //Partición ocupada
    UNUSED //Partición no usada, lista para dividirse
}e_dataState;

typedef struct t_data{
    uint32_t size;
    void* data;
    char* queueAssociated;
    int id;
    e_dataState state;
    int usedTime;
} t_data;

typedef struct t_memory_configuration{
    char* memoryAlgorithm;
    char* replaceAlgorithm;
    char* freePartitionAlgorithm;
    int size;
    int minimunPartitionSize;
    int compactationFrecuency;
} t_memory_configuration;

typedef struct t_memory{
    t_memory_configuration configuration;
    t_list* partitions;
    int failedSearchCount;
} t_memory;

t_memory memory;

void initializeMemory();
void addData(int sizeData, void* data);

#endif