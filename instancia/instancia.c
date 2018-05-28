#include "circular.h"
#include "tabla.h"
#include "socket_client.h"

typedef struct {
	int cantidad_entradas;
	int size_key;
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
	memcpy(&init.size_key, message+4, 4);
	memcpy(&init.retardo, message+8, 4);

	log_info(logger, "Valores iniciales %d, %d, %d", init.cantidad_entradas, init.size_key, init.retardo);

	log_info(logger, "Recibi la inicializacion");

	//correrTestsCircular();

	correrTestsLRU();

	return EXIT_SUCCESS;
}
