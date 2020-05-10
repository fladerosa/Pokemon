#include "memory.h"

void initializeMemory(){
    memory.partitions = list_create();
    memory.configuration.memoryAlgorithm = cfg_values.algoritmo_memoria;
    memory.configuration.replaceAlgorithm = cfg_values.algoritmo_reemplazo;
    memory.configuration.freePartitionAlgorithm = cfg_values.algoritmo_particion_libre;
    memory.configuration.size = cfg_values.tamano_memoria;
    memory.configuration.minimunPartitionSize = cfg_values.tamano_minimo_particion;
    memory.configuration.countFailedSearchForCompact = cfg_values.frecuencia_compactacion;
   
    t_data data;
    data.size = memory.configuration.size;
    data.data = malloc(memory.configuration.size);
    data.state = FREE;

    list_add(memory.partitions, &data);
}

void addData(int sizeData, void* data){
    t_data* freePartition = seekPartitionAvailable(sizeData);

    allocateData(sizeData, data, freePartition);
}

t_data* seekPartitionAvailable(int sizeData){
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

t_data* getPartitionAvailable(int sizeData){
    if(strcmp(memory.configuration.freePartitionAlgorithm, "FF") == 0){
        return FF_getPartitionAvailable();
    }else{
        return BF_getPartitionAvailable();
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

void allocateData(int sizeData, void* data, t_data* freePartition){
    if(strcmp(memory.configuration.memoryAlgorithm, "BS") == 0){
        BS_allocateData(sizeData, data, freePartition);
    }else{
        DP_allocateData(sizeData, data, freePartition);
    }
}


//region memory algorithms
t_data* FF_getPartitionAvailable(){
    return NULL;
}
t_data* BF_getPartitionAvailable(){
    return NULL;
}

void BS_compact(){

}
void DP_compact(){

}

void FIFO_destroyPartition(){

}
void LRU_destroyPartition(){

}

void BS_allocateData(int sizeData, void* data, void* freePartition){

}
void DP_allocateData(int sizeData, void* data, void* freePartition){

}
//end region