#include "gameboy.h"

int create_connection(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

int main(int argc, char ** argv){

    t_log* obligatory_logger;
    t_log* optional_logger; 

    t_config* config;

    //Inicializando la config
    config = config_create("./cfg/gameboy.config");

    int showConsole = config_get_int_value(config,"LOG_SHOW");

    //Inicializando el logger
    obligatory_logger = log_create("./cfg/obligatory.log", "obligatory", showConsole, LOG_LEVEL_INFO); 
    optional_logger = log_create("./cfg/optional.log", "optional", showConsole, LOG_LEVEL_INFO);

    char* ip; 
    char* port;

    char* server = argv[0];
    //Busco el ip y el puerto
    if(strcmp(server,"BROKER")){
        ip = config_get_string_value(config, "IP_BROKER");
        port = config_get_string_value(config, "PUERTO_BROKER");
    }else if(strcmp(server,"TEAM")){
        ip = config_get_string_value(config, "IP_TEAM");
        port = config_get_string_value(config, "PUERTO_TEAM");
    }else if(strcmp(server,"GAMECARD")){
        ip = config_get_string_value(config, "IP_GAMECARD");
        port = config_get_string_value(config, "PUERTO_GAMECARD");
    }else{
        log_error(optional_logger,"No se pudo conectar");
        exit(4);
    }

    int connection = create_connection(ip, port);


    closeAll(optional_logger, obligatory_logger,config,connection);
    return 0;
}

closeAll(t_log* optional_logger,t_log* obligatory_logger, t_config* config, int connection){
    log_destroy(optional_logger);
    log_destroy(obligatory_logger);
    config_destroy(config);
    close(connection);
}