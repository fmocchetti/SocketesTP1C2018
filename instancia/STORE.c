/*
 * STORE.c
 *
 *  Created on: 7 jun. 2018
 *      Author: Pablo Monti
 */
#include "STORE.h"
/*

int STORE(t_list* tabla,char* clave,char* ruta,t_log* logger){



	//pthread_mutex_unlock(&lock_dump);

	struct Dato* claveBuscada = buscar(tabla,clave);

	if(claveBuscada == NULL){

		log_info(logger,"No se encontro la Clave en STORE: %s", clave);
		return (1);

	}

	//char* ruta = puntoDeMontaje;
	//const char* clave =(char*) unDato->clave;
	//ruta = strcat(ruta,clave);


	char* rutaArmada = malloc(sizeof(ruta)+sizeof(clave)+1);

	//memcpy(rutaArmada,ruta,sizeof(ruta));
	//memcpy(rutaArmada+sizeof(ruta),'/',1);
	//memcpy(rutaArmada+sizeof(clave)+1,clave,sizeof(clave));
	strcpy(rutaArmada,ruta);
	strcpy(rutaArmada,"/");
	strcpy(rutaArmada,clave);

	log_info(logger,"Se guardara el archivo en ruta: %s",rutaArmada);


	int fd = open(rutaArmada, O_RDWR | O_CREAT, (mode_t)0600);
	log_info(logger,"Se crea el archivo: %d",fd);
	lseek(fd, claveBuscada->cantidadDeBytes-1, SEEK_SET);
	write(fd, "", 1);
	char *map = mmap(0, claveBuscada->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(map, claveBuscada->posicionMemoria, claveBuscada->cantidadDeBytes);
	msync(map, claveBuscada->cantidadDeBytes, MS_SYNC);
	munmap(map, claveBuscada->cantidadDeBytes);
	close(fd);
	free(rutaArmada);
	log_info(logger,"Se cierra el archivo");

	//pthread_mutex_lock(&lock_dump);

return 0;
}*/
