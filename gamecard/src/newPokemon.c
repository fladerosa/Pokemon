#include "newPokemon.h"

//void newPokemon(int id_mensaje,char* pokemon,coordenadaPokemon coord, int cantidad){
    //Verificar si esta el pokemon en el directorio Pokemon, si no esta crearlo
    //Verificar si no hay otro proceso que lo esté abriendo. En caso de que si lo haya finalizar hilo y reintentar en "TIEMPO_DE_REINTENTO_OPERACION"
    //Verificar si la coordenada ya existe dentro del archivo. En caso de existir, agregar la cantidad pasada por parámetro a la actual. En caso de no existir se debe agregar al final del archivo una nueva línea indicando la cantidad de Pokémon pasadas.
    //Cerrar el archivo
    //Enviar un mensaje que contenga:ID del mensaje recibido, Pokémon y Coordenada a la cola de mensajes "APPEARED_POKEMON" 
    /**En caso de que no se pueda establecer conexion con el broker notificarlo por Logs y continuar**/
//}

void newPokemonTallGrass(new_pokemon* newPokemon){
    char* directorio = "./TALL_GRASS/Files/";
    char buffer[50];   
    char* stream = malloc(newPokemon->sizePokemon + 1); //Ver que va adentro del sizeof
    memcpy(stream, newPokemon->pokemon, newPokemon->sizePokemon); 
    stream[newPokemon->sizePokemon] = '\0';

    strcpy(buffer, "");
    strcat(buffer, directorio);
    strcat(buffer, stream);

    char* directory = malloc(strlen(directorio) + newPokemon->sizePokemon + 2); //revisar 
    memcpy(directory, buffer, strlen(directorio) + newPokemon->sizePokemon + 1);
    memcpy(directory + strlen(directorio) + newPokemon->sizePokemon + 1, "\0", sizeof(char));
    //directory[strlen(directorio) + newPokemon->sizePokemon + 1] = '\0';

    int created = mkdir(directory, ACCESSPERMS);
    if(created == -1){
        printf("%d",errno);
        printf("No se pudo crear el directorio.\n");
    }else{
        printf("Directorio creado o abierto.\n");
    }
    
    createMetadataPokemon(directory, newPokemon);
    free(directory);
    free(stream);
}

void createMetadataPokemon(char* directory, new_pokemon* newPokemon){
    char* metadata = "/Metadata.bin";
    char* directorioMetadata = malloc(strlen(directory) + strlen(metadata) + 1); //Ver que va adentro del sizeof
    strcpy(directorioMetadata,"");
    strcat(directorioMetadata,directory);
    //memcpy(directorioMetadata, directory, strlen(directory));
    strcat(directorioMetadata, metadata);

    FILE* file = fopen(directorioMetadata,"ab+");

    //fseek(file, 0, SEEK_SET);

    //struct stat st;
    //stat(directorioMetadata,&st);
    //int sizeFile = st.st_size;
    fseek(file, 0, SEEK_END);
    int sizeFile = ftell(file); //fileno(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);

    if(sizeFile == 0){
        configMetadataCreate(directorioMetadata);
        
        //char* metadataInfo = "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=Y";
        //fwrite(metadataInfo, strlen(metadataInfo), 1, file);      
       /* 
        fseek(file, 27, SEEK_SET);
        fwrite(bin, strlen(bin), 1,file); //NO esta funcionando*/
    }

    if(metadataBlocks(directorioMetadata) == 0){
        char* bin = crearBloque(newPokemon);
        addBlockMetadata(directorioMetadata,bin);
        free(bin);
    }else{
        agregarDatosYOrdenarBloques(directorioMetadata, newPokemon);
    }
    
    free(directorioMetadata);
}

char* crearBloque(new_pokemon* newPokemon){
    char* directorio = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";

    size_t sizeBitmap = bitarray_get_max_bit(bitmap);
    int bin = 1;  
    while(bin <= (int)sizeBitmap + 1){
        //if(ch = "0"){
        bool testBit = bitarray_test_bit(bitmap, bin - 1);
        if(testBit == 0){
            char* binChar = malloc(10);
            //char binTochar = (char) bin;
            strcpy(binChar,"");
            sprintf(binChar, "%d", bin);
            char* directorioBloques = malloc(strlen(directorio) + strlen(binChar) + strlen(extension)+1); //ver de cambiarlo 
            strcpy(directorioBloques,"");
            strcat(directorioBloques,directorio);
            strcat(directorioBloques,binChar);
            strcat(directorioBloques,extension);
            
            //memcpy(directorioBloques, directorio, strlen(directorio));
            //memcpy(directorioBloques + strlen(directorio), binChar, sizeof(char));
            //memcpy(directorioBloques +  strlen(directorio) + sizeof(char), extension, strlen(extension));
            
            FILE* binary = fopen(directorioBloques,"wb"); 

            char* posX = malloc(10);
            strcpy(posX,"");
            sprintf(posX,"%d",newPokemon->position.posx);//(char); 

            char* posY = malloc(10);
            strcpy(posY,"");
            sprintf(posY,"%d",newPokemon->position.posy);//(char); 

            char* quantity = malloc(10);
            strcpy(quantity,"");
            sprintf(quantity,"%d",newPokemon->quantity);//(char); 

            char* writeBinary = malloc(strlen(posX) + strlen("-") + strlen(posY) + strlen("=") + 1); 
            strcpy(writeBinary,"");
            memcpy(writeBinary, posX, strlen(posX)); 
            memcpy(writeBinary + strlen(posX), "-", strlen("-")); 
            memcpy(writeBinary + strlen(posX) + strlen("-"), posY, strlen(posY));
            memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY), "=", strlen("="));
            memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY) + strlen("="), quantity, strlen(quantity));
            //memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY) + strlen("=") + strlen(quantity), "\n", strlen("\n"));
            strcat(writeBinary, "\n");

            fwrite(writeBinary, strlen(writeBinary), 1, binary);
            fclose(binary);
            bitarray_set_bit(bitmap, bin-1);
            free(posX);
            free(posY);
            free(quantity);
            free(directorioBloques);
            free(writeBinary);
            //imprimirBITARRAY(bitmap);
            return binChar;
            break; 
        }else{
            bin++;
        }
    }

    //fclose(bitmap);
    return NULL;
}


void configMetadataCreate(char* metadata){
    t_config* configMetadataTallGrass = config_create("./cfg/tall_grass_metadata.config");

    config_save_in_file(configMetadataTallGrass, metadata);
    config_destroy(configMetadataTallGrass);
}

void addBlockMetadata(char* metadata, char* block){
    t_config* configMetadataTallGrass = config_create(metadata);

    abrirMetadata(metadata);

    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil(size / configM.blockSize);
    int cantidadBloquesMasUno = cantidadBloques + 1;
    
    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");

    char* bloquesConfig = malloc((cantidadBloquesMasUno*2 + 2)*sizeof(char) + 1);
    strcpy(bloquesConfig,"");
    //bloques = realloc(bloques, sizeof(char*)*(size+1));
    //bloques[size] = block;

    strcat(bloquesConfig,"[");
    for(int i = 0; i < (cantidadBloques*2); (i = i + 2)){
        strcat(bloquesConfig,bloques[i]);
        strcat(bloquesConfig,",");
        //bloquesConfig[i]=*bloques[i];
        //bloquesConfig[i++]=",";
        //i++;
    }
    strcat(bloquesConfig,block); 
    strcat(bloquesConfig,"]");

    char* directorio = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";

    char* bloque = malloc(strlen(directorio) + sizeof(char) + strlen(extension));
    strcpy(bloque,"");
    strcat(bloque, directorio); 
    strcat(bloque, block); 
    strcat(bloque, extension);
    
    //memcpy(bloque, directorio, strlen(directorio));
    //memcpy(bloque + strlen(directorio), block, sizeof(char));
    //memcpy(bloque + strlen(directorio) + sizeof(char), extension, strlen(extension));
    //memcpy(bloque + strlen(directorio) + strlen(block) + strlen(extension), "\0", strlen("\0"));
    FILE* file = fopen(bloque,"rb");

    fseek(file, 0, SEEK_END);
    int sizeFile = ftell(file);
    fseek(file, 0, SEEK_SET);

    fclose(file);

    size += sizeFile;

    char* sizeChar = malloc(sizeof(uint32_t));
    strcpy(sizeChar, "");
    sprintf(sizeChar, "%d", size);

    config_set_value(configMetadataTallGrass, "SIZE", sizeChar);
    config_set_value(configMetadataTallGrass,"BLOCKS", bloquesConfig);

    config_save(configMetadataTallGrass);
    //bloques[size - 1] = block;
    //char** bloquesMasBloqueNuevo = malloc()
    config_destroy(configMetadataTallGrass);
    cerrarMetadata(metadata);
    free(bloque);
    free(bloquesConfig);
    free(bloques);
}

void abrirMetadata(char* metadata){
    t_config* configMetadataTallGrass = config_create(metadata);
    
    config_set_value(configMetadataTallGrass, "OPEN", "Y");

    config_save(configMetadataTallGrass);

    config_destroy(configMetadataTallGrass);
}

void cerrarMetadata(char* metadata){
    t_config* configMetadataTallGrass = config_create(metadata);
    
    config_set_value(configMetadataTallGrass, "OPEN", "N");

    config_save(configMetadataTallGrass);

    config_destroy(configMetadataTallGrass);
}

