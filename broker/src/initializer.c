#include "initializer.h"

void initialize(){
    char* config_path = "cfg/broker.config";
    config = config_create(config_path);
    if (config == NULL) {
        error_show(
            "Configuration could not be loaded in %s. Aborting.", 
            config_path
        );
        exit(CONFIG_FAIL);
    }
    obligatory_logger = createLogFromConfig("LOG_FILE", "BROKER", "LOG_SHOW");
    optional_logger = createLogFromConfig(
        "OPTIONAL_LOG_FILE", "BROKER-D", "OPTIONAL_LOG_SHOW"
    );

    log_info(optional_logger, "Configuration and logs loaded successfully.");
}

t_log* createLogFromConfig(char* path_key, char* name, char* show_key){
    char* log_path = config_get_string_value(config, path_key);
    int log_show = config_get_int_value(config, show_key);
    t_log* logger =  log_create(log_path, name, log_show, LOG_LEVEL_INFO);
    if (logger==NULL){
        error_show("Log %s in path: %s could not be loaded. Aborting.", name, log_path);
        exit(LOG_FAIL);
    }
    return logger;
}