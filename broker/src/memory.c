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
    memset(memory.data, '\0', memory.configuration.size);
   
    t_data* data = malloc(sizeof(t_data));
    data->size = memory.configuration.size;
    data->offset = 0;
    data->state = FREE;

    list_add(memory.partitions, data);
}

void* getData(t_data* data){
    //TODO si el retorno debe ser un puntero a la memoria se debe cambiar aca
    void* dataReturn = malloc(data->size);
    memcpy(dataReturn, memory.data + data->offset, data->size);
    return dataReturn;
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
    if(memory.configuration.countFailedSearchForCompact == -1){
        return list_all_satisfy(memory.partitions, partition_is_free);
    }
    return memory.failedSearchCount == memory.configuration.countFailedSearchForCompact;
}

bool partition_is_free(void* data) {
    t_data* partition = (t_data*)data;
    return partition->state == FREE;
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
        if(dataAux->state == FREE && dataAux->size < minimunSize){
            minimunSize = dataAux->size;
        }
    }
    return FF_getPartitionAvailable(minimunSize);
}

bool partition_size_validation(void* data, void* sizeData){
    return data ? ((t_data*) data)->size > (uint32_t) sizeData && ((t_data*) data)->state == FREE : false;
}

void BS_compact(){
    //Debería ir uniendo las particiones contiguas de igual tamaño
}
void DP_compact(){
    //Debería ir desplazando todos los datos a la izquierda y uniendo todas las particiones libres
}

void FIFO_destroyPartition(){
    time_t minimunTime = time(NULL);
    uint32_t sizeList =  list_size(memory.partitions);
    uint32_t indexFinded = 0;
    t_data* dataAux;
    for(int i = 0; i < sizeList; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(dataAux->state == USING && difftime(dataAux->creationTime, minimunTime) < 0){
            minimunTime = dataAux->creationTime;
            indexFinded = i;
        }
    }
    t_data* partitionSelected = (t_data*)list_get(memory.partitions, indexFinded);
    partitionSelected->state = FREE;
}
void LRU_destroyPartition(){
    time_t oldestTime = time(NULL);
    uint32_t sizeList =  list_size(memory.partitions);
    uint32_t indexFinded = 0;
    t_data* dataAux;
    for(int i = 0; i < sizeList; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(dataAux->state == USING && difftime(dataAux->lastTimeUsed, oldestTime) < 0){
            oldestTime = dataAux->lastTimeUsed;
            indexFinded = i;
        }
    }
    t_data* partitionSelected = (t_data*)list_get(memory.partitions, indexFinded);
    partitionSelected->state = FREE;
}

void BS_allocateData(uint32_t sizeData, void* data, t_data* freePartitionData){
    if(sizeData <= freePartitionData->size / 2){
        t_data* newData = malloc(sizeof(t_data));
        newData->size = freePartitionData->size - sizeData;
        newData->offset = freePartitionData->offset + sizeData;
        newData->state = FREE;
        list_add(memory.partitions, newData);
        freePartitionData->size = freePartitionData->size / 2;
        BS_allocateData(sizeData, data, freePartitionData);
    }else{
        freePartitionData->creationTime = time(NULL);
        freePartitionData->lastTimeUsed = time(NULL);
        freePartitionData->state = USING;
        memcpy(memory.data + freePartitionData->offset, data, sizeData);
    }
}

void DP_allocateData(uint32_t sizeData, void* data, t_data* freePartitionData){
    if(sizeData != freePartitionData->size){
        //If the size of the data is bigger than the free space, its create a new partition
        t_data* newData = malloc(sizeof(t_data));
        newData->size = freePartitionData->size - sizeData;
        newData->offset = freePartitionData->offset + sizeData;
        newData->state = FREE;
        list_add(memory.partitions, newData);
    }
    freePartitionData->size = sizeData;
    freePartitionData->creationTime = time(NULL);
    freePartitionData->lastTimeUsed = time(NULL);
    freePartitionData->state = USING;
    memcpy(memory.data + freePartitionData->offset, data, sizeData);
}
//end region