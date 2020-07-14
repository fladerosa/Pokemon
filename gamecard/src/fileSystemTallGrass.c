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

    t_config* configMetadata = config_create("./TALL_GRASS/Metadata/Metadata.bin");

    configM.blockSize = config_get_int_value(configMetadata, "BLOCK_SIZE");
    configM.blocks = config_get_int_value(configMetadata, "BLOCKS");

    config_destroy(configMetadata);
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
