#include "memory.h"

void initializeMemory(){
    memory.partitions = list_create();
    memory.configuration.memoryAlgorithm = config_get_string_value(config, "ALGORITMO_MEMORIA");
    memory.configuration.replaceAlgorithm = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    memory.configuration.freePartitionAlgorithm = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
    memory.configuration.size = config_get_int_value(config, "TAMANO_MEMORIA");
    memory.configuration.minimunPartitionSize = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
   
    t_data data;
    data.size = memory.configuration.size;
    data.data = malloc(memory.configuration.size);
    data.state = UNUSED;

    list_add(memory.partitions, &data);
}

void addData(int sizeData, void* data){

}