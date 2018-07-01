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


		// creo el storage y la tabla
		char* storage = (char*)malloc(init.cantidad_entradas*init.tamanioEntrada);
		if(storage==NULL) {
			_exit_with_error(server, "No pude alocar la memoria", NULL);

		}

		log_info(logger, "Cree el storage");

		*storage =0;

		t_list* tabla = list_create();


		while(1){

			/******************************************************************************************************************/





				char* posicionDeLectura = storage;

				char* posicionFinDeMemoria = (storage+(init.cantidad_entradas*init.tamanioEntrada));



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
					STORE(tabla,clave, "",logger);
				}
				usleep(init.retardo * 1000);
				log_info(logger, "Ya termine de ejecutar esa instruccion");
				identificador = 1;
				send(server, &identificador, 1, 0);
		}

	return EXIT_SUCCESS;
}
