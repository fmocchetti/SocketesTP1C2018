/*
 * STORE.c
 *
 *  Created on: 7 jun. 2018
 *      Author: Pablo Monti
 */
#include "STORE.h"

int STORE(t_list** tabla,t_list** registro,char* primeraPosicionMemoria,char* clave,char* ruta,int tamanioEntrada){


	struct Dato* claveBuscada = buscar(*tabla,clave);

	if(claveBuscada == NULL){

		log_info(logger,"STORE:No se encontro la Clave: %s", clave);

		return (1);

	}




	char* rutaArmada = malloc(strlen(ruta)+strlen(clave)+strlen(getenv("HOME"))+1);
	//strcpy(rutaArmada,getenv("HOME"));
	if(strlen(ruta)>1)
		strcpy(rutaArmada,ruta);
	//strcat(rutaArmada,"/");
	strcat(rutaArmada,clave);

/*
	//memcpy(rutaArmada,ruta,sizeof(*ruta));
	memcpy(rutaArmada,"/",sizeof("/"));
	memcpy(rutaArmada+sizeof("/"),clave,sizeof(*clave));
*/
	log_info(logger,"STORE:Se guardara el archivo en ruta: %s",rutaArmada);


	int fd = open(rutaArmada, O_RDWR | O_CREAT, (mode_t)0600);

	if(fd < 0){

		log_error(logger,"STORE:Error al intentar abrir archivo en la ruta");
		return -1;
	}

	log_info(logger,"STORE:Se crea el archivo: %d",fd);
	lseek(fd, claveBuscada->cantidadDeBytes-1, SEEK_SET);
	write(fd, "", 1);
	char *map = mmap(0, claveBuscada->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(map, claveBuscada->posicionMemoria, claveBuscada->cantidadDeBytes);
	msync(map, claveBuscada->cantidadDeBytes, MS_SYNC);
	munmap(map, claveBuscada->cantidadDeBytes);
	close(fd);


	log_info(logger,"STORE:Se cierra el archivo");


	int cantidEntradasAOcupar = calcular_cantidad_entradas(claveBuscada->cantidadDeBytes,tamanioEntrada);

	registrar_acceso_a_entrada(registro,primeraPosicionMemoria,claveBuscada->posicionMemoria,tamanioEntrada,cantidEntradasAOcupar);

	log_info(logger,"STORE:Se registro acceso a entrada");

/*
	if(borrar_un_dato(tabla,claveBuscada) == 0){

		log_info(logger,"STORE:Se libero la clave %s",claveBuscada->clave);
	}
	else{
		log_error(logger,"STORE:No se pudo liberar la clave");
	}
*/
	free(rutaArmada);



return 0;
}


