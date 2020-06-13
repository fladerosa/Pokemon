#include "tests.h"


void broker_run_tests(){
    CU_initialize_registry();
    CU_pSuite broker_tests = CU_add_suite("BROKER Suite",NULL,NULL);
    //CU_add_test(broker_tests,"Agregar mensaje a queue",test_agregar_a_queue);
    CU_add_test(broker_tests,"Test manejo a memoria",manejo_memoria);
    run_tests();
}

void test_agregar_a_queue(){
    CU_ASSERT_TRUE(true);
}

void manejo_memoria(){
    initialize();
    initializeMemory();
    CU_ASSERT_TRUE(testSeekAlgorithms());
    initializeMemory();
    CU_ASSERT_TRUE(testAllocateDataBS());
    initializeMemory();
    CU_ASSERT_TRUE(testAllocateDataDP());
    initializeMemory();
    CU_ASSERT_TRUE(testDestroyPartitionFIFO());
    initializeMemory();
    CU_ASSERT_TRUE(testDestroyPartitionLRU());
    initializeMemory();
    CU_ASSERT_TRUE(testCompactDP());
    /*
    casos de prueba particiones dinamicas
        ubicar dato con memoria vacía
        ubicar dato para verificar First Fit
        ubicar dato para verificar Best Fit
        ubicar dato con memoria llena para verificar compactacion
        ubicar dato con memoria llena para verificar elección de victima FIFO
        ubicar dato con memoria llena para verificar elección de victima LRU

    casos de prueba Buddy System
        ubicar dato con memoria vacía
        ubicar dato para verificar First Fit
        ubicar dato para verificar Best Fit
        ubicar dato con memoria llena para verificar compactacion
        ubicar dato con memoria llena para verificar elección de victima FIFO
        ubicar dato con memoria llena para verificar elección de victima LRU
    */
}

bool testSeekAlgorithms(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    data = BF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    return true;
}

bool testAllocateDataBS(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    BS_allocateData(128, data);
    if(list_size(memory.partitions) != 5){
        return false;
    }
    return true;
}

bool testAllocateDataDP(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    DP_allocateData(128, data);
    if(list_size(memory.partitions) != 2){
        return false;
    }
    return true;
}

bool testDestroyPartitionFIFO(){
    //Context
    internalStringData(1, "Primer dato");
    internalStringData(2, "Segundo dato");
    if(list_size(memory.partitions) != 3){
        return false;
    }
    FIFO_destroyPartition();
    t_data* firstPartition = (t_data*)list_get(memory.partitions, 0);
    if(firstPartition->state != FREE){
        return false;
    }
    t_data* secondPartition = (t_data*)list_get(memory.partitions, 1);
    if(secondPartition->state == FREE){
        return false;
    }
    void* partitionData = getData(2);
    if(strcmp(partitionData, "Segundo dato") != 0 ){
        return false;
    }
    return true;
}
    
bool testDestroyPartitionLRU(){
    //Context
    internalStringData(1, "Primer dato");
    internalStringData(2, "Segundo dato");
    if(list_size(memory.partitions) != 3){
        return false;
    }

    t_data* firstPartition = (t_data*)list_get(memory.partitions, 0);
    sleep(3);
    void* partitionData = getData(1);

    LRU_destroyPartition();
    firstPartition = (t_data*)list_get(memory.partitions, 0);
    if(firstPartition->state == FREE){
        return false;
    }
    t_data* secondPartition = (t_data*)list_get(memory.partitions, 1);
    if(secondPartition->state != FREE){
        return false;
    }
    partitionData = getData(1);
    if(strcmp(partitionData, "Primer dato") != 0 ){
        return false;
    }
    return true;
}

void internalStringData(uint32_t id, char* text){
    int sizeText = strlen(text)+1;
    void* strData = mallocMemory(id, sizeText);
    memcpy(strData, text, sizeText);
}

bool testCompactDP(){
    //Context
    memory.configuration.memoryAlgorithm = "DP";
    internalStringData(1, "Primer dato");
    internalStringData(2, "Segundo dato");
    internalStringData(3, "Tercer dato");
    if(list_size(memory.partitions) != 4){
        return false;
    }

    sleep(3);
    void* partitionData1 = getData(1);
    void* partitionData2 = getData(3);

    LRU_destroyPartition();
    DP_compact();
    if(list_size(memory.partitions) != 3){
        return false;
    }

    partitionData1 = getData(1);
    partitionData2 = getData(3);
    if(strcmp(partitionData1, "Primer dato") != 0 ){
        return false;
    }
    if(strcmp(partitionData2, "Tercer dato") != 0 ){
        return false;
    }
    return true;
}