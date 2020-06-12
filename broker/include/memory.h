#ifndef MEMORY_H
#define MEMORY_H

#include <commons/collections/list.h>
#include "common_utils.h"
#include "initializer.h"

typedef enum {
	FREE = 1,
	USING = 2
}e_dataState;

typedef struct t_data{
    uint32_t size;
    uint32_t offset;
    char* queueAssociated;
    uint32_t id;
    e_dataState state;
    time_t lastTimeUsed;
    time_t creationTime;
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
    void* data;
} t_memory;

t_memory memory;

void initializeMemory();
void* getData(t_data* data);
void addData(uint32_t sizeData, void* data);

t_data* seekPartitionAvailable(uint32_t sizeData);
t_data* getPartitionAvailable(uint32_t sizeData);
bool verifMustCompact();
bool partition_is_free(void* data);
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

void BS_allocateData(uint32_t sizeData, void* data, t_data* freePartition);
void DP_allocateData(uint32_t sizeData, void* data, t_data* freePartition);
#endif