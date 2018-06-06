/*
 * Dump.c
 *
 *  Created on: 3 jun. 2018
 *      Author: PabloSMonti
 */
#include "Dump.h"


int respaldar_informacion(t_list* tabla,char* storage,char* puntoDeMontaje,t_log* logger){

	log_info(logger,"Comienza el proceso respaldo de Informacion en: %s",puntoDeMontaje);

	int tamanioTabla = list_size(tabla);
	if(tamanioTabla <= 0){
		log_error(logger,"La tabla se encontro vacia, o error al obtener dimension");
		return(-1);
	}

	struct Dato* unDato;


	for(int i = 0 ; i < tamanioTabla ; i++){

		unDato = list_get( tabla, i );
		//char* ruta = puntoDeMontaje;
		//const char* clave =(char*) unDato->clave;
		//ruta = strcat(ruta,clave);
		char* ruta = unDato->clave;
		int fd = open(ruta, O_RDWR | O_CREAT, (mode_t)0600);
		lseek(fd, unDato->cantidadDeBytes-1, SEEK_SET);
		write(fd, "", 1);
		char *map = mmap(0, unDato->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(map, unDato->posicionMemoria, unDato->cantidadDeBytes);
		msync(map, unDato->cantidadDeBytes, MS_SYNC);
		munmap(map, unDato->cantidadDeBytes);
		close(fd);


	}
	log_info(logger,"Finaliza el proceso respaldo de Informacion..");


return EXIT_SUCCESS;
}

int respaldar_informacion_thread(parametros_dump* parametros){

	while(1){

		sleep(parametros->intervaloDeDump);

		log_info(parametros->logger,"Comienza el proceso respaldo de Informacion en: %s",parametros->puntoDeMontaje);

		int tamanioTabla = list_size(parametros->tabla);
		if(tamanioTabla <= 0){
			log_error(parametros->logger,"La tabla se encontro vacia, o error al obtener dimension");
			return(-1);
		}

		struct Dato* unDato;


		for(int i = 0 ; i < tamanioTabla ; i++){

			unDato = list_get( parametros->tabla, i );
			//char* ruta = puntoDeMontaje;
			//const char* clave =(char*) unDato->clave;
			//ruta = strcat(ruta,clave);
			char* ruta = unDato->clave;
			int fd = open(ruta, O_RDWR | O_CREAT, (mode_t)0600);
			lseek(fd, unDato->cantidadDeBytes-1, SEEK_SET);
			write(fd, "", 1);
			char *map = mmap(0, unDato->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			memcpy(map, unDato->posicionMemoria, unDato->cantidadDeBytes);
			msync(map, unDato->cantidadDeBytes, MS_SYNC);
			munmap(map, unDato->cantidadDeBytes);
			close(fd);


		}
		log_info(parametros->logger,"Finaliza el proceso respaldo de Informacion..");

	}

return EXIT_SUCCESS;
}

