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
    uint32_t id;
    e_dataState state;
    uint32_t usedTime;
} t_data;

typedef struct t_memory_configuration{
    char* memoryAlgorithm;
    char* replaceAlgorithm;
    char* freePartitionAlgorithm;
    uint32_t size;
    uint32_t minimunPartitionSize;
    uint32_t countFailedSearchForCompact;
} t_memory_configuration;

typedef struct t_memory{
    t_memory_configuration configuration;
    t_list* partitions;
    uint32_t failedSearchCount;
} t_memory;

t_memory memory;

void initializeMemory();
void addData(uint32_t sizeData, void* data);

t_data* seekPartitionAvailable(uint32_t sizeData);
t_data* getPartitionAvailable(uint32_t sizeData);
bool verifMustCompact();
void compact();
void destroyPartition();
void allocateData(uint32_t sizeData, void* data, t_data* freePartition);

t_data* FF_getPartitionAvailable();
t_data* BF_getPartitionAvailable();

bool partition_size_validation(void* data, void* sizeData);

void BS_compact();
void DP_compact();

void FIFO_destroyPartition();
void LRU_destroyPartition();

void BS_allocateData(uint32_t sizeData, void* data, void* freePartition);
void DP_allocateData(uint32_t sizeData, void* data, void* freePartition);
#endif