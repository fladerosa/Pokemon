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
   
    t_data* data = malloc(sizeof(t_data));
    data->size = memory.configuration.size;
    data->offset = 0;
    data->state = FREE;

    list_add(memory.partitions, data);
}

void* mallocMemory(uint32_t idMensaje, uint32_t size){
    t_data* freePartition = seekPartitionAvailable(size);
    freePartition->id = idMensaje;
    allocateData(size, freePartition);
    return memory.data + freePartition->offset;
}
void setIdQueue(uint32_t idQueue, uint32_t idMensaje){
    t_data* partition = (t_data*)list_find_with_args(memory.partitions, partition_match_id_mensaje,(void*)idMensaje);
    if(partition == NULL) return;
    partition->idQueue = idQueue;
}
void* getData(uint32_t idMensaje){
    t_data* partition = (t_data*)list_find_with_args(memory.partitions, partition_match_id_mensaje,(void*)idMensaje);
    if(partition == NULL) return NULL;
    partition->lastTimeUsed = time(NULL);
    return memory.data + partition->offset;
}
bool partition_match_id_mensaje(void* data, void* idMensaje){
    return data ? ((t_data*) data)->id == (uint32_t) idMensaje && ((t_data*) data)->state == USING : false;
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
    memory.configuration.countFailedSearchForCompact = 0;
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

void allocateData(uint32_t sizeData, t_data* freePartition){
    if(strcmp(memory.configuration.memoryAlgorithm, "BS") == 0){
        BS_allocateData(sizeData, freePartition);
    }else{
        DP_allocateData(sizeData, freePartition);
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
    return data ? ((t_data*) data)->size >= (uint32_t) sizeData && ((t_data*) data)->state == FREE : false;
}

void BS_compact(){
    //Debería unir solo cuando son particiones del "mismo bloque"
    uint32_t sizeList =  list_size(memory.partitions);
    t_data* previousPartition = NULL;
    t_data* dataAux;
    bool mustFinish = false;
    for(int i = 0; i < sizeList && !mustFinish; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(previousPartition == NULL){
            previousPartition = dataAux;
        }else{
            if(previousPartition->size == dataAux->size){
                if(previousPartition->state == FREE && dataAux->state == FREE){
                    //I join them
                    previousPartition->size += dataAux->size;
                    list_remove(memory.partitions, i);
                    mustFinish = true;
                }else{
                    previousPartition = NULL;
                }
            }else{
                previousPartition = dataAux;
            }
        }
        if(mustFinish) BS_compact();
    }
    
}
void DP_compact(){
    //It moves the partitions with state using to the init an join the free partitions
    uint32_t sizeList =  list_size(memory.partitions);
    t_data* freePartition = NULL;
    t_data* dataAux;
    uint32_t intAux;
    t_list* indexPartitionsToRemove = list_create();
    for(int i = 0; i < sizeList; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(dataAux->state == FREE){
            if(freePartition == NULL) {
                //First free partition founded
                freePartition = dataAux;
            } else {
                //I already had a free partition, i join them
                freePartition->size += dataAux->size;
                void* dato = malloc(sizeof(int));
                memcpy(dato, (void*)&i, sizeof(int));
                list_add(indexPartitionsToRemove, dato);
            }
        }else{
            if(freePartition == NULL){
                //I do nothing
            }else{
                //I had a previous free space, so i move the data
                memcpy(memory.data + freePartition->offset, memory.data + dataAux->offset, dataAux->size);

                intAux = freePartition->size;
                freePartition->size = dataAux->size;
                dataAux->size = intAux;

                dataAux->offset = freePartition->offset + dataAux->size;

                freePartition->lastTimeUsed = dataAux->lastTimeUsed;
                freePartition->creationTime = dataAux->creationTime;
                freePartition->id = dataAux->id;
                freePartition->state = dataAux->state;
                dataAux->state = FREE;
                dataAux->id = 0;

                freePartition = dataAux;
            }
        }
    }

    sizeList = list_size(indexPartitionsToRemove);
    for(int i = 0; i < sizeList; i++){
        uint32_t* index = (uint32_t*)list_get(indexPartitionsToRemove, i);
        list_remove(memory.partitions, *index);
    }
}

void FIFO_destroyPartition(){
    time_t minimunTime = time(NULL);
    uint32_t sizeList =  list_size(memory.partitions);
    uint32_t indexFinded = 0;
    t_data* dataAux;
    for(int i = 0; i < sizeList; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(dataAux->state == USING && difftime(dataAux->creationTime, minimunTime) <= 0){
            minimunTime = dataAux->creationTime;
            indexFinded = i;
        }
    }
    t_data* partitionSelected = (t_data*)list_get(memory.partitions, indexFinded);
    partitionSelected->state = FREE;
    list_destroy(partitionSelected->receivers);
    pthread_mutex_destroy(partitionSelected->m_receivers_modify);
}
void LRU_destroyPartition(){
    time_t oldestTime = time(NULL);
    uint32_t sizeList =  list_size(memory.partitions);
    uint32_t indexFinded = 0;
    t_data* dataAux;
    for(int i = 0; i < sizeList; i++){
        dataAux = (t_data*)list_get(memory.partitions, i);
        if(dataAux->state == USING && difftime(dataAux->lastTimeUsed, oldestTime) <= 0){
            oldestTime = dataAux->lastTimeUsed;
            indexFinded = i;
        }
    }
    t_data* partitionSelected = (t_data*)list_get(memory.partitions, indexFinded);
    partitionSelected->state = FREE;
    list_destroy(partitionSelected->receivers);
    pthread_mutex_destroy(partitionSelected->m_receivers_modify);
}

void BS_allocateData(uint32_t sizeData, t_data* freePartitionData){
    if(sizeData <= freePartitionData->size / 2){
        t_data* newData = malloc(sizeof(t_data));
        newData->size = freePartitionData->size / 2;
        newData->offset = freePartitionData->offset + newData->size;
        newData->state = FREE;
        list_add(memory.partitions, newData);
        freePartitionData->size = freePartitionData->size / 2;
        BS_allocateData(sizeData, newData);
    }else{
        freePartitionData->creationTime = time(NULL);
        freePartitionData->lastTimeUsed = time(NULL);
        freePartitionData->state = USING;
    }
}

void DP_allocateData(uint32_t sizeData, t_data* freePartitionData){
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
}

void dumpMemory(){
    log_info(optional_logger, "Dumping cache into %s", cfg_values.dump_file);
    FILE* file = txt_open_for_append(cfg_values.dump_file);

    txt_write_in_file(file, "------------------------------------------------------------------------------\n");
    dump_write_time(file);
    dump_partitions(file);
    txt_write_in_file(file, "------------------------------------------------------------------------------\n");

    txt_close_file(file);
    log_info(optional_logger, "Finished dumping.", cfg_values.dump_file);
}
void dump_write_time(FILE* file){
    time_t timer;
    char* timeFormated = malloc(26);
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(timeFormated, 26, "%d/%m/%Y %H:%M:%S", tm_info);
    char* prefix = "Dump: ";
    char* text = malloc(26 + strlen(prefix) + 1);
    strcpy(text, prefix);
    strcat(text, timeFormated);
    txt_write_in_file(file, text);
    txt_write_in_file(file, "\n");
}
void dump_partitions(FILE* file){
    uint32_t sizeList = list_size(memory.partitions);
    char* strFormat_using = "Partición %d: %p - %p. [X] Size: %db LRU: %li Cola: %d ID:%d\n";
    char* str_using = malloc(strlen(strFormat_using) + sizeof(void*)*2 + sizeof(int) * 4 + sizeof(long));
    char* strFormat_free = "Partición %d: %p - %p. [L] Size: %db\n";
    char* str_free = malloc(strlen(strFormat_free) + sizeof(void*)*2 + sizeof(int) * 2);
    void* initialPointer;
    void* endPointer;
    for(int i = 0; i < sizeList; i++){
        t_data* partition = list_get(memory.partitions, i);
        initialPointer = memory.data + partition->offset;
        endPointer = initialPointer + partition->size;//TODO mis dudas
        if(partition->state == FREE){
            sprintf(str_free, strFormat_free, i, initialPointer, endPointer, partition->size);
            txt_write_in_file(file, str_free);
        }else{
            sprintf(str_using, strFormat_using, i, initialPointer, endPointer, partition->size, partition->lastTimeUsed, partition->idQueue, partition->id);
            txt_write_in_file(file, str_using);
        }
    }
}

t_data* assign_and_return_message(uint32_t id_queue, uint32_t sizeofrawstream, void* stream){
    uint32_t sizeofdata;
    t_data* freePartition;
    switch(id_queue){
        case NEW_POKEMON:
        case CATCH_POKEMON:
        case GET_POKEMON:
            sizeofdata = sizeofrawstream - sizeof(uint32_t);
            freePartition = seekPartitionAvailable(sizeofdata);
            break;
        case APPEARED_POKEMON:
        case CAUGHT_POKEMON:
        case LOCALIZED_POKEMON:
            sizeofdata = sizeofrawstream - 2 * sizeof(uint32_t);
            freePartition = seekPartitionAvailable(sizeofdata);
            memcpy(&freePartition->id_correlational, stream + sizeofdata + sizeof(uint32_t), sizeof(uint32_t));
            break;
        default:
            return NULL;
    }
    allocateData(sizeofdata, freePartition);
    void* data = memory.data + freePartition->offset;
    memcpy(data, stream, sizeofdata);
    pthread_mutex_lock(&m_id_message);
    id_message++;
    freePartition->id = id_message;
    pthread_mutex_unlock(&m_id_message);
    freePartition->idQueue = id_queue;
    freePartition->receivers = list_create();
    freePartition->m_receivers_modify = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(freePartition->m_receivers_modify,NULL);
    return freePartition;
} 



//end region