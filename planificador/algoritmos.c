/*
 * algoritmos.c
 *
 *  Created on: 12 may. 2018
 *      Author: pdelucchi
 */


#include "algoritmos.h"

int id_esi_global = 0;
char clave_bloqueada_global[40];
int result_connection = 0;
bool result_satisfy =false;


void laWeaReplanificadoraFIFO(t_list * listaDestino, t_list *listaEntrada){
    t_list * tempList = list_create();
    tempList = list_take_and_remove(listaEntrada, 1);
    list_add_all(listaDestino, tempList);
    list_destroy(tempList);
}

//muestra el estado de los procesos encolados en las listas existentes
void estadoListas(){
    int cantidadListos = 0;
    int cantidadEjecucion = 0;
    int cantidadBloqueados = 0;
    int cantidadTerminados = 0;
    cantidadEjecucion = list_size(ejecucion);
    cantidadListos = list_size(listos);
    cantidadBloqueados = list_size(bloqueados);
    cantidadTerminados = list_size(terminados);
    printf("Procesos en la cola de listos %d\n",cantidadListos);
    printf("Procesos en la cola de ejecucion %d\n",cantidadEjecucion);
    printf("Procesos en la cola de bloqueados %d\n",cantidadBloqueados);
    printf("Procesos en la cola de terminados %d\n",cantidadTerminados);

}

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


void sjfsd(){
	//sem_init(&mutex_ejecucion, 0, 1);
	//pthread_mutex_init(&mutex_ejecucion,NULL);
	//int lista_vacia = list_is_empty(ejecucion);
	int sem_value = 0;

	unsigned char permisoDeEjecucion = 1;
	unsigned char contestacionESI = 0;

	while(1){
		ESI *nodo_lista_ejecucion = NULL;
		printf("Estas en SJFSD\n");
		printf("Esperando que haya un nuevo proceso encolado en listos\n");
		//espero a que me digan que hay algo en la cola de listos
		sem_wait(&new_process);
		printf("Nuevo elemento en la cola de listos\n");
		estadoListas();

		//replanifico aca, dependiendo de la rafaga
		list_sort(listos, (void*)sort_by_estimacion);

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		printf("Nodo de listos movido a Ejecucion\n ");
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		id_esi_global = nodo_lista_ejecucion->id_ESI;

		//Mientras la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		while(nodo_lista_ejecucion->cantidadDeLineas >0){
			sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
				sem_wait(&sem_pausar_algoritmo);
			}

			//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
			send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);

			//Espero que la esi me conteste
			result_connection = recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);
			if (result_connection <= 0) {
				//////////elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
				printf("------RECIBI MENOS DE 0\n");
				claves* clave_temporal = (claves*) malloc(sizeof(claves));
				clave_temporal = list_remove_by_condition(claves_tomadas,identificador_ESI);
				ESI_STORE(clave_temporal->claveAEjecutar);
				//list_remove_and_destroy_by_condition(claves_tomadas,(void*)identificador_ESI,(void*)clave_destroy);
				free(clave_temporal);
				//////////
				//hago close del socket
				_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
				break;
				nodo_lista_ejecucion->cantidadDeLineas = 0;

				printf("------RECIBI MENOS DE 123123132\n");
			}

			printf("contestacionESI %d\n",contestacionESI);
			//Si es 1, entonces espero que me envie la nueva cantidad de Lineas que tiene
			if(contestacionESI == 2){
				//recibo de la esi la cantidad de lineas
				//recv(nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->cantidadDeLineas, sizeof(nodo_lista_ejecucion->cantidadDeLineas),0);
				nodo_lista_ejecucion->cantidadDeLineas --;
				//printf("Cantidad de lineas por ejecutar: %d\n", nodo_lista_ejecucion->cantidadDeLineas);
			} else {
				//nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->claveAEjecutar, sizeof(nodo_lista_ejecucion->claveAEjecutar),0);
				//
				//agregar a la estructura
				printf("ESTOY BLOQUEANDO\n");
				laWeaReplanificadoraFIFO(bloqueados,ejecucion);
				break;
			}

				//magia con el coord (Recibe si es store/get y realiza acorde)
				coord_communication(nodo_lista_ejecucion->socket_esi,nodo_lista_ejecucion->id_ESI ,contestacionESI);


