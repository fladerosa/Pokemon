#include "initializer.h"

void initialize(){
    char* config_path = "cfg/broker.config";
    config = config_create(config_path);
    if (config == NULL) {
        error_show(
            "Configuration could not be loaded in %s. Aborting.\n", 
            config_path
        );
        exit(CONFIG_FAIL);
    }
    obligatory_logger = create_log_from_config("LOG_FILE", "BROKER", "LOG_SHOW");
    optional_logger = create_log_from_config(
        "OPTIONAL_LOG_FILE", "BROKER-D", "OPTIONAL_LOG_SHOW"
    );
    log_info(optional_logger, "Initializing Application...");
    fill_config_values();
    connections = list_create();
    pthread_mutex_init(&m_id_connection, NULL);
    pthread_mutex_init(&m_id_message, NULL);
    set_sig_handler();
    p_on_request = &process_request;
    log_info(optional_logger, "Configuration and loggers loaded successfully.");
}

t_log* create_log_from_config(char* path_key, char* name, char* show_key){
    char* log_path = config_get_string_value(config, path_key);
    uint32_t log_show = config_get_int_value(config, show_key);
    t_log* logger =  log_create(log_path, name, log_show, LOG_LEVEL_INFO);
    if (logger==NULL){
        error_show("Log %s in path: %s could not be loaded. Aborting.\n", name, log_path);
        exit(LOG_FAIL);
    }
    return logger;
}

void fill_config_values(){
    cfg_values.tamano_memoria = config_get_int_value(config, "TAMANO_MEMORIA");
    cfg_values.tamano_minimo_particion = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
    cfg_values.algoritmo_memoria = config_get_string_value(config, "ALGORITMO_MEMORIA");
    cfg_values.algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    cfg_values.algoritmo_particion_libre = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
    cfg_values.ip_broker = config_get_string_value(config, "IP_BROKER");
    cfg_values.puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
    cfg_values.frecuencia_compactacion = config_get_int_value(config, "FRECUENCIA_COMPACTACION");
    cfg_values.dump_file = config_get_string_value(config, "DUMP_FILE");
    log_info(optional_logger, 
        "Configuration values: \n\n\tTAMANO_MEMORIA=%d\n\tTAMANO_MINIMO_PARTICION=%d\n\tALGORITMO_MEMORIA=%s\n\tALGORITMO_REEMPLAZO=%s\n\tALGORITMO_PARTICION_LIBRE=%s\n\tIP_BROKER=%s\n\tPUERTO_BROKER=%s\n\tFRECUENCIA_COMPACTACION=%d\n\tDUMP_FILE=%s\n\n",
        cfg_values.tamano_memoria, cfg_values.tamano_minimo_particion, cfg_values.algoritmo_memoria,
        cfg_values.algoritmo_reemplazo, cfg_values.algoritmo_particion_libre, cfg_values.ip_broker,
        cfg_values.puerto_broker, cfg_values.frecuencia_compactacion, cfg_values.dump_file
    );
}

void set_sig_handler(void)
{
    struct sigaction* action = malloc(sizeof(sigaction));


    (*action).sa_flags = SA_SIGINFO; 
    (*action).sa_sigaction = dump;

    if (sigaction(SIGUSR1, action, NULL) == -1) { 
        perror("sigusr: sigaction");
        _exit(1);
    }

    free(action);
}