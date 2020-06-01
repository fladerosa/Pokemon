#include "fileSystemTallGrass.h"


void iniciarTallGrass(){
    FILE* file; 
    unsigned char buffer[25]; //revisar
    unsigned char BLOCKS[10];

    file = fopen("./TALL_GRASS/Metadata/Metadata.bin","rb");

    fread(buffer,sizeof(buffer),1,file);// 11 a 13 block size

    /*for(int i=21; i<25; i++){
        concatenate(BLOCKS,buffer[i]);
    }

    printf("%s", BLOCKS);*/
    
}