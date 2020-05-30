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