#include "dump.h"

void dump(int signum){
    log_info(optional_logger, "Dumping cache into %s", cfg_values.dump_file);
    //TODO
    log_info(optional_logger, "Finished dumping successfully");
}