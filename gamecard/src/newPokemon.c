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
    char* stream = malloc(sizeof(new_pokemon)); //Ver que va adentro del sizeof
    memcpy(stream, newPokemon->pokemon, newPokemon->sizePokemon); 
    stream[newPokemon->sizePokemon] = '\0';

    strcpy(buffer, "");
    strcat(buffer, directorio);
    strcat(buffer, stream);

    char* directory = malloc(strlen(directorio) + newPokemon->sizePokemon); 
    memcpy(directory, buffer, strlen(directorio) + newPokemon->sizePokemon);
    directory[strlen(directorio) + newPokemon->sizePokemon] = '\0';

    int created = mkdir(directory, ACCESSPERMS);
    if(created == -1){
        printf("%d",errno);
        printf("No se pudo crear el directorio.");
    }else{
        printf("Directorio creado o abierto.");
    }
    
    createMetadataPokemon(directory, newPokemon);
}

void createMetadataPokemon(char* directory, new_pokemon* newPokemon){
    char* metadata = "/Metadata.bin";
    char* directorioMetadata = malloc(sizeof(new_pokemon)); //Ver que va adentro del sizeof
    memcpy(directorioMetadata, directory, strlen(directory));
    memcpy(directorioMetadata + strlen(directory), metadata, strlen(metadata));

    FILE* file = fopen(directorioMetadata,"wb");

    fseek(file, 0, SEEK_END);
    int sizeFile = ftell(file);

    if(sizeFile == 0){
        configMetadataCreate(directorioMetadata);
        
        //char* metadataInfo = "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=Y";
        //fwrite(metadataInfo, strlen(metadataInfo), 1, file);      
       /* 
        fseek(file, 27, SEEK_SET);
        fwrite(bin, strlen(bin), 1,file); //NO esta funcionando*/
    }

    if(metadataBlocks() == 0){
        char* bin = crearBloque(newPokemon);
        addBlockMetadata(directorioMetadata,bin);
    }else{
        agregarDatosYOrdenarBloques(directorioMetadata, newPokemon);
    }

    fclose(file);
    //free(directorioMetadata);
}

char* crearBloque(new_pokemon* newPokemon){
    char* directorio = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";

    //FILE* bitmap = fopen("./TALL_GRASS/Metadata/Bitmap.bin", "wb+");

    char ch;
    size_t sizeBitmap = bitarray_get_max_bit(bitmap);
    int bin = 1;  
    while(bin <= (int)sizeBitmap + 1){
        //if(ch = "0"){
        bool testBit = bitarray_test_bit(bitmap, bin - 1);
        if(testBit == 0){
            char* binChar = malloc(sizeof(char));
            //char binTochar = (char) bin;
            strcpy(binChar,"");
            sprintf(binChar, "%d", bin);
            char* directorioBloques = malloc(strlen(directorio) + strlen(binChar) + strlen(extension)); //ver de cambiarlo 
            strcpy(directorioBloques,"");
            strcat(directorioBloques,directorio);
            strcat(directorioBloques,binChar);
            strcat(directorioBloques,extension);
            
            //memcpy(directorioBloques, directorio, strlen(directorio));
            //memcpy(directorioBloques + strlen(directorio), binChar, sizeof(char));
            //memcpy(directorioBloques +  strlen(directorio) + sizeof(char), extension, strlen(extension));
            
            FILE* binary = fopen(directorioBloques,"wb"); 

            char* posX = malloc(sizeof(char));
            strcpy(posX,"");
            sprintf(posX,"%d",newPokemon->position.posx);//(char); 

            char* posY = malloc(sizeof(char));
            strcpy(posY,"");
            sprintf(posY,"%d",newPokemon->position.posy);//(char); 

            char* quantity = malloc(sizeof(char));
            strcpy(quantity,"");
            sprintf(quantity,"%d",newPokemon->quantity);//(char); 

            char* writeBinary = malloc(sizeof(char)); 
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
            //imprimirBITARRAY(bitmap);
            return binChar;
            break; 
        }else{
            bin++;
        }
    }

    //fclose(bitmap);
}


void configMetadataCreate(char* metadata){
    t_config* configMetadataTallGrass = config_create("./cfg/tall_grass_metadata.config");

    config_save_in_file(configMetadataTallGrass, metadata);
}

void addBlockMetadata(char* metadata, char* block){
    t_config* configMetadataTallGrass = config_create(metadata);

    abrirMetadata(metadata);

    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil(size / configM.blockSize);
    int cantidadBloquesMasUno = cantidadBloques + 1;
    
    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");

    char* bloquesConfig = malloc((cantidadBloquesMasUno*2 + 2)*sizeof(char));
    strcpy(bloquesConfig,"");
    //bloques = realloc(bloques, sizeof(char*)*(size+1));
    //bloques[size] = block;
    strcat(bloquesConfig,"[");
    for(int i = 0; i < (cantidadBloques*2); i++){
        bloquesConfig[i]=*bloques[i];
        bloquesConfig[i++]=",";
        i++;
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
    cerrarMetadata(metadata);
}

void abrirMetadata(char* metadata){
    t_config* configMetadataTallGrass = config_create(metadata);
    
    config_set_value(configMetadataTallGrass, "OPEN", "Y");

    config_save(configMetadataTallGrass);
}

void cerrarMetadata(char* metadata){
    t_config* configMetadataTallGrass = config_create(metadata);
    
    config_set_value(configMetadataTallGrass, "OPEN", "N");

    config_save(configMetadataTallGrass);
}

int metadataBlocks(char* metadata){
    t_config* configMetadataTallGrass = config_create(metadata);

    //char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");

    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil(size / configM.blockSize);

    return cantidadBloques;
}

void agregarDatosYOrdenarBloques(char* directorioMetadata, new_pokemon newPokemon){

}

void actualizarBitmap(){
    
}