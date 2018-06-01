#include "circular.h"
#include "tabla.h"
#include "socket_client.h"

typedef struct {
	int cantidad_entradas;
	int tamanioEntrada;
	int retardo;
} valores_iniciales;


int main () {
	valores_iniciales init;
	char identificador = 0;

	configure_logger();

	int server = create_client("127.0.0.1","12345");

	log_info(logger,"Estado server %d \n", server);

	recv(server, &identificador, 1, 0);
	size_t messageLength;
	recv(server, &messageLength, 4, 0);
	void *message = malloc(messageLength);
	recv(server, message, messageLength, 0);

	memcpy(&init.cantidad_entradas, message, 4);
	memcpy(&init.tamanioEntrada, message+4, 4);
	memcpy(&init.retardo, message+8, 4);

	log_info(logger, "Valores iniciales %d, %d, %d", init.cantidad_entradas, init.tamanioEntrada, init.retardo);

	log_info(logger, "Recibi la inicializacion");

	/**********************************************************************/


	// creo el storage y la tabla
	char* storage = (char*)malloc(init.cantidad_entradas*init.tamanioEntrada);

	*storage =0;

	char* posicionDeLectura = storage;

	char* posicionFinDeMemoria = (storage+(init.cantidad_entradas*init.tamanioEntrada));

	t_list* tabla = list_create();



	//recibo los datos

	recv(server, &identificador, 1, 0);
	recv(server, &messageLength, 4, 0);
	message = malloc(messageLength);
	recv(server, message, messageLength, 0);


	//lleno estructura que le paso al set_cicular
	struct ClaveValor claveValor;

	claveValor.tamanioEntrada = init.tamanioEntrada;



	int tamanioClave,tmananioValor;

	//OJO el campo claveValor.clave es char[40] tamaño fijado de la clave, en el tp  lo pide

	memcpy(&tamanioClave, message+4, 4);
	memcpy(&claveValor.clave, message+8, tamanioClave);
	memcpy(&tmananioValor, message+8+tamanioClave, 4);
	memcpy(&claveValor.valor, message+12+tamanioClave, tmananioValor);
	memcpy(&init.retardo, message+12+tamanioClave+tmananioValor, 4);

	log_info(logger, "Valores iniciales %d, %d, %d", claveValor.clave, claveValor.valor, init.retardo);

	log_info(logger, "Recibi clave y valor");

	//inserto en memoria
	SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);


	//vemos que se guardo
	puts(storage);

	//libero memoria
    free(storage);
    liberar_recursos(&tabla);

/******************************************************************************************************************/










	//correrTestsCircular();

	//correrTestsLRU();

	return EXIT_SUCCESS;
}