int metadataBlocks(char* metadata){
    t_config* configMetadataTallGrass = config_create(metadata);

    //char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");

    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil((float)size / configM.blockSize);

    config_destroy(configMetadataTallGrass);
    return cantidadBloques;
}

void agregarDatosYOrdenarBloques(char* metadata, new_pokemon* newPokemon){
    t_config* configMetadataTallGrass = config_create(metadata);

    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");
    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil((float)size / configM.blockSize);

    char* ultimoBloque = bloques[cantidadBloques-1]; 

    char* directorio = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";

    char* bloque = malloc(strlen(directorio) + sizeof(char) + strlen(extension)+1);
    strcpy(bloque,"");
    strcat(bloque, directorio); 
    strcat(bloque, ultimoBloque); 
    strcat(bloque, extension);

    FILE* file = fopen(bloque,"ab+");
    fseek(file, 0, SEEK_END);
    int sizeFile = ftell(file);
    fseek(file,0,SEEK_SET);

    if(sizeFile >= configM.blockSize){
        char* bin = crearBloque(newPokemon);
        addBlockMetadata(metadata,bin);
    }else{

        char* posX = malloc(10);
        strcpy(posX,"");
        sprintf(posX,"%d",newPokemon->position.posx);//(char); 

        char* posY = malloc(10);
        strcpy(posY,"");
        sprintf(posY,"%d",newPokemon->position.posy);//(char); 

        char* quantity = malloc(10);
        strcpy(quantity,"");
        sprintf(quantity,"%d",newPokemon->quantity);//(char); 

        t_list* lista = levantarBloquesAMemoria(bloques, cantidadBloques);

        positionQuantity* posicionNewPokemon = malloc(sizeof(positionQuantity));
        posicionNewPokemon->posicionX = atoi(posX); 
        posicionNewPokemon->posicionY = atoi(posY);
        posicionNewPokemon->cantidad = atoi(quantity);

        positionQuantity* posicionEncontrada = list_find_with_args(lista, coincidePosicion, (void*)posicionNewPokemon);

        if(posicionEncontrada != NULL){
            posicionEncontrada->cantidad = posicionEncontrada->cantidad + posicionNewPokemon->cantidad;
            free(posicionNewPokemon);
        }else{
            list_add(lista, posicionNewPokemon);
        }

        bajarBloquesADisco(lista, bloques, cantidadBloques);
 /*       if(posicionEncontrada == NULL){
            char* writeBinary = malloc(sizeof(char)); 
            strcpy(writeBinary,"");
            memcpy(writeBinary, posX, strlen(posX)); 
            memcpy(writeBinary + strlen(posX), "-", strlen("-")); 
            memcpy(writeBinary + strlen(posX) + strlen("-"), posY, strlen(posY));
            memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY), "=", strlen("="));
            memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY) + strlen("="), quantity, strlen(quantity));
            //memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY) + strlen("=") + strlen(quantity), "\n", strlen("\n"));
            strcat(writeBinary, "\n");

            fseek(file, 0, SEEK_END);
            fwrite(writeBinary, strlen(writeBinary), 1, file);
        }*/

        list_destroy_and_destroy_elements(lista,free);
        free(posicionEncontrada);
        free(posY);
        free(posX);
        free(quantity);
    }

    fclose(file);
    config_destroy(configMetadataTallGrass);
    free(bloque);
    free(bloques);
}

void actualizarBitmap(char* bloque){
    //bitarray_set_bit(bitmap, bin-1);
}

t_list* levantarBloquesAMemoria(char** bloques, int cantidadBloques){
    char* directorio = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";
    t_list* listaPosiciones = list_create();
    int caracterActual = 0; 
    char buffer[10];
    memset(buffer,'\0',10);
    positionQuantity* lineaBloque = malloc(sizeof(positionQuantity));
    bool posx = true;
    bool posy = false;
    bool cant = false;
        
    for(int i = 0; i<cantidadBloques; i++){
        char* direccionBinario = malloc(strlen(directorio) + strlen(bloques[i]) + strlen(extension) + 1);

        strcpy(direccionBinario,"");
        strcat(direccionBinario,directorio);
        strcat(direccionBinario,bloques[i]);
        strcat(direccionBinario,extension);

        FILE* fileBloque = fopen(direccionBinario, "rb");
        //fseek(fileBloque, 0, SEEK_SET);

        int c; 
        
        while((c=fgetc(fileBloque)) != EOF){
            char position = (char) c; 
            if(posx){
                if(isdigit(position)){
                    buffer[caracterActual] = position;
                    caracterActual++; //revisar
                }else{
                    lineaBloque->posicionX = atoi(buffer);
                    posx = false;
                    posy = true;
                    memset(buffer,'\0',10);
                    caracterActual = 0;
                    continue;
                }     
            }else if(posy){
                if(isdigit(position)){
                    buffer[caracterActual] = position;
                    caracterActual++; //revisar
                }else{
                    lineaBloque->posicionY = atoi(buffer);
                    posy = false;
                    cant = true;
                    memset(buffer,'\0',10);
                    caracterActual = 0;
                    continue;
                } 
            }else if(cant){
                if(isdigit(position)){
                    buffer[caracterActual] = position;
                    caracterActual++; //revisar
                }else{
                    lineaBloque->cantidad = atoi(buffer);
                    posx = true;
                    cant = false;
                    memset(buffer,'\0',10);
                    caracterActual = 0;
                    list_add(listaPosiciones, lineaBloque);
                    lineaBloque = malloc(sizeof(positionQuantity));
                    continue;
                } 
            }
        }
        fclose(fileBloque);
        free(direccionBinario);
    }
    
    free(lineaBloque);
    return listaPosiciones;
}

bool coincidePosicion(void* elem, void* args){
    positionQuantity* posLista = (positionQuantity*) elem; 
    positionQuantity* posNewPokemon = (positionQuantity*) args;

    if(posLista->posicionX == posNewPokemon->posicionX &&
        posLista->posicionY == posNewPokemon->posicionY){
            return true;
    }
    return false; 
}

void bajarBloquesADisco(t_list* lista, char** bloques, int cantidadBloques){
    t_list* writeListBinary = list_map(lista, structALinea);
    char* listaConcatenada = concatenarStrings(writeListBinary);
    int j=0;
    char* directorio = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";
    for(int i = 0; i<cantidadBloques; i++){
        char* direccionBinario = malloc(strlen(directorio) + strlen(bloques[i]) + strlen(extension) + 1);

        strcpy(direccionBinario,"");
        strcat(direccionBinario,directorio);
        strcat(direccionBinario,bloques[i]);
        strcat(direccionBinario,extension);

        FILE* fileBloque = fopen(direccionBinario, "wb");

        int sizeArchivo = 0; 

        while(sizeArchivo < configM.blockSize){
            if(strlen(listaConcatenada) < j){
                break;
            }
            fputc(listaConcatenada[j],fileBloque);
            j++;
            sizeArchivo++;
        }

        fclose(fileBloque);
        free(direccionBinario);
    }

    //list_destroy_and_destroy_elements(lista,free);
    list_destroy_and_destroy_elements(writeListBinary, free);
    //free(directorio);
    //free(extension);
    free(listaConcatenada);
}

void* structALinea(void* posicion){
    positionQuantity* lineaStruct = (positionQuantity*)posicion;

    char* posX = malloc(10);
    strcpy(posX,"");
    sprintf(posX,"%d",lineaStruct->posicionX);//(char); 

    char* posY = malloc(10);
    strcpy(posY,"");
    sprintf(posY,"%d",lineaStruct->posicionY);//(char); 

    char* quantity = malloc(10);
    strcpy(quantity,"");
    sprintf(quantity,"%d",lineaStruct->cantidad);//(char); 

    char* writeBinary = malloc(strlen(posX) + strlen("-") + strlen(posY) + strlen("=") + strlen(quantity) + 3); 
    strcpy(writeBinary,"");
    strcat(writeBinary,posX);
    strcat(writeBinary,"-");
    strcat(writeBinary,posY);
    strcat(writeBinary, "=");
    strcat(writeBinary,quantity);
    strcat(writeBinary, "\n");
    //memcpy(writeBinary, posX, strlen(posX)); 
    //memcpy(writeBinary + strlen(posX), "-", strlen("-")); 
    //memcpy(writeBinary + strlen(posX) + strlen("-"), posY, strlen(posY));
    //memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY), "=", strlen("="));
    //memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY) + strlen("="), quantity, strlen(quantity));
    //memcpy(writeBinary + strlen(posX) + strlen("-") + strlen(posY) + strlen("=") + strlen(quantity), "\n", strlen("\n"));
    //strcat(writeBinary, "\n");

    free(posX);
    free(posY);
    free(quantity);
    return (void*)writeBinary;
}

char* concatenarStrings(t_list* lista){
    uint32_t tamano = 0;
    for(int i = 0; i<list_size(lista); i++){
        tamano += strlen(list_get(lista, i));
    }
    tamano++;
    char* concatenacion = malloc(tamano);

    for(int i = 0; i<list_size(lista); i++){
        strcat(concatenacion, (char*)list_get(lista,i));
    }

    return concatenacion;
}