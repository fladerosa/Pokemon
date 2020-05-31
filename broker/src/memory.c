#include "memory.h"

void initializeMemory(){
    memory.partitions = list_create();
    memory.configuration.memoryAlgorithm = cfg_values.algoritmo_memoria;
    memory.configuration.replaceAlgorithm = cfg_values.algoritmo_reemplazo;
    memory.configuration.freePartitionAlgorithm = cfg_values.algoritmo_particion_libre;
    memory.configuration.size = cfg_values.tamano_memoria;
    memory.configuration.minimunPartitionSize = cfg_values.tamano_minimo_particion;
    memory.configuration.countFailedSearchForCompact = cfg_values.frecuencia_compactacion;
    memory.data = malloc(memory.configuration.size);
   
    t_data data;
    data.size = memory.configuration.size;
    data.offset = 0;
    data.state = FREE;

    list_add(memory.partitions, &data);
}

void addData(uint32_t sizeData, void* data){
    t_data* freePartition = seekPartitionAvailable(sizeData);

    allocateData(sizeData, data, freePartition);
}

t_data* seekPartitionAvailable(uint32_t sizeData){
    t_data* freePartition = getPartitionAvailable(sizeData);

    if(freePartition == NULL){
        if(verifMustCompact()){
            compact();
        }else{
            destroyPartition();
        }
        return seekPartitionAvailable(sizeData);
    }else{
        return freePartition;
    }
}

t_data* getPartitionAvailable(uint32_t sizeData){
    if(strcmp(memory.configuration.freePartitionAlgorithm, "FF") == 0){
        return FF_getPartitionAvailable(sizeData);
    }else{
        return BF_getPartitionAvailable(sizeData);
    }
}

bool verifMustCompact(){
    memory.failedSearchCount++;
    return memory.failedSearchCount == memory.configuration.countFailedSearchForCompact;
}

void compact(){
    if(strcmp(memory.configuration.memoryAlgorithm, "BS") == 0){
        BS_compact();
    }else{
        DP_compact();
    }
}

void destroyPartition(){
    if(strcmp(memory.configuration.replaceAlgorithm, "FIFO") == 0){
        FIFO_destroyPartition();
    }else{
        LRU_destroyPartition();
    }
}

void allocateData(uint32_t sizeData, void* data, t_data* freePartition){
    if(strcmp(memory.configuration.memoryAlgorithm, "BS") == 0){
        BS_allocateData(sizeData, data, freePartition);
    }else{
        DP_allocateData(sizeData, data, freePartition);
    }
}


//region memory algorithms
t_data* FF_getPartitionAvailable(uint32_t sizeData){
    return (t_data*)list_find_with_args(memory.partitions, partition_size_validation,(void*)sizeData);
}
t_data* BF_getPartitionAvailable(uint32_t sizeData){
    uint32_t sizeList =  list_size(memory.partitions);
    uint32_t minimunSize = sizeData;
    t_data* dataAux;
    for(int i = 0; i < sizeList; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(dataAux->size < minimunSize){
            minimunSize = dataAux->size;
        }
    }
    return FF_getPartitionAvailable(minimunSize);
}

bool partition_size_validation(void* data, void* sizeData){
    return data ? ((t_data*) data)->size > (uint32_t) sizeData : false;
}

void BS_compact(){

}
void DP_compact(){

}

void FIFO_destroyPartition(){

}
void LRU_destroyPartition(){

}

void BS_allocateData(uint32_t sizeData, void* data, t_data* freePartitionData){
    //t_data* freePartitionData = *freePartition;
    
    if(sizeData <= freePartitionData->size / 2){
        t_data newData;
        newData.size = freePartitionData->size - sizeData;
        newData.offset = freePartitionData->offset + sizeData;
        newData.state = FREE;
        list_add(memory.partitions, &newData);
        freePartitionData->size = freePartitionData->size / 2;
        BS_allocateData(sizeData, data, freePartitionData);
    }else{
        memcpy(memory.data + freePartitionData->offset, data, sizeData);
    }
}
/*
void BS_allocateData_test(uint32_t sizeData, void* data, t_data freePartition){
   return;
}
void BS_allocateData_test2(uint32_t sizeData, void* data, t_data** freePartition){
   return;
}
*/

void DP_allocateData(uint32_t sizeData, void* data, t_data* freePartitionData){
    //t_data* freePartitionData = (t_data*)freePartition;
    if(sizeData != freePartitionData->size){
        //Si los tamaños no coinciden debo realizar una partición
        t_data newData;
        newData.size = freePartitionData->size - sizeData;
        newData.offset = freePartitionData->offset + sizeData;
        newData.state = FREE;
        list_add(memory.partitions, &newData);
    }
    freePartitionData->size = sizeData;
    memcpy(memory.data + freePartitionData->offset, data, sizeData);
}
//end region