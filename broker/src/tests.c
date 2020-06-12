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
    CU_ASSERT_TRUE(true);
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
    void* strData = malloc(128);
    strcpy(strData, "Hola test");
    BS_allocateData(128, strData, data);
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
    void* strData = malloc(128);
    strcpy(strData, "Hola test");
    DP_allocateData(128, strData, data);
    if(list_size(memory.partitions) != 2){
        return false;
    }
    return true;
}

bool testDestroyPartitionFIFO(){
    //Context
    char* cadena = "Primer dato";
    void* strData = malloc(strlen(cadena));
    strcpy(strData, cadena);
    addData(strlen(cadena), strData);
    cadena = "Segundo dato";
    strData = malloc(strlen(cadena));
    strcpy(strData, cadena);
    addData(strlen(cadena), strData);
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
    //char* partitionData = (char*)getData(secondPartition);
    void* partitionData = malloc(secondPartition->size);
    memcpy(partitionData, memory.data + secondPartition->offset, secondPartition->size);
    char* valor = malloc(secondPartition->size);
    strcpy(valor, partitionData);
    if(strcmp(partitionData, cadena) != 0 ){
        return false;
    }
    return true;
}
    
bool testDestroyPartitionLRU(){
    return true;
}