/*BAKCUP NO BORRAR HASTA PROBAR
				//primero hago un recv del coordinador, que me indica que operacion voy a realizar
				recv(socket_coord,&id_mensaje_coord,sizeof(id_mensaje_coord),0);
				claves *clave1= (claves*) malloc(sizeof(claves)); //DEFINO ESTRUCTURA PARA RECIBIR CLAVE
				//Recibo del coordinador la clave que la ESI va a ejecutar
				recv(socket_coord,&clave1->claveAEjecutar,sizeof(clave1->claveAEjecutar),0);


				switch (id_mensaje_coord) {
					case 24:
						ESI_GET(clave1->claveAEjecutar,nodo_lista_ejecucion->socket_esi);
						break;
					case 26:
						ESI_STORE(clave1->claveAEjecutar);
						break;
					case 25:
						printf("Recibi un dato innecesario");
						break;
					default:
						//TODO: Aca hace algo negro
						break;
				}*/
		}
		//////////elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
		claves* clave_temporal = (claves*) malloc(sizeof(claves));


		clave_temporal = list_remove_by_condition(claves_tomadas,(void*)identificador_ESI);

		printf("ENTRO ACAAAAAAAA\n");
		ESI_STORE(clave_temporal->claveAEjecutar);

		free(clave_temporal);

		//////////


		free(nodo_lista_ejecucion);
		//free(clave1);//REVISAR SI ESTO SE HACE ACA
		//limpio la lista de ejecucion una vez que termino de ejecutar la ESI
		list_clean(ejecucion);
		printf("Limpio lista\n");
	}
}

void sjfcd(){
	//sem_init(&mutex_ejecucion, 0, 1);
	//pthread_mutex_init(&mutex_ejecucion,NULL);
	//int lista_vacia = list_is_empty(ejecucion);

	unsigned char permisoDeEjecucion = 1;
	unsigned char contestacionESI = 0;
	int sem_value = 0;
	int lista_vacia = 0;
	int result_connection = 0;

	while(1){
		ESI *nodo_lista_ejecucion = NULL;//(ESI*) malloc(sizeof(ESI));
		printf("Estas en SJFCD\n");
		printf("Esperando que haya un nuevo proceso encolado en listos\n");

		//espero a que me digan que hay un nuevo proceso en listos
		sem_wait(&new_process);

		printf("Nuevo elemento en la cola de listos\n");
		estadoListas();

		//replanifico dependiendo de la rafaga
		if(replanificar == 1){
		printf("Replanificando\n");
		list_sort(listos, (void*)sort_by_estimacion);
		replanificar = 0;
		}

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		printf("Nodo de listos movido a Ejecucion\n ");
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		printf("ID de la ESI a ejecutar %d\n", nodo_lista_ejecucion->id_ESI);

		//sem_getvalue(&new_process,&sem_value);
		//Ejecuto la esi seleccionada hasta recibir algun evento que necesite replanificar(nueva esi en listos, de bloqueado a listos, etc).
		while(replanificar == 0){
			sem_getvalue(&sem_pausar_planificacion,&sem_value);
			if(sem_value<1){
				sem_wait(&sem_pausar_algoritmo);
			}
		//Si la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		if(nodo_lista_ejecucion->cantidadDeLineas >0){

		//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
				send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
		//Espero que la esi me conteste
				result_connection = recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);
				if (result_connection <= 0) {
				    _exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
				    nodo_lista_ejecucion->cantidadDeLineas = 0;
				  }
				//printf("contestacionESI %d\n",contestacionESI);
		//Si es 2, entonces resto 1 a cada linea faltante y resto 1 por cada ejecucion de sentencia a la rafaga
				else if(contestacionESI == 2){
					//recibo de la esi la cantidad de lineas
					//recv(nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->cantidadDeLineas, sizeof(nodo_lista_ejecucion->cantidadDeLineas),0);
					nodo_lista_ejecucion->cantidadDeLineas --;
					printf("Cantidad de lineas por ejecutar: %d\n", nodo_lista_ejecucion->cantidadDeLineas);
					nodo_lista_ejecucion->rafaga --;
				}
				else{
					//nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->claveAEjecutar, sizeof(nodo_lista_ejecucion->claveAEjecutar),0);
					//agregar a la estructura
					//agrego a bloqueados en caso de recibir otra contestacion de la ESI
					printf("ESTOY BLOQUEANDO\n");
					laWeaReplanificadoraFIFO(bloqueados,ejecucion);
					break;
				}

				//magia con el coord (Recibe si es store/get y realiza acorde)
				coord_communication(nodo_lista_ejecucion->socket_esi,nodo_lista_ejecucion->id_ESI ,contestacionESI);
		}
		//Si la cantidad de lineas es menor a 0, muevo la ESI a la cola de terminados
		else{
			//printf("Entre a 1\n");
			laWeaReplanificadoraFIFO(terminados,ejecucion);
			estadoListas();
			break;					//
		}
	}
		//si el valor de replanificar cambia, se sale del while y se evalua si la lista de ejecucion no esta vacia
		lista_vacia = list_is_empty(ejecucion);
		printf("lista vacia %d\n",lista_vacia);
		if(lista_vacia != 1){

		//De ser afirmativo, se mueve la esi que estaba ejecutando a listos para su replanificacion
			//printf("Entre a 3\n");
			laWeaReplanificadoraFIFO(listos,ejecucion);
			replanificar = 1;					//
			sem_post(&new_process);
			//free(nodo_lista_ejecucion);
		}
}
}

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
	printf("Entre a identificador_ESI\n");
	claves *esi1= (claves*) data; //recibo estructura de la lista?
	printf("ID de la ESI: %d\n",esi1->id_ESI);
	if(esi1->id_ESI == id_esi_global) {
		return true;
	}
	return false;
}

