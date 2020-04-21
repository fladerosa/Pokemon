
#include "configuracion.h"
void escucharAGameBoy()
{
   iniciar_servidor(config);
  
}
/*
void establecerConexionBroker()
{
    int conexion = crear_conexion(valores.ip_broker, valores.puerto_broker);
    enviar_mensaje("Prueba", conexion);
}

void stop_server(int socketServer)
{
    close(socketServer);
}
*/
void iniciar_servidor(t_config *config)
{
	uint32_t socket_team;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(config_get_string_value(config, "IP"), config_get_string_value(config, "PUERTO"), &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_team = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            log_error(optional_logger, "No se pudo crear el socket\n");
            continue;

        if (bind(socket_team, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_team);
            log_error(optional_logger, "No se pudo establecer la conexion con el socket\n");
            continue;
        }
        break;
    }
	listen(socket_team, SOMAXCONN);
    log_info(optional_logger, "Escuchando en la ip %s y el puerto %s", valores.ip_team, valores.puerto_team);
    freeaddrinfo(servinfo);

    while(1)
    	esperar_a_gameboy(socket_team);
}
void esperar_a_gameboy(uint32_t socket_team)
{
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	uint32_t socket_gameboy = accept(socket_team, (void*) &dir_cliente, &tam_direccion);
    if(socket_gameboy < 0)
    {
        log_info(optional_logger, "No se acepto la conexion\n");
    }
    else{
        log_info(optional_logger, "Conexion establecida con el Servidor \n");
    }

	pthread_create(&thread_gameboy,NULL,(void*)serve_client,&socket_gameboy);
	pthread_detach(thread_gameboy);

}
void cerrar_servidor()
{
   // close(socket_team);
}
void leer_config()
{   
    char* config_path = "./cfg/team.config";
    config = config_create(config_path); 
    if(config == NULL)   
    {
        error_show("Error al leer el config del proceso TEAM en %s\n", config_path);
        exit(CONFIG_FAIL);
    }           
}
void cargar_posiciones_config_team()
{     
    char** posicion_config =  config_get_array_value(config, "POSICIONES_ENTRENADORES");
    valores.posicion_entrenador= list_create();
    string_iterate_lines(posicion_config, imprimir_posicion);
    list_iterate(valores.posicion_entrenador, mostrar);
}
void agregar_posicion_a_la_lista(char *posicion) {
    if (posicion != NULL) {
      list_add(valores.posicion_entrenador, posicion);
    }
  }
void imprimir_posicion(char *valor) {
    if(valor != NULL) {
      char **posiciones = string_split(valor, "|");
      string_iterate_lines(posiciones, agregar_posicion_a_la_lista);
      printf("Leo: %s\n", valor);
    } else {
      printf("Vacio\n");
    }
  }
void mostrar(void *elemento) {
    printf("El elemento: %s\n", (char *)elemento);
  }

void cargar_pokemones_config_team()
{
    char** pokemon_config = config_get_array_value(config, "POKEMON_ENTRENADORES");
    valores.pokemon_entrenador = list_create();
    string_iterate_lines(pokemon_config, imprimir_pokemon);
    list_iterate(valores.pokemon_entrenador, mostrar);

}
void agregar_pokemon_a_la_lista(char *pokemon) {
    if (pokemon != NULL) {
      list_add(valores.pokemon_entrenador, pokemon);
    }
  }
void imprimir_pokemon(char *valor) {
    if(valor != NULL) {
      char **pokemones = string_split(valor, "|");
      string_iterate_lines(pokemones, agregar_pokemon_a_la_lista);
      printf("Leo: %s\n", valor);
    } else {
      printf("Vacio\n");
    }
  }
void cargar_objetivos_config_team()
{
   char** objetivo_config = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
   valores.objetivo_entrenador = list_create();
   string_iterate_lines(objetivo_config, imprimir_objetivo);
   list_iterate(valores.objetivo_entrenador, mostrar);
}

