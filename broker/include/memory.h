#ifndef MEMORY_H
#define MEMORY_H

#include <commons/collections/list.h>
#include "common_utils.h"
#include "initializer.h"

typedef enum {
	FREE = 1,
	OCCUPIED = 2
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
    int countFailedSearchForCompact;
} t_memory_configuration;

typedef struct t_memory{
    t_memory_configuration configuration;
    t_list* partitions;
    int failedSearchCount;
} t_memory;

t_memory memory;

void initializeMemory();
void addData(int sizeData, void* data);

t_data* seekPartitionAvailable(int sizeData);
t_data* getPartitionAvailable(int sizeData);
bool verifMustCompact();
void compact();
void destroyPartition();
void allocateData(int sizeData, void* data, t_data* freePartition);

#endif