bool identificador_clave(void * data){
	claves *clave1= (claves*) data; //recibo estructura de la lista?
	printf("Clave Bloqueada: %s\n",clave1->claveAEjecutar);
	if(strcmp(clave1->claveAEjecutar,clave_bloqueada_global)==0) {
		return true;
	}
	return false;
}

void element_destroyer(void * data){
	free(data);
}

void ESI_GET(char * claveAEjecutar, int id_ESI, unsigned char respuesta_ESI){
	/*
	//si la clave no esta en la lista, la agrego y continuo
	strcpy(clave_bloqueada_global,claveAEjecutar);
	claves* clave1 = (claves*) malloc(sizeof(claves));
	strcpy(clave1->claveAEjecutar,claveAEjecutar);
	clave1->id_ESI = id_ESI;

	result_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave);
	if(!result_satisfy){
		list_add(listos, (claves*)clave1);
		//hacer un get para testear
	} else */
	claves* clave1 = (claves*) malloc(sizeof(claves));
	strcpy(clave1->claveAEjecutar,claveAEjecutar);
	clave1->id_ESI = id_ESI;

	if(respuesta_ESI==2){
		list_add(claves_tomadas, (claves*)clave1);
	}
	else if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
		printf("Entre en 1\n");
		t_queue * queue_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
		//Si la queue ya existe, se pushea el nuevo id_ESI en la cola de la clave bloqueada
		queue_push(queue_clave, &id_ESI);
	} else {
		//Si no existe la clave, creo la cola asociada, pusheo el id_ESI y agrego la clave con su cola asociada
		printf("Entre en 2\n");
		t_queue * queue_clave = queue_create();
		queue_push(queue_clave, &id_ESI);
		dictionary_put(claves_bloqueadas, claveAEjecutar, queue_clave);
	}
}

/*BACKUP - NO BORRAR HASTA TESTEAR
 * void ESI_GET(char * claveAEjecutar, int * id_ESI){
	if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
	    	printf("Entre en 1\n");
	    	t_queue * queue_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
	    	//Si la queue ya existe, se pushea el nuevo id_ESI en la cola de la clave bloqueada
	    		queue_push(queue_clave, id_ESI);
	    	}
	    		//Si no existe la clave, creo la cola asociada, pusheo el id_ESI y agrego la clave con su cola asociada
	    else{
	    	printf("Entre en 2\n");
	    	t_queue * queue_clave = queue_create();
	    	queue_push(queue_clave, id_ESI);
	    	dictionary_put(claves_bloqueadas, claveAEjecutar, queue_clave);
	    }
}*/

void ESI_STORE(char *claveAEjecutar){
	int queue_vacia = 0;
    int list_vacia = 0;
    int id_esi_desbloqueado = 0;
    //reviso si la clave existe en el diccionario
	if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
		t_queue * queue_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
	    queue_vacia = queue_is_empty(queue_clave);
	    printf("Estado QUEUE: %d\n",queue_vacia);
	//reviso si la queue no esta vacia
	    if(!queue_vacia){
	    	printf("Entre a queue vacia\n");
	    	//hago un pop de la queue, que sera la proxima esi a salir de bloqueados
			id_esi_desbloqueado = (int)queue_pop(queue_clave);
			printf("id ESI bloqueado %d\n",id_esi_desbloqueado);
			ESI* esi1 = (ESI*) malloc(sizeof(ESI));
			//asigno la esi a la variable global para utilizar en la funcion para remover de lista por condicion
			id_esi_global = id_esi_desbloqueado;
			t_list * lista_temporal = list_filter(bloqueados,(void*)identificador_ESI);
			list_vacia = list_is_empty(lista_temporal);
			//chequeo si la lista no esta vacia, remuevo la esi de bloqueados y la muevo a listos
			if(list_vacia != 1){
				//printf("Entre a queue vacia\n",id_esi_desbloqueado);
				esi1 = list_remove_by_condition(bloqueados, (void*)identificador_ESI);//recorre la lista y remueve bajo condicion
				printf("Procesos removido de bloqueados %d\n",esi1->id_ESI);
				list_add(listos, (ESI*)esi1);
				//seteo los semaforos para el sjfcd
				replanificar = 1;
				sem_post(&new_process);
				}
			//Si esta vacia, la esi no existe en la cola de bloqueados
			else{
				dictionary_remove_and_destroy(claves_bloqueadas, claveAEjecutar, (void*)clave_dictionary_destroy);
				printf("No existe la esi en la cola de bloqueados %d\n",esi1->id_ESI);
				}
	    	}
	    //Si la queue esta vacia, entonces la clave asociada no esta tomada (STORE innecesario)
	    else{
	    	printf("la clave no esta tomada\n");
	    	}
	    }
	else{
		printf("La clave no existe en el diccionario\n");
	}
}
/*BACKUP - NO BORRAR HASTA TESTEAR
void ESI_STORE(char *claveAEjecutar){
	int queue_vacia = 0;
    int list_vacia = 0;
    int id_esi_desbloqueado = 0;
    //reviso si la clave existe en el diccionario
	if(dictionary_has_key(claves_bloqueadas,claveAEjecutar)){
		t_queue * queue_clave = dictionary_get(claves_bloqueadas,claveAEjecutar);
	    queue_vacia = queue_is_empty(queue_clave);
	//reviso si la queue no esta vacia
	    if(queue_vacia!=1){
	    	//hago un pop de la queue, que sera la proxima esi a salir de bloqueados
			id_esi_desbloqueado = queue_pop(queue_clave);
			printf("id ESI bloqueado %d\n",id_esi_desbloqueado);
			ESI* esi1 = (ESI*) malloc(sizeof(ESI));
			//asigno la esi a la variable global para utilizar en la funcion para remover de lista por condicion
			id_esi_global = id_esi_desbloqueado;
			t_list * lista_temporal = list_filter(bloqueados,(void*)identificador_ESI);
			list_vacia = list_is_empty(lista_temporal);
			//chequeo si la lista no esta vacia, remuevo la esi de bloqueados y la muevo a listos
			if(list_vacia != 1){
				esi1 = list_remove_by_condition(bloqueados, (void*)identificador_ESI);//recorre la lista y remueve bajo condicion
				printf("Procesos removido de bloqueados %d\n",esi1->id_ESI);
				list_add(listos, (ESI*)esi1);
				//seteo los semaforos para el sjfcd
				replanificar = 1;
				sem_post(&new_process);
				}
			//Si esta vacia, la esi no existe en la cola de bloqueados
			else{
				dictionary_remove_and_destroy(claves_bloqueadas, claveAEjecutar, (void*)clave_destroy);
				printf("No existe la esi en la cola de bloqueados %d\n",esi1->id_ESI);
				}
	    	}
	    //Si la queue esta vacia, entonces la clave asociada no esta tomada (STORE innecesario)
	    else{
	    	printf("la clave no esta tomada\n");
	    	}
	    }
	else{
		printf("La clave no existe en el diccionario\n");
	}
}*/

