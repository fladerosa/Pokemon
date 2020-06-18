#include "fileSystemTallGrass.h"

void iniciarTallGrass(){
    obtenerConfig();
    crearBloques();

}

void crearBloques(){
    char* nombreArchivo = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";
    char buffer[29];
    char* bloque = malloc(sizeof(char));
    

    for(uint32_t i = 1; i <= configM.blocks; i++){
        FILE* block;
        strcpy(buffer,"");
        snprintf(bloque,10,"%d",i);

        strcat(buffer,nombreArchivo);
        strcat(buffer,bloque);
        strcat(buffer,extension);
        block = fopen(buffer,"wb");
        fclose(block);
        
    }

    free(bloque);
}

void obtenerConfig(){
    FILE* file; 
    char buffer[25]; //revisar
    char BLOCKS[4];
    char BLOCKSIZE[2];
    uint32_t j=0;

    file = fopen("./TALL_GRASS/Metadata/Metadata.bin","rb");

    fread(buffer,sizeof(buffer),1,file);

    for(int i = 11; i<13; i++){
        if(buffer[i] == "\n"){
            break;
        }
        
        BLOCKSIZE[j] = buffer[i];
        j++;
    }
    configM.blockSize = atoi(BLOCKSIZE);
    j=0;
    for(int i = 21; i<25; i++){
        if(buffer[i] == "\n"){
            break;
        }

        BLOCKS[j] = buffer[i];
        j++;
    }
    configM.blocks = atoi(BLOCKS);
    fclose(file);
}