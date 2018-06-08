#include "circular.h"
#include "tabla.h"
#include "socket_client.h"
#include "Dump.h"
#include <pthread.h>
#include "STORE.h"
typedef struct {
	int cantidad_entradas;
	int tamanioEntrada;
	int retardo;
} valores_iniciales;

pthread_mutex_t lock;

int main () {



	valores_iniciales init;
		char identificador = 0;
		char * clave= 0;
		char * valor= 0;
		char * ruta= 0;
		int size_clave = 0, size_valor = 0,size_ruta =  0;

		configure_logger();

		int server = create_client("127.0.0.1","12346");

		log_info(logger,"Estado server %d \n", server);

		// creo el storage y la tabla
		char* storage = (char*)malloc(init.cantidad_entradas*init.tamanioEntrada);

		log_info(logger, "Cree el storage");

		*storage =0;

		t_list* tabla = list_create();


		while(1){

				recv(server, &identificador, 1, 0);
				int messageLength;
				log_info(logger,"Recibi %d \n", identificador);
				recv(server, &messageLength, 4, 0);
				log_info(logger,"Recibi %d \n", messageLength);
				void * message = malloc(messageLength);
				int rec =recv(server, message, messageLength, 0);
				log_info(logger,"Recibi %d bytes", rec);

				memcpy(&init.cantidad_entradas, message, 4);
				log_info(logger,"Cantidad entradas: %d", init.cantidad_entradas);

				memcpy(&init.tamanioEntrada, message+4, 4);
				log_info(logger,"Tamanio entradas: %d", init.tamanioEntrada);

				memcpy(&init.retardo, message+8, 4);
				log_info(logger,"Retardo: %d", init.retardo);

				free(message);

				log_info(logger, "Valores iniciales %d, %d, %d", init.cantidad_entradas, init.tamanioEntrada, init.retardo);

				log_info(logger, "Recibi la inicializacion");

			/******************************************************************************************************************/





				char* posicionDeLectura = storage;

				char* posicionFinDeMemoria = (storage+(init.cantidad_entradas*init.tamanioEntrada));



				//creo thread dump
			/*
				log_info(logger, "Cree el thread Dump");
				pthread_t threadDumpeador;

				parametros_dump* parametros = malloc(sizeof(parametros_dump));

				parametros->logger = logger;
				parametros->puntoDeMontaje = "";
				parametros->storage = storage;
				parametros->tabla = tabla;
				parametros->intervaloDeDump = 10; //tomar del recv


				if( pthread_create(&threadDumpeador, NULL, (void *)respaldar_informacion_thread,parametros)) {
					log_error(logger, "Error creating thread Dump");
				}


			*/

				//recibo operacion

				recv(server, &identificador, 1, 0);

				log_info(logger, "Identificador %d", identificador);


				//recibo clave
				int rc = recv(server, &size_clave, 4, 0);

				log_info(logger, "size_clave %d %d", size_clave, rc);

				clave = (char *)malloc(size_clave + 1);

				rc = recv(server, clave, size_clave, 0);
				log_info(logger, "Clave %d", rc);

				clave[size_clave] = '\0';

				log_info(logger, "Clave %s", clave);

				log_info(logger, "Identificador %d", identificador);

				//TODO: Si es un get aca no tiene que recibir nada
				if(identificador == 22){

					//lleno estructura que le paso al set_cicular
					struct ClaveValor claveValor;

					claveValor.tamanioEntrada = init.tamanioEntrada;

					int tamanioClave,tmananioValor;

					//OJO el campo claveValor.clave es char[40] tamaño fijado de la clave, en el tp  lo pide

					tamanioClave = (int) size_clave;

					log_info(logger, "Asigne tamanioClave %d" , size_clave);
						//memcpy(&(claveValor.clave), clave, size_clave);
					strcpy(claveValor.clave,clave);


					log_info(logger, "Asigne claveValor.clave");
					tmananioValor = (int) size_clave;

					log_info(logger, "Asigne tmananioValor");
					claveValor.valor = malloc(size_valor);
					//memcpy(&(claveValor.valor), valor, size_valor);
					log_info(logger, "Asigne claveValor.valor");

					log_info(logger, "Asigne las estructuras");

					recv(server, &size_valor, 4, 0);
					valor = malloc(size_valor + 1);
					recv(server, valor, size_valor, 0);
					valor[size_valor] = '\0';
					log_info(logger, "Valor %s", valor);

					claveValor.valor=valor;

					//memcpy(&init.retardo, message+12+tamanioClave+tmananioValor, 4);

					log_info(logger, "Valores iniciales %s, %s, %d", clave, valor, init.retardo);

					log_info(logger, "Recibi clave y valor");


				//	pthread_mutex_unlock (& ​​lock);
					//inserto en memoria
					SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);
				//	pthread_mutex_unlock (& ​​lock);


					//vemos que se guardo
					puts(storage);
				} else if(identificador==23) {//store
					/*recv(server, &size_ruta, 4, 0);
					log_info(logger, "Size Valor %s", size_ruta);
					ruta = malloc(size_ruta + 1);
					recv(server, ruta, size_ruta, 0);
					ruta[size_ruta] = '\0';
					log_info(logger, "Valor %s", ruta);*/

					ruta = malloc(strlen(""));
					strcpy(ruta, "");
					/*
					struct ClaveValor claveValor;
					claveValor.tamanioEntrada = init.tamanioEntrada;
					strcpy(claveValor.clave, clave);

					valor = malloc(strlen("pabloPuto"));
					strcpy(valor,"pabloPuto");
					claveValor.valor = valor;

					SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);
					*/
					STORE(tabla,clave,ruta,logger);
				}

		}
		//libero memoria
	    //free(storage);//free(parametros);
	    //liberar_recursos(&tabla);



  /*******************************************************************************************************************/





//-----------ACA LOS TESTS-------------------------------------------------------------------------------------

	//correrTestsCircular();

	//correrTestsLRU();

    //correrTestsDump();

	//correrTestsSTORE();

	return EXIT_SUCCESS;
}