void clave_dictionary_destroy(t_dictionary *data){
	free(data);
}

void clave_destroy(claves *self) {
    free(self->claveAEjecutar);
    free(self);
}

void coord_communication(int socket_ESI, unsigned char id_ESI ,unsigned char estado_esi){
	int size_clave = 0, rc = 0;
	char * clave = NULL;
	unsigned char id_mensaje_coord = 0;
	//primero hago un recv del coordinador, que me indica que operacion voy a realizar
	rc = recv(socket_coord, &id_mensaje_coord,1, 0);
	printf("Recibi %d bytes del coordinador \n", rc);
	printf("Recibi del coordinador %d \n", id_mensaje_coord);
	if(id_mensaje_coord != 25) {
		recv(socket_coord, &size_clave, 4, 0);
		clave = (char *) malloc (size_clave +1);
		recv(socket_coord, clave, size_clave, 0);
		clave[size_clave] = '\0';
		printf("Recibi la clave %s \n", clave);
	}

	switch (id_mensaje_coord) {
		case 24:
			printf("Entre al get \n");
			ESI_GET(clave, id_ESI, estado_esi);
			break;
		case 26:
			printf("Entre al store \n");
			ESI_STORE(clave);
			break;
		case 25:
			printf("Entre al set \n");
			break;
		default:
			//TODO: Aca hace algo negro
			break;
	}
	//TODO: Fijate si esto es necesario o explota
	free(clave);
}

/*
void get_keys_bloqueadas_de_entrada(){
	char string[100] = config_get_string_value(config_file, "claves_bloqueadas");

}*/
/*
//Si la clave ya existe en el diccionario
					if(dictionary_has_key(claves_bloqueadas,clave){
						t_queue * queue_clave = dictionary_get(claves_bloqueadas,clave);
						//Si la queue ya existe, se pushea el nuevo id_ESI en la cola de la clave bloqueada
						if(queue_clave != NULL){
							queue_push(queue_clave, nodo_lista_ejecucion->id_ESI);
						}
						//Si la queue no existe, se crea y se pushea el nuevo id_ESI en la cola de la clave bloqueada recientemente creada
						else{
							queue_clave = queue_create();
							queue_push(queue_clave, nodo_lista_ejecucion->id_ESI);
						}
						//Si no existe la clave, creo la cola asociada, pusheo el id_ESI y agrego la clave con su cola asociada
				   else{
						t_queue * queue_clave = queue_create();
						queue_push(queue_clave, nodo_lista_ejecucion->id_ESI);
						dictionary_put(claves_bloqueadas, clave, queue_clave);
						}

						//STORE
						 * if(dictionary_has_key(claves_bloqueadas,clave){
					t_queue * queue_clave = dictionary_get(claves_bloqueadas,clave)
//					int id_esi_desbloqueado = queue_pop(queue_clave);
					dictionary_put(claves_bloqueadas, clave, queue_clave);
					ESI *esi= (ESI*) malloc(sizeof(ESI));
					tuwea = id_esi_bloqueado;
					esi = list_remove_by_condition(bloqueados, identificador_ESI);//revisar como concha se hace esto
				 	list_add(listos, (ESI*)esi);
					}
						 *
						*/