void agregar_objetivo_a_la_lista(char *objetivo) {
    if (objetivo != NULL) {
      list_add(valores.objetivo_entrenador, objetivo);
    }
  }
void imprimir_objetivo(char *valor) {
    if(valor != NULL) {
      char **objetivos = string_split(valor, "|");
      string_iterate_lines(objetivos, agregar_objetivo_a_la_lista);
      printf("Leo: %s\n", valor);
    } else {
      printf("Vacio\n");
    }
  }
void cargar_valores_config(t_config * config)
{
    cargar_posiciones_config_team();
    cargar_pokemones_config_team();
    cargar_objetivos_config_team();
    valores.tiempo_reconexion = (uint32_t)config_get_int_value(config, "TIEMPO_RECONEXION");
    valores.retardo_ciclo_cpu = (uint32_t)config_get_int_value(config, "RETARDO_CICLO_CPU");
    valores.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    valores.quantum = (uint32_t)config_get_int_value(config, "QUANTUM");
    valores.estimacion_inicial = (uint32_t)config_get_int_value(config, "ESTIMACION_INICIAL");
    valores.ip_broker = config_get_string_value(config, "IP_BROKER");
    valores.puerto_broker= config_get_string_value(config, "PUERTO_BROKER");
    valores.ip_team = config_get_string_value(config, "IP_TEAM");
    valores.puerto_team = config_get_string_value(config, "PUERTO_TEAM");
}

void crear_logger_obligatorio()
{      
    char* log_config = config_get_string_value(config, "LOG_FILE");
    obligatory_logger = log_create(log_config, "TEAM", 1, LOG_LEVEL_INFO);
    if(obligatory_logger == NULL)
    {
        error_show("Error al crear el logger del proceso TEAM en %s\n", log_config);
        exit(LOG_FAIL);
    }
    log_info(obligatory_logger, "Log Obligatorio creado correctamente\n");
}
 
 void crear_logger_opcional()
{      
    char* log_config = config_get_string_value(config, "LOG_FILE_OPTIONAL");
    optional_logger = log_create(log_config, "TEAM", 1, LOG_LEVEL_INFO);
    if(optional_logger == NULL)
    {
        error_show("Error al crear el logger del proceso TEAM en %s\n", log_config);
        exit(LOG_FAIL);
    }
    log_info(optional_logger, "Log Opcional creado correctamente\n");
}
void inicializar()
{    
    leer_config(config);
    crear_logger_obligatorio();
    crear_logger_opcional();
    cargar_valores_config(config);
    log_info(optional_logger, "Inicializacion y carga de configuracion exitosa\n", LOG_LEVEL_INFO);
}

void liberar_recursos()
{ 
    if(config)
        config_destroy(config);

    if(obligatory_logger)
        log_destroy(obligatory_logger);

    if(optional_logger)
        log_destroy(optional_logger);
     
   //  list_destroy_and_destroy_elements(valores.posicion_entrenador,posicion);
     //list_destroy_and_destroy_elements(valores.pokemon_entrenador, pokemon);
     //list_destroy_and_destroy_elements(valores.objetivo_entrenador, objetivo);
  //  cerrar_servidor(socket_team);
   // close(conexion);
}

enum t_algoritmo obtenerAlgoritmo()
{
	enum t_algoritmo esAlgoritmo;
	char* sAlgort = valores.algoritmo_planificacion;
    if(string_equals_ignore_case(sAlgort, "FIFO"))
		esAlgoritmo = fifo;
	else if(string_equals_ignore_case(sAlgort, "RR"))
		esAlgoritmo = rr;
	else if(string_equals_ignore_case(sAlgort, "SJF-SD"))
		esAlgoritmo = sjf_sd;
	else if(string_equals_ignore_case(sAlgort, "SJF-SD"))
		esAlgoritmo = sjf_cd;
	return esAlgoritmo;
}
