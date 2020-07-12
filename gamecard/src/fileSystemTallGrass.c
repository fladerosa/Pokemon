#include "fileSystemTallGrass.h"

/*void crearBloques(){
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
}*/

void obtenerConfig(){
    FILE* file; 
    char buffer[25];
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

void crearArchivoBitmap(){

	int blockNum = configM.blocks;
	while(blockNum%8!=0){
		blockNum++;
	}
	int bytes = BIT_CHAR(blockNum);
	char* bitsVacios = string_repeat(0,bytes);

	FILE *fp;
	t_bitarray* arrayCreador = bitarray_create_with_mode(bitsVacios,bytes,0);

	printf("\n");
	fp=fopen("./TALL_GRASS/Metadata/Bitmap.bin","w");
	fwrite(arrayCreador->bitarray,1,bytes,fp);
	fclose(fp);

	bitarray_destroy(arrayCreador); 

	free(bitsVacios);
}

void imprimirBITARRAY(t_bitarray* bitarray){

	int blockNum = configM.blocks;

	for (int i = 0; i<blockNum;i++){
		printf("%d",bitarray_test_bit(bitarray,i));
	}
	printf("\n");

}


void crearBitMap(){

	int blockNum = configM.blocks;
	while(blockNum%8!=0){
			blockNum++; 
	}
	int bytes = BIT_CHAR(blockNum);
	char* archivoBitmap;
	int fd = open("./TALL_GRASS/Metadata/Bitmap.bin", O_RDWR , (mode_t)0600);
	struct stat mystat;
	if(fstat(fd,&mystat)<0){
		printf("Error al establecer fstat\n");
		close (fd);
	}

	archivoBitmap=(char*) mmap(0,mystat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED ,fd,0);
	if(archivoBitmap==MAP_FAILED){
		printf("ERROR AL MAPEAR A MEMORIA");
		close(fd);

	}

	bitmap=bitarray_create_with_mode(archivoBitmap , mystat.st_size ,0);

	msync(archivoBitmap,bytes,MS_SYNC);

    imprimirBITARRAY(bitmap);

	close(fd);
}


void ActualizarBitmap(){

	int blockNum = configM.blocks;
	while(blockNum%8!=0){
			blockNum++; 
	}
	int bytes = BIT_CHAR(blockNum);
	FILE *fp;
	fp=fopen("./TALL_GRASS/Metadata/Bitmap.bin","w");
	fwrite(bitmap->bitarray,1,bytes,fp);


	fclose(fp);
}

int hayBitmap(){

    return access( "./TALL_GRASS/Metadata/Bitmap.bin", F_OK ) != -1;

}

void iniciarTallGrass(){
    obtenerConfig();
    //crearBloques();
    if(hayBitmap()){
        printf("\nCreando Bitmap...\n");
        crearArchivoBitmap();
        printf("Bitmap Creado con exito.\n");
    }

    crearBitMap();
}
