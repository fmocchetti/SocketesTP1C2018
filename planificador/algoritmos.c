/*
 * algoritmos.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */


#include "algoritmos.h"
#include "protocolo.h"



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
	ESI *nodo_lista_ejecucion =(ESI*) malloc(sizeof(ESI));
	int permisoDeEjecucion = 1;
	int contestacionESI = 0;
	sem_init(&mutex_ejecucion, 0, 1);
	//pthread_mutex_init(&mutex_ejecucion,NULL);
	//int lista_vacia = list_is_empty(ejecucion);

	while(1){

		printf("Esperando que haya un nuevo proceso encolado en listos\n");
		estadoListas();

		//espero a que me digan que hay algo en la cola de listos
		sem_wait(&new_process);

		//Muevo de la lista de listos, el primer nodo a la lista de ejecucion
		laWeaReplanificadoraFIFO(ejecucion,listos);
		printf("Nodo de listos movido a Ejecucion ");
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);

		//Mientras la cantidadDeLineas de la ESI en ejecucion sea mayor a 0
		while(nodo_lista_ejecucion->cantidadDeLineas >0){

		//Envio al socket de la esi que esta en ejecucion, que puede ejecutarse
				send(nodo_lista_ejecucion->socket_esi, &permisoDeEjecucion, sizeof(permisoDeEjecucion), 0);
		//Espero que la esi me conteste
				recv(nodo_lista_ejecucion->socket_esi, &contestacionESI, sizeof(contestacionESI),0);
		//Si es 1, entonces espero que me envie la nueva cantidad de Lineas que tiene
				if(contestacionESI == 1){
					recv(nodo_lista_ejecucion->socket_esi, &nodo_lista_ejecucion->cantidadDeLineas, sizeof(nodo_lista_ejecucion->cantidadDeLineas),0);
					printf("Cantidad de lineas por ejecutar: %d\n", nodo_lista_ejecucion->cantidadDeLineas);
				}
				else{
					laWeaReplanificadoraFIFO(bloqueados,ejecucion);
				}
		}
		//limpio la lista de ejecucion una vez que termino de ejecutar la ESI
		list_clean(ejecucion);
		printf("Limpio lista\n");
		sem_post(&mutex_listos);
	}
	free(nodo_lista_ejecucion);
}



/*
int main(){

    listos = list_create();
    bloqueados = list_create();
    ejecucion = list_create();
    terminados = list_create();
    t_proceso *proceso1= (t_proceso*) malloc(sizeof(t_proceso));
    proceso1->ID = 1;
    t_proceso* proceso2 =(t_proceso*) malloc(sizeof(t_proceso));
    proceso2->ID = 2;
    t_proceso* proceso3 =(t_proceso*) malloc(sizeof(t_proceso));
    t_proceso* proceso4 =(t_proceso*) malloc(sizeof(t_proceso));



    //list_add(listos, (t_proceso*)&proceso1);
    list_add(listos, (t_proceso*)proceso1);
    list_add(listos, (t_proceso*)proceso2);
    proceso3 =  (t_proceso*) list_get(listos, 0);
    if(proceso3->ID ==1){

        //tempList = list_take_and_remove(listos, 1);
        //list_add_all(ejecucion, tempList);
        //list_clean(tempList);
        laWeaReplanificadora(ejecucion,listos);


        proceso3 =  (t_proceso*) list_get(listos, 0);
    }
    if(proceso3->ID ==2){
        laWeaReplanificadora(bloqueados,listos);
    }
    estadoListas();

    /*
    free(proceso1);
    free(proceso2);
    free(proceso3);
    free(proceso4);
}*/


// mueve el primer nodo de la lista de entrada a la lista de destino

/*
proceso4 =  (t_proceso*) list_get(ejecucion, 0);
        printf("Procesos en la cola de ejecucion %d\n",proceso4->ID);
        proceso3 =  (t_proceso*) list_get(listos, 0);
        printf("Procesos en la cola de listos %d\n",proceso3->ID);*/
