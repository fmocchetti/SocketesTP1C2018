/*
 * algoritmos.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */


#include "algoritmos.h"
#include "protocolo.h"
int id_esi_global = 0;


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

		//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
				send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
		//Espero que la esi me conteste
				recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);
				//printf("contestacionESI %d\n",contestacionESI);
		//Si es 1, entonces espero que me envie la nueva cantidad de Lineas que tiene
				if(contestacionESI == 1){
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

				//primero hago un recv del coordinador, que me indica que operacion voy a realizar
				//recv(coord,&id_mensaje_coord,sizeof(id_mensaje_coord),0);
				//clave *clave1= (clave*) malloc(sizeof(clave)); DEFINO ESTRUCTURA PARA RECIBIR CLAVE
				//Recibo del coordinador la clave que la ESI va a ejecutar
				//recv(coord,&clave1->claveAEjecutar,sizeof(clave1->claveAEjecutar),0);

				/*if(id_mensaje_coord == 24){ //GET
					ESI_GET(clave1->claveAEjecutar,nodo_lista_ejecucion->socket_esi);
				}



				if(id_mensaje_coord == 26){ //STORE
				ESI_STORE(clave1->claveAEjecutar)
				}

				if(id_mensaje_coord == 25){ //SET hace algo??
				}

				else{
				//ERROR

				}




								if diccionario tiene la clave
								 obtenes el elem del diccionario
								 te fijas si existe la cola
								 si exite haces un push esi
								 si no existe creas y haces push esi
								else
								 metes la clave en el diccionario
								 creas la cola
								 haces push de la esi

				*/
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

		//Mientras la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		while(nodo_lista_ejecucion->cantidadDeLineas >0){

		//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
				send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
		//Espero que la esi me conteste
				recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);
				//printf("contestacionESI %d\n",contestacionESI);
		//Si es 1, entonces espero que me envie la nueva cantidad de Lineas que tiene
				if(contestacionESI == 1){
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

				//primero hago un recv del coordinador, que me indica que operacion voy a realizar
				//recv(coord,&id_mensaje_coord,sizeof(id_mensaje_coord),0);
				//clave *clave1= (clave*) malloc(sizeof(clave)); DEFINO ESTRUCTURA PARA RECIBIR CLAVE
				//Recibo del coordinador la clave que la ESI va a ejecutar
				//recv(coord,&clave1->claveAEjecutar,sizeof(clave1->claveAEjecutar),0);

				/*if(id_mensaje_coord == 24){ //GET
					ESI_GET(clave1->claveAEjecutar,nodo_lista_ejecucion->socket_esi);
				}



				if(id_mensaje_coord == 26){ //STORE
				ESI_STORE(clave1->claveAEjecutar)
				}

				if(id_mensaje_coord == 25){ //SET hace algo??
				}

				else{
				//ERROR

				}




								if diccionario tiene la clave
								 obtenes el elem del diccionario
								 te fijas si existe la cola
								 si exite haces un push esi
								 si no existe creas y haces push esi
								else
								 metes la clave en el diccionario
								 creas la cola
								 haces push de la esi

				*/
		}
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

	while(1){
		ESI *nodo_lista_ejecucion = NULL;
		printf("Estas en SJFCD\n");
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


		while((int)new_process.__align == 0 && nodo_lista_ejecucion->cantidadDeLineas >0){
			printf("Entre AL WHILE \n");
		//Mientras la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		if(nodo_lista_ejecucion->cantidadDeLineas >0){

		//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
				send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, 1, 0);
		//Espero que la esi me conteste
				recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, 1,0);
				//printf("contestacionESI %d\n",contestacionESI);
		//Si es 1, entonces espero que me envie la nueva cantidad de Lineas que tiene
				if(contestacionESI == 1){
					//recibo de la esi la cantidad de lineas
					recv(nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->cantidadDeLineas, sizeof(nodo_lista_ejecucion->cantidadDeLineas),0);
					printf("Cantidad de lineas por ejecutar: %d\n", nodo_lista_ejecucion->cantidadDeLineas);
					nodo_lista_ejecucion->rafaga --;
				}
				else{
					//nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->claveAEjecutar, sizeof(nodo_lista_ejecucion->claveAEjecutar),0);
					//
					//agregar a la estructura
					printf("ESTOY BLOQUEANDO\n");
					laWeaReplanificadoraFIFO(bloqueados,ejecucion);
				}

				//primero hago un recv del coordinador, que me indica que operacion voy a realizar
				//recv(coord,&id_mensaje_coord,sizeof(id_mensaje_coord),0);
				//clave *clave1= (clave*) malloc(sizeof(clave)); DEFINO ESTRUCTURA PARA RECIBIR CLAVE
				//Recibo del coordinador la clave que la ESI va a ejecutar
				//recv(coord,&clave1->claveAEjecutar,sizeof(clave1->claveAEjecutar),0);

				/*if(id_mensaje_coord == 24){ //GET
					ESI_GET(clave1->claveAEjecutar,nodo_lista_ejecucion->socket_esi);
				}



				if(id_mensaje_coord == 26){ //STORE
				ESI_STORE(clave1->claveAEjecutar)
				}

				if(id_mensaje_coord == 25){ //SET hace algo??
				}

				else{
				//ERROR

				}
				*/


						//free(clave1);//REVISAR SI ESTO SE HACE ACA
						//limpio la lista de ejecucion una vez que termino de ejecutar la ESI
						//list_clean(ejecucion);
						//printf("Limpio lista\n");
		}


	}
		laWeaReplanificadoraFIFO(listos,ejecucion);
				free(nodo_lista_ejecucion);
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
	claves *esi1= (claves*) data; //recibo estructura de la lista?
	printf("ID de la ESI: %d\n",esi1->id_ESI);
	if(esi1->id_ESI == id_esi_global) {
		return true;
	}
	return false;
}

void element_destroyer(void * data){
	free(data);
}

void ESI_GET(char * claveAEjecutar, int * id_ESI){
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
}

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
				}
			//Si esta vacia, la esi no existe en la cola de bloqueados
			else{
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
