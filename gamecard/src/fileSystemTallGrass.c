#include "fileSystemTallGrass.h"

void iniciarTallGrass(){
    obtenerConfig();
    crearBloques();

}

void crearBloques(){
    char* buffer = malloc(3*sizeof(char));
    char* bloque = malloc(sizeof(uint32_t));
    char* nombreArchivo = "./TALL_GRASS/Blocks/";
    char* extension = ".bin";

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
    //free(buffer);
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
        BLOCKSIZE[j] = buffer[i];
        j++;
    }
    configM.blockSize = atoi(BLOCKSIZE);
    j=0;
    for(int i = 21; i<25; i++){
        BLOCKS[j] = buffer[i];
        j++;
    }
    configM.blocks = atoi(BLOCKS);
    fclose(file);
}