/*
 * algoritmos.c
 *
 *  Created on: 12 may. 2018
 *      Author: pdelucchi
 */

#include "algoritmos.h"

int result_connection = 0;
bool result_satisfy =false;
char claveGlobal[100];

void laWeaReplanificadoraFIFO(t_list * listaDestino, t_list *listaEntrada){
    ESI* primerElemento = NULL;
    primerElemento = list_remove(listaEntrada,0);
    list_add(listaDestino,primerElemento);
}

//muestra el estado de los procesos encolados en las listas existentes
void estadoListas(){
    int cantidadListos = 0;
    int cantidadEjecucion = 0;
    int cantidadBloqueados = 0;
    int cantidadTerminados = 0;
    int cantidadMuertos = 0;
    cantidadEjecucion = list_size(ejecucion);
    cantidadListos = list_size(listos);
    cantidadBloqueados = list_size(bloqueados);
    cantidadTerminados = list_size(terminados);
    cantidadMuertos = list_size(muertos);
    printf("Procesos en la cola de listos %d\n",cantidadListos);
    printf("Procesos en la cola de ejecucion %d\n",cantidadEjecucion);
    printf("Procesos en la cola de bloqueados %d\n",cantidadBloqueados);
    printf("Procesos en la cola de terminados %d\n",cantidadTerminados);
    printf("Procesos en la cola de muertos %d\n",cantidadMuertos);

}

void sjfsd(){
	int sem_value = 0;
	int sem_value2 = 0;
	unsigned char permisoDeEjecucion = 1;
	unsigned char contestacionESI = 0;
	int resultado_lista = 0;
	int result_send = 0;
	int resultado_lista_satisfy = 0;
	int l = 0;
	int resultado_satisfy = 0;

	while(1){
		ESI *nodo_lista_ejecucion = NULL;//(ESI*) malloc(sizeof(ESI));
		log_info(logger,"Esperando para planificar");


		//espero a que me digan que hay un nuevo proceso en listos
		sem_wait(&new_process);

		log_info(logger,"Nuevo elemento en la cola de listos o desbloqueo manual de una clave que genero una replanificacion");
		estadoListas();

		sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
			sem_wait(&sem_pausar_algoritmo);
				}

		//replanifico aca, dependiendo de la rafaga
		if(list_size(listos) >= 1){
			log_info(logger,"Replanificando");
			list_sort(listos, (void*)sort_by_estimacion);

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		log_info(logger,"Nodo de listos movido a Ejecucion");

		resultado_lista = list_is_empty(ejecucion);

		if(!resultado_lista){
			nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
			log_info(logger,"ID de la ESI a ejecutar %d, con una estimacion de rafaga de %f", nodo_lista_ejecucion->id_ESI,nodo_lista_ejecucion->rafaga);
			id_esi_global = nodo_lista_ejecucion->id_ESI;
		}

		//Mientras la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		while(nodo_lista_ejecucion->cantidadDeLineas >0){
			sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
				sem_wait(&sem_pausar_algoritmo);
			}

			//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
			result_send = send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
			//aca tengo que verificar
			if (result_send <= 0) {
				nodo_lista_ejecucion->cantidadDeLineas = 0;
				laWeaReplanificadoraFIFO(muertos,ejecucion);
				_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
				break;
			}

			//Espero que la esi me conteste
			result_connection = recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);

			if (result_connection <= 0) {
				id_esi_global = nodo_lista_ejecucion->id_ESI;
				resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				while(resultado_lista_satisfy == 1){
					//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
					claves* clave_temporal = NULL;
					clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
					ESI_STORE(clave_temporal->claveAEjecutar,0);
					free(clave_temporal);
					//list_remove_and_destroy_by_condition(claves_tomadas,(void*)identificador_clave_por_idESI,(void*)clave_destroy);
					resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				}

				//hago close del socket

				laWeaReplanificadoraFIFO(muertos,ejecucion);
				_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
				break;
				}

			//magia con el coord (Recibe si es store/get y realiza acorde)
			coord_communication(nodo_lista_ejecucion->socket_esi,nodo_lista_ejecucion->id_ESI ,contestacionESI,nodo_lista_ejecucion->cantidadDeLineas);


			if(contestacionESI == 2) {
				//Si es 2, entonces resto 1 a cada linea faltante y sumo 1 por cada ejecucion de sentencia a las sentencias ejecutadas
				nodo_lista_ejecucion->cantidadDeLineas --;
				nodo_lista_ejecucion->lineas_ejecutadas ++;
			} else {
				//agrego a bloqueados en caso de recibir otra contestacion de la ESI
				log_info(logger,"La ESI '%d' que se encontraba en EJECUCION se pasara a BLOQUEADOS",nodo_lista_ejecucion->id_ESI);
				//Sumo uno a las lineas a ejecutar ya que intento ejecutar una sentencia aunque no pudo y cuenta segun issue foro: #1131
				nodo_lista_ejecucion->lineas_ejecutadas ++;
				log_info(logger, "lineas ejecutadas so far: %d", nodo_lista_ejecucion->lineas_ejecutadas);
				//estimo la rafaga que va a tener ahora que ya ejecuto algunas sentencias
				nodo_lista_ejecucion->rafaga = calculoProxRafaga((float)alpha,nodo_lista_ejecucion->estimacion_rafaga,(float)nodo_lista_ejecucion->lineas_ejecutadas);
				nodo_lista_ejecucion->lineas_ejecutadas = 0;
				nodo_lista_ejecucion->estimacion_rafaga = nodo_lista_ejecucion->rafaga;
				log_info(logger, "Calculo de rafaga: %f", nodo_lista_ejecucion->rafaga);
				laWeaReplanificadoraFIFO(bloqueados,ejecucion);
				break;
			}

		}
		if(nodo_lista_ejecucion->cantidadDeLineas <=0){
		//Si la cantidad de lineas es menor a 0, muevo la ESI a la cola de terminados
		log_info(logger, "Ejecucion de la ESI '%d' terminada", nodo_lista_ejecucion->id_ESI);

		//desalojo las claves tomadas en caso de que existan
		id_esi_global = nodo_lista_ejecucion->id_ESI;
		resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
		while(resultado_satisfy == 1){
		//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
			claves* clave_temporal = NULL;
			clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
			ESI_STORE(clave_temporal->claveAEjecutar,0);
			free(clave_temporal);
			resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
			}
		laWeaReplanificadoraFIFO(terminados,ejecucion);
		estadoListas();
		}
	}
		else{
			log_info(logger, "No era necesario replanificar ya que la cola de listos estaba vacia");
			sem_getvalue(&new_process,&sem_value2);
			for(l = 0;l<sem_value2;l++){
				sem_wait(&new_process);
			}
		}
	}
}

void sjfcd(){
	log_info(logger,"Estas en SJFCD");

	unsigned char permisoDeEjecucion = 1;
	unsigned char contestacionESI = 0;
	int sem_value = 0;
	int sem_value2 = 0;
	int result_connection = 0;
	int resultado_lista_satisfy = 0;
	int result_send = 0;
	int replanificar_en_ejecucion = 0;
	int l = 0;
	int resultado_satisfy = 0;

	while(1) {
		ESI *nodo_lista_ejecucion = NULL;

		log_info(logger,"Esperando para planificar");

		//espero a que me digan que hay un nuevo proceso en listos
		sem_wait(&new_process);

		log_info(logger,"Nuevo elemento en la cola de listos o desbloqueo manual de una clave que genero una replanificacion");
		estadoListas();

		//En caso de ejecutar el comando pausar se usa el siguiente semaforo y condicion
		sem_getvalue(&sem_pausar_planificacion,&sem_value);
		if(sem_value<1){
			sem_wait(&sem_pausar_algoritmo);
			//break;
		}
		//replanifico dependiendo de la rafaga
		if(replanificar == 1){
			log_info(logger,"Replanificando");
			list_sort(listos, (void*)sort_by_estimacion);
			replanificar = 0;
		}
		if(list_size(listos)>0){

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		log_info(logger,"Nodo de listos movido a Ejecucion");
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		log_info(logger,"ID de la ESI a ejecutar %d", nodo_lista_ejecucion->id_ESI);
		log_info(logger, "Calculo de rafaga: %f", nodo_lista_ejecucion->rafaga);
		id_esi_global = nodo_lista_ejecucion->id_ESI;

		//Ejecuto la esi seleccionada hasta recibir algun evento que necesite replanificar(nueva esi en listos, de bloqueado a listos, etc).
		while(replanificar == 0){
			//En caso de ejecutar el comando pausar se usa el siguiente semaforo y condicion
			sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
				sem_wait(&sem_pausar_algoritmo);
			}
			//Si la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
			if(nodo_lista_ejecucion->cantidadDeLineas >0) {

				//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
				result_send = send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
				if (result_send <= 0) {
					nodo_lista_ejecucion->cantidadDeLineas = 0;
					laWeaReplanificadoraFIFO(muertos,ejecucion);
					_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
					break;
				}

				//Espero que la esi me conteste
				result_connection = recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);

				if (result_connection <= 0) {
					id_esi_global = nodo_lista_ejecucion->id_ESI;
					resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
					while(resultado_lista_satisfy == 1){
						//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
						claves* clave_temporal = (claves*) malloc(sizeof(claves));
						clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
						ESI_STORE(clave_temporal->claveAEjecutar,0);
						free(clave_temporal);
						resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
					}

					//hago close del socket

					laWeaReplanificadoraFIFO(muertos,ejecucion);
					_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
					break;
					}

				//magia con el coord (Recibe si es store/get y realiza acorde)
				coord_communication(nodo_lista_ejecucion->socket_esi,nodo_lista_ejecucion->id_ESI ,contestacionESI,nodo_lista_ejecucion->cantidadDeLineas);

				if(contestacionESI == 2) {
					//Si es 2, entonces resto 1 a cada linea faltante y sumo 1 por cada ejecucion de sentencia a las sentencias ejecutadas
					nodo_lista_ejecucion->cantidadDeLineas --;
					nodo_lista_ejecucion->lineas_ejecutadas ++;
					//resto a la rafaga en caso de ser replanificado en ejecucion
					nodo_lista_ejecucion->rafaga --;

				} else {
					//agrego a bloqueados en caso de recibir otra contestacion de la ESI
					log_info(logger,"La ESI %d que se encontraba en EJECUCION se pasara a BLOQUEADOS",nodo_lista_ejecucion->id_ESI);
					//Sumo uno a las lineas a ejecutar ya que intento ejecutar una sentencia aunque no pudo y cuenta segun issue foro: #1131
					nodo_lista_ejecucion->lineas_ejecutadas ++;
					log_info(logger, "lineas ejecutadas so far: %d", nodo_lista_ejecucion->lineas_ejecutadas);
					//estimo la rafaga que va a tener ahora que ya ejecuto algunas sentencias
					nodo_lista_ejecucion->rafaga = calculoProxRafaga((float)alpha,nodo_lista_ejecucion->estimacion_rafaga,(float)nodo_lista_ejecucion->lineas_ejecutadas);
					nodo_lista_ejecucion->lineas_ejecutadas = 0;
					nodo_lista_ejecucion->estimacion_rafaga = nodo_lista_ejecucion->rafaga;
					//log_info(logger, "Calculo de rafaga: %f", nodo_lista_ejecucion->rafaga);

					laWeaReplanificadoraFIFO(bloqueados,ejecucion);

					if(list_size(listos)>=1){
					replanificar = 1;
					sem_post(&new_process);

					}
					break;
				}

			} else {
				//Si la cantidad de lineas es menor a 0, muevo la ESI a la cola de terminados
				//printf("Entre a 1\n");
				log_info(logger, "Ejecucion de la ESI '%d' terminada", nodo_lista_ejecucion->id_ESI);
				//desalojo las claves tomadas en caso de que existan
				id_esi_global = nodo_lista_ejecucion->id_ESI;
				resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				while(resultado_satisfy == 1){
				//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
					claves* clave_temporal = NULL;
					clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
					ESI_STORE(clave_temporal->claveAEjecutar,0);
					free(clave_temporal);
					resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				}
				laWeaReplanificadoraFIFO(terminados,ejecucion);
				estadoListas();
				replanificar = 1;
			}
		}
		//evaluo si aun existia una esi en ejecucion ante la peticion de replanificar
		if(nodo_lista_ejecucion->cantidadDeLineas >0 && !list_is_empty(ejecucion)){
			replanificar_en_ejecucion = 1;
		}

		if(replanificar_en_ejecucion == 1){
		//De ser asi, se replanifica la esi en ejecucion, moviendola de dicha lista a LISTOS
		//Y se mueve la esi que estaba ejecutando a listos para su replanificacion
			log_info(logger,"La ESI %d que se encontraba en EJECUCION se pasara a LISTOS",nodo_lista_ejecucion->id_ESI);
			laWeaReplanificadoraFIFO(listos,ejecucion);
			estadoListas();
			replanificar = 1;
			sem_post(&new_process);
			replanificar_en_ejecucion = 0;
			//free(nodo_lista_ejecucion);

		}
		}
	else{
		sem_getvalue(&new_process,&sem_value2);
		for(l = 0;l<sem_value2;l++){
			sem_wait(&new_process);
		}

	}
}
}

void hrrn(){
	log_info(logger,"Estas en HRRN");

	unsigned char permisoDeEjecucion = 1;
	unsigned char contestacionESI = 0;
	int sem_value = 0;
	int sem_value2 = 0;
	int result_connection = 0;
	int resultado_lista_satisfy = 0;
	int result_send = 0;
	int l = 0;
	int resultado_satisfy = 0;

	while(1) {
		ESI *nodo_lista_ejecucion = NULL;
		log_info(logger,"Esperando para planificar");

		//espero a que me digan que hay un nuevo proceso en listos
		sem_wait(&new_process);
		log_info(logger,"Nuevo elemento en la cola de listos o desbloqueo manual de una clave que genero una replanificacion");
		estadoListas();

		//En caso de ejecutar el comando pausar
		sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
				sem_wait(&sem_pausar_algoritmo);
			}
		//replanifico dependiendo del RR (El que mayor RR posea)
		if(list_size(listos)>0){
			log_info(logger,"Replanificando");
			obtenerPrioridad();
			list_sort(listos, (void*)sort_by_aging);

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		log_info(logger,"Nodo de listos movido a Ejecucion");


		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		nodo_lista_ejecucion->espera = 0;
		log_info(logger,"ID de la ESI a ejecutar %d", nodo_lista_ejecucion->id_ESI);
		log_info(logger, "Calculo de prioridad: %f", nodo_lista_ejecucion->prioridad);
		//log_info(logger, "Calculo de rafaga: %f", nodo_lista_ejecucion->rafaga);
		id_esi_global = nodo_lista_ejecucion->id_ESI;

		//Ejecuto la esi seleccionada hasta recibir algun evento que necesite replanificar(nueva esi en listos, de bloqueado a listos, etc).
		while(nodo_lista_ejecucion->cantidadDeLineas >0){

			//En caso de ejecutar el comando pausar se usa el siguiente semaforo y condicion
			sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
				sem_wait(&sem_pausar_algoritmo);
				//break;
			}

			//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
			result_send = send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
			//aca tengo que verificar
			if (result_send <= 0) {
				nodo_lista_ejecucion->cantidadDeLineas = 0;
				laWeaReplanificadoraFIFO(muertos,ejecucion);
				_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
				break;
			}

			//Espero que la esi me conteste
			result_connection = recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);

			if (result_connection <= 0) {
				id_esi_global = nodo_lista_ejecucion->id_ESI;
				resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				while(resultado_lista_satisfy == 1){
					//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
					claves* clave_temporal = (claves*) malloc(sizeof(claves));
					clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
					ESI_STORE(clave_temporal->claveAEjecutar,0);
					free(clave_temporal);
					//list_remove_and_destroy_by_condition(claves_tomadas,(void*)identificador_clave_por_idESI,(void*)clave_destroy);
					resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				}

				//hago close del socket

				laWeaReplanificadoraFIFO(muertos,ejecucion);
				_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
				break;
			}

			//magia con el coord (Recibe si es store/get y realiza acorde)
			coord_communication(nodo_lista_ejecucion->socket_esi,nodo_lista_ejecucion->id_ESI ,contestacionESI,nodo_lista_ejecucion->cantidadDeLineas);

			 if(contestacionESI == 2) {
				//Si es 2, entonces resto 1 a cada linea faltante y sumo 1 por cada ejecucion de sentencia a las sentencias ejecutadas
				nodo_lista_ejecucion->cantidadDeLineas --;
				nodo_lista_ejecucion->lineas_ejecutadas ++;
				//sumo 1 por cada instruccion a todas las esis en ready a su W
				envejecerLista(1);

			} else {
				//agrego a bloqueados en caso de recibir otra contestacion de la ESI
				log_info(logger,"La ESI %d que se encontraba en ejecucion se pasara a BLOQUEADOS",nodo_lista_ejecucion->id_ESI);
				//Sumo uno a las lineas a ejecutar ya que intento ejecutar una sentencia aunque no pudo y cuenta segun issue foro: #1131
				nodo_lista_ejecucion->lineas_ejecutadas ++;
				//Lo mismo para el wait time, contaria como uno extra
				envejecerLista(1);
				log_info(logger, "lineas ejecutadas so far: %d", nodo_lista_ejecucion->lineas_ejecutadas);
				//envejecemos TODOS los nodos encolados ANTES de replanificar
				nodo_lista_ejecucion->rafaga = calculoProxRafaga((float)alpha,nodo_lista_ejecucion->estimacion_rafaga,(float)nodo_lista_ejecucion->lineas_ejecutadas);
				nodo_lista_ejecucion->estimacion_rafaga = nodo_lista_ejecucion->rafaga;
				log_info(logger, "Calculo de rafaga: %f", nodo_lista_ejecucion->rafaga);
				//envejecerLista(nodo_lista_ejecucion->lineas_ejecutadas);
				nodo_lista_ejecucion->lineas_ejecutadas = 0;
				laWeaReplanificadoraFIFO(bloqueados,ejecucion);
				break;
			}

		}
			if(nodo_lista_ejecucion->cantidadDeLineas <=0) {
				//Si la cantidad de lineas es menor a 0, muevo la ESI a la cola de terminados
				log_info(logger, "Ejecucion de la ESI '%d' terminada", nodo_lista_ejecucion->id_ESI);
				//desalojo las claves tomadas en caso de que existan
				id_esi_global = nodo_lista_ejecucion->id_ESI;
				resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				while(resultado_satisfy == 1){
				//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
					claves* clave_temporal = NULL;
					clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
					ESI_STORE(clave_temporal->claveAEjecutar,0);
					free(clave_temporal);
					resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
				}
				laWeaReplanificadoraFIFO(terminados,ejecucion);
				estadoListas();
				replanificar = 1;
			}
		}
	else{
		sem_getvalue(&new_process,&sem_value2);
				for(l = 0;l<sem_value2;l++){
					sem_wait(&new_process);
				}
		}
	}
}

//HRRN functions//
bool sort_by_aging(void * data1, void * data2){
	ESI * esi1 = (ESI*) data1;
	ESI * esi2 = (ESI*) data2;
	if(esi1->prioridad >= esi2->prioridad) {
			return true;
		}
		return false;
}

void envejecerLista(int tiempoEsperado){
	void sumar_espera(ESI* esi1) {
	     esi1->espera += tiempoEsperado;
	     }
	listos = list_map(listos, (void*) sumar_espera);
}

void obtenerPrioridad(){
	list_map(listos,(void*) aplicarHRRN);
}

void aplicarHRRN(ESI* esi){
	float espera; //cantidadDeLineas, lineas_ejecutadas;
	espera = (float) esi->espera;
	//cantidadDeLineas = (float) esi->cantidadDeLineas;
	//lineas_ejecutadas = (float) esi->lineas_ejecutadas;
	if(esi->rafaga == 0){
		esi->prioridad = 1;
	}
	else{
	esi->prioridad = 1 + (espera / (esi->rafaga));
	log_info(logger,"La ESI %d tiene los siguientes valores",esi->id_ESI);
	log_info(logger,"El calculo de RR dio %f",esi->prioridad);
	log_info(logger,"El calculo de W dio %f",espera);
	log_info(logger,"El calculo de S dio %f",esi->rafaga);
	}
}
//HRRN functions//


float calculoProxRafaga(float alpha,float estimacion_rafaga, float rafaga_real){
	float resultado = ((alpha/100)*rafaga_real + (1-(alpha/100))*estimacion_rafaga);
	return resultado;
}


bool sort_by_estimacion(void * data1, void * data2){
	ESI * esi1 = (ESI*) data1;
	ESI * esi2 = (ESI*) data2;
	if(esi1->rafaga <= esi2->rafaga) {
			return true;
		}
		return false;
}

bool identificador_ESI(void * data){
	ESI *esi1= (ESI*) data;
	if(esi1->id_ESI == id_esi_global) {
		return true;
	}
	return false;
}

bool identificador_ESI_kill(void * data){
		ESI *esi1= (ESI*) data;
		if(esi1->id_ESI == id_esi_global_kill) {
			return true;
		}
		return false;
}

bool identificador_clave(void * data){
	claves *clave1= (claves*) data;
	if(strcmp(clave1->claveAEjecutar,clave_bloqueada_global)==0) {
		return true;
	}
	return false;
}

bool identificador_clave_por_idESI(void * data){
	claves *clave1= (claves*) data;
	if(clave1->id_ESI == id_esi_global) {
		return true;
	}
	return false;
}

void element_destroyer(void * data){
	free(data);
}

void nodo_lista_claves_destroyer(claves * data){
	free(data);
}


void ESI_GET(char * claveAEjecutar, int id_ESI, unsigned char respuesta_ESI){

	claves* clave1 = (claves*) malloc(sizeof(claves));
	memset(clave1, 0, sizeof(claves));
	strcpy(clave1->claveAEjecutar,claveAEjecutar);
	clave1->id_ESI = id_ESI;

	if(esi_bloqueada_de_entrada==1){
		strcpy(clave_bloqueada_global,claveAEjecutar);

		if(dictionary_has_key(claves_bloqueadas,clave_bloqueada_global)){
				t_list * list_clave = dictionary_get(claves_bloqueadas,clave_bloqueada_global);
				//Si la lista ya existe, se pushea el nuevo id_ESI en la lista de la clave bloqueada
				list_add(list_clave, (int*)id_ESI);
				log_info(logger, "Inserte la esi %d en la queue de claves bloqueadas, para la clave '%s'", id_ESI, clave_bloqueada_global);
				free(clave1);
		} else {
				//Si no existe la clave, creo la lista asociada, pusheo el id_ESI y agrego la clave con su cola asociada
				printf("Entre en 2\n");
				t_list * list_clave = list_create();
				list_add(list_clave, (int*)id_ESI);
				dictionary_put(claves_bloqueadas, claveAEjecutar, list_clave);
				log_info(logger, "Inserte la esi %d en la que de claves bloqueadas, para la clave '%s'", id_ESI, clave_bloqueada_global);
				free(clave1);
		}
	}
	else{
		if(respuesta_ESI==2){
			list_add(claves_tomadas, (claves*)clave1);
			log_info(logger, "La esi %d tomo la clave '%s'", id_ESI, claveAEjecutar);
		}
		else if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
			t_list * list_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
			//Si la lista ya existe, se pushea el nuevo id_ESI en la cola de la clave bloqueada
			list_add(list_clave, (int*)id_ESI); //REVISAR si ese (int*) funca bien
			log_info(logger, "La esi %d se sumara a esperar la liberacion de la clave '%s'", id_ESI, claveAEjecutar);
			free(clave1);
		} else {
			//Si no existe la clave, creo la lista asociada, pusheo el id_ESI y agrego la lista con su cola asociada
			t_list * list_clave = list_create();
			list_add(list_clave, (int*)id_ESI);
			dictionary_put(claves_bloqueadas, claveAEjecutar, list_clave);
			log_info(logger, "La esi %d se sumara a esperar la liberacion de la clave '%s'", id_ESI, claveAEjecutar);
			free(clave1);
		}
	}
	//free(clave1);//REVISAR!!!
}

void ESI_STORE(char *claveAEjecutar, int cantidadDeLineas){
	int list_vacia = 0;
    int id_esi_desbloqueado = 0;
    int key_existente = 0;
    int resultado_lista_satisfy = 0;
    strcpy(clave_bloqueada_global,claveAEjecutar);
    resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave);
    if(resultado_lista_satisfy==1){
    	//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
		list_remove_and_destroy_by_condition(claves_tomadas,(void*)identificador_clave,(void*)clave_destroy);
		//printf("Clave removida de la lista %s\n",clave_temporal->claveAEjecutar);
		//free(clave_temporal);
		log_info(logger,"Clave eliminada correctamente de la lista de tomados\n");
    }

    //reviso si la clave existe en el diccionario
    if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
		t_list * list_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
	    list_vacia = list_is_empty(list_clave);

	    //reviso si la lista no esta vacia
		if(!list_vacia){
			//quito el primer elemento de la lista, que sera la proxima esi a salir de bloqueados
			id_esi_desbloqueado = (int)list_remove(list_clave,0);
			log_info(logger,"id ESI desbloqueado de la queue asociada a la key en el diccionario %d",id_esi_desbloqueado);
			ESI* esi1 = NULL;
			//asigno la esi a la variable global para utilizar en la funcion para remover de lista por condicion
			id_esi_global = id_esi_desbloqueado;

	/*lo que se hace a continuacion es para ver si la esi que saque de la cola del diccionario esta en la cola de bloqueados. De ser asi, la
	 muevo a listos y replanifico. En caso de no encontrarla, sigue buscando la proxima, ya que dicha ESI no estaria bloqueada,
	 por lo que replanificar no tendria sentido
	 */
			resultado_lista_satisfy= list_any_satisfy(bloqueados, (void*)identificador_ESI);
			while(key_existente!=1){
				if(resultado_lista_satisfy ==1){
					esi1 = list_remove_by_condition(bloqueados, (void*)identificador_ESI);//recorre la lista y remueve bajo condicion
					log_info(logger,"Procesos removido de bloqueados %d",esi1->id_ESI);
					list_add(listos, (ESI*)esi1);
					log_info(logger,"Procesos agregado a la lista de listos %d",esi1->id_ESI);
					//seteo los semaforos para el sjfcd
					key_existente=1;
					replanificar = 1;
					sem_post(&new_process);
					}
				//Si esta vacia, la esi no existe en la cola de bloqueados
				else{

					log_error(logger,"No existe la esi en la cola de bloqueados %d",id_esi_global);
					//evaluo la proxima ESI que sigue en la queue
					list_vacia = list_is_empty(list_clave);
					if(!list_vacia){
						id_esi_desbloqueado = (int)list_remove(list_clave,0);
						log_info(logger,"id ESI desbloqueado de la queue asociada a la key en el diccionario %d",id_esi_desbloqueado);
						id_esi_global = id_esi_desbloqueado;
						resultado_lista_satisfy= list_any_satisfy(bloqueados, (void*)identificador_ESI);
					}
					else{
						break;
					}
				}
			}
	   }
	    //Si la queue esta vacia, entonces la clave asociada no esta tomada (STORE innecesario)
	    else{
	    	log_info(logger,"la clave no esta tomada");
	    	//list_destroy(list_clave);
	    	//dictionary_remove_and_destroy(claves_bloqueadas,claveAEjecutar,(void*)clave_dictionary_destroy);
				/*if(!list_is_empty(listos)){
					sem_post(&new_process);
					replanificar = 1;
	    		}*/
				/*if(cantidadDeLineas<=1){
									sem_post(&new_process);
									replanificar = 1;
	    	}*/
    	}
    }
	else{
		log_info(logger,"La clave no existe en el diccionario, nada que desbloquear");
	}
}

void desbloquear_del_diccionario(char *claveAEjecutar, int socket){
	int list_vacia = 0;
	int id_esi_desbloqueado = 0;
	int key_existente = 0;
	int resultado_lista_satisfy = 0;
	unsigned char mensaje_coord = 34;
	int tamanio_clave = 0;
	strcpy(clave_bloqueada_global,claveAEjecutar);
	resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave);
	if(resultado_lista_satisfy==1){
	//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
	    list_remove_and_destroy_by_condition(claves_tomadas,(void*)identificador_clave,(void*)clave_destroy);
	    //printf("Clave removida de la lista %s\n",clave_temporal->claveAEjecutar);
	    //free(clave_temporal);
	    log_info(logger,"Clave eliminada correctamente de la lista de tomados");
	    }

    //reviso si la clave existe en el diccionario
    if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
		t_list * list_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
	    list_vacia = list_is_empty(list_clave);
	//reviso si la queue no esta vacia
	    if(!list_vacia){
	    	printf("Entre a queue NO vacia\n");
	    	//saco el primer elemento de la lista, que sera la proxima esi a salir de bloqueados
			id_esi_desbloqueado = (int)list_remove(list_clave,0);
			log_info(logger,"id ESI desbloqueado de la queue asociada a la key en el diccionario %d",id_esi_desbloqueado);
			ESI* esi1 = NULL;//(ESI*) malloc(sizeof(ESI));
			//asigno la esi a la variable global para utilizar en la funcion para remover de lista por condicion
			id_esi_global = id_esi_desbloqueado;

	/*lo que se hace a continuacion es para ver si la esi que saque de la cola del diccionario esta en la cola de bloqueados. De ser asi, la
	 muevo a listos y replanifico. En caso de no encontrarla, sigue buscando la proxima, ya que dicha ESI no estaria bloqueada,
	 por lo que replanificar no tendria sentido
	 */
			resultado_lista_satisfy= list_any_satisfy(bloqueados, (void*)identificador_ESI);
			while(key_existente!=1){
					if(resultado_lista_satisfy ==1){
						esi1 = list_remove_by_condition(bloqueados, (void*)identificador_ESI);//recorre la lista y remueve bajo condicion
						log_info(logger,"Procesos removido de bloqueados %d",esi1->id_ESI);
						list_add(listos, (ESI*)esi1);
						log_info(logger,"Procesos agregado a la lista de listos %d",esi1->id_ESI);
						//seteo los semaforos para el sjfcd
						key_existente=1;
						}
				//Si esta vacia, la esi no existe en la cola de bloqueados
					else{
						log_error(logger,"No existe la esi en la cola de bloqueados %d",id_esi_global);
						//evaluo la proxima ESI que sigue en la queue
						list_vacia = list_is_empty(list_clave);
						if(!list_vacia){
							id_esi_desbloqueado = (int)list_remove(list_clave,0);
							log_info(logger,"id ESI desbloqueado de la queue asociada a la key en el diccionario %d",id_esi_desbloqueado);
							id_esi_global = id_esi_desbloqueado;
							resultado_lista_satisfy= list_any_satisfy(bloqueados, (void*)identificador_ESI);
						}
						else{
							break;
						}
					}
			}
		}
	    else{
	    	log_info(logger,"Enviandole al coordinador un mensaje para liberar la clave");
	    	send(socket,&mensaje_coord,1,0);
			tamanio_clave = strlen(claveAEjecutar);
			send(socket,&tamanio_clave,sizeof(tamanio_clave),0);
			send(socket,claveAEjecutar,tamanio_clave,0);
			replanificar = 1;
			sem_post(&new_process);
			log_info(logger,"clave '%s' desbloqueada correctamente",claveAEjecutar);
	    	}
	    }
	else{
		printf("La clave no existe en el diccionario, nada que desbloquear\n");
	}
}

void clave_dictionary_destroy(t_dictionary *data){
	free(data);
}

void clave_destroy(claves *self) {
    //free(self->claveAEjecutar);
    free(self);
}

void coord_communication(int socket_ESI, int id_ESI ,unsigned char estado_esi, int cantidadDeLineas){
	int size_clave = 0, rc = 0;
	char * clave = NULL;
	unsigned char id_mensaje_coord = 32;
	send(socket_coord, &id_mensaje_coord,1,0);
	//primero hago un recv del coordinador, que me indica que operacion voy a realizar
	rc = recv(socket_coord, &id_mensaje_coord,1, 0);
	if(rc<=0){
		_exit_with_error_and_close(socket_coord, "Se perdio la conexion con el coordinador, desconectando", NULL);
	}

	if(id_mensaje_coord != 25) {
		rc = recv(socket_coord, &size_clave, 4, 0);
		if(rc<=0){
			_exit_with_error_and_close(socket_coord, "Se perdio la conexion con el coordinador, desconectando", NULL);
		}
		clave = (char *) malloc (size_clave +1);
		rc = recv(socket_coord, clave, size_clave, 0);
		if(rc<=0){
			_exit_with_error_and_close(socket_coord, "Se perdio la conexion con el coordinador, desconectando", NULL);
		}
		clave[size_clave] = '\0';
		log_debug(logger,"Recibi la clave %s \n", clave);
	}

	switch (id_mensaje_coord) {
		case 24:
			log_info(logger,"Haciendo GET de la clave '%s' \n", clave);
			ESI_GET(clave, id_ESI, estado_esi);
			break;
		case 26:
			log_info(logger,"Haciendo STORE de la clave '%s' \n", clave);
			ESI_STORE(clave,cantidadDeLineas);
			break;
		case 25:
			//log_info(logger,"Haciendo SET de la clave '%s' \n", clave);
			break;
		default:
			_exit_with_error(socket_ESI,"La ESI sera abortada ya que la instancia esta caida",NULL);
			//TODO: Aca hace algo negro
			break;
	}
	//TODO: Fijate si esto es necesario o explota
	free(clave);
}

void get_keys_bloqueadas_de_entrada(int socket){

	t_queue * queue_clave_inicio = queue_create();
	int tamanio_queue = 0;
	int tamanio_clave = 0;
	unsigned char mensaje_coord = 33;
	//char* string = (char*) malloc(sizeof(config_get_string_value(config_file, "claves_bloqueadas")));
	//char token[40];
	char *token = "";
	const char comma[2] = ",";
	//char string[100] = config_get_string_value(config_file, "claves_bloqueadas");
	esi_bloqueada_de_entrada = 1;
	char* string = config_get_string_value(config_file, "claves_bloqueadas");

	if(string == NULL){
		log_info(logger,"Nada que bloquear por archivo de config");
	}

	else{
		token = strtok(string, comma);

		while( token != NULL ) {
		//printf( " %s\n", token );
		queue_push(queue_clave_inicio,token);
		token = strtok(NULL, comma);
		   }

		tamanio_queue = queue_size(queue_clave_inicio);
		send(socket,&mensaje_coord,1,0);
		send(socket,&tamanio_queue,sizeof(tamanio_queue),0);

		while(!queue_is_empty(queue_clave_inicio)){
			token = queue_pop(queue_clave_inicio);
			tamanio_clave = strlen(token);
			send(socket,&tamanio_clave,sizeof(int),0);
			send(socket,token,tamanio_clave,0);
			ESI_GET(token,-1,0);
		}
	}
	//free(token);
	free(string);
	queue_destroy(queue_clave_inicio);
	esi_bloqueada_de_entrada = 0;
}

/*--------------FIFO------------------------------------
void fifo(){
	//sem_init(&mutex_ejecucion, 0, 1);
	//pthread_mutex_init(&mutex_ejecucion,NULL);
	//int lista_vacia = list_is_empty(ejecucion);

	unsigned char permisoDeEjecucion = 1;
	unsigned char contestacionESI = 0;


	while(1){
		ESI *nodo_lista_ejecucion = NULL;
		int sem_value = 0;
		printf("Esperando que haya un nuevo proceso encolado en listos\n");
		estadoListas();

		//espero a que me digan que hay algo en la cola de listos
		sem_wait(&new_process);
		printf("Nuevo elemento en la cola de listos\n");
		estadoListas();

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		printf("Nodo de listos movido a Ejecucion\n ");
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);

		//Mientras la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		while(nodo_lista_ejecucion->cantidadDeLineas >0){
			sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
			sem_wait(&sem_pausar_algoritmo);
			}

		//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
			send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
		//Espero que la esi me conteste
			recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);
			if (result_connection <= 0) {
						_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
						nodo_lista_ejecucion->cantidadDeLineas = 0;
						}
		//Si es 1, entonces espero que me envie la nueva cantidad de Lineas que tiene
			if(contestacionESI == 2){
				//recibo de la esi la cantidad de lineas
				recv(nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->cantidadDeLineas, sizeof(nodo_lista_ejecucion->cantidadDeLineas),0);
				printf("Cantidad de lineas por ejecutar: %d\n", nodo_lista_ejecucion->cantidadDeLineas);
			}
			else{
				//nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->claveAEjecutar, sizeof(nodo_lista_ejecucion->claveAEjecutar),0);
				//
				//agregar a la estructura
				printf("ESTOY BLOQUEANDO\n");
				laWeaReplanificadoraFIFO(bloqueados,ejecucion);
			}
			//magia con el coord (Recibe si es store/get y realiza acorde)
			coord_communication(nodo_lista_ejecucion->socket_esi,nodo_lista_ejecucion->id_ESI ,contestacionESI);

			}
		free(nodo_lista_ejecucion);
		//free(clave1);//REVISAR SI ESTO SE HACE ACA
		//limpio la lista de ejecucion una vez que termino de ejecutar la ESI
		list_clean(ejecucion);
		printf("Limpio lista\n");
		}
	}
*/
