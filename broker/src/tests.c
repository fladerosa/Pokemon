#include "tests.h"


void broker_run_tests(){
    CU_initialize_registry();
    CU_pSuite broker_tests = CU_add_suite("BROKER Suite",NULL,NULL);
    CU_add_test(broker_tests,"Agregar mensaje a queue",test_agregar_a_queue);
    run_tests();
}

void test_agregar_a_queue(){
    CU_ASSERT_TRUE(true);
}