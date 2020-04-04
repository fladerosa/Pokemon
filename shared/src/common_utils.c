#include "common_utils.h"

void funcionABorrar(){
    t_log* logger = log_create("cfg/ejemplo.log","EJEMPLO", true, LOG_LEVEL_INFO);
    log_info(logger, "Este log viene de la shared");
}