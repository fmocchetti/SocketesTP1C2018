#include "socket.h"

#define IDENTIDAD "coordinador"

int listening_port = 0;
int cantidad_entradas = 20;
int size_key = 200;
int retardo = 300;
enum {	LSU, EL, KE};

int main (int argc, char *argv[])
{
	configure_logger();

	printf("%s\n",IDENTIDAD);

	create_server(32, 20 * 60 * 1000, THREAD_CONNECTION, 12345);

	exit_gracefully(0);
}

void inicializar_instancia (int socket) {
	char identificador = INICIALIZAR_INSTANCIA;
	size_t messageLength = sizeof(int) * 3;
	void * buffer = malloc (messageLength + 6);
	memcpy(buffer, &identificador, 1);
	memcpy(buffer+1, &messageLength, 4);
	memcpy(buffer+5, &cantidad_entradas, 4);
	memcpy(buffer+9, &size_key, 4);
	memcpy(buffer+13, &retardo, 4);
	send(socket, buffer, 5 + messageLength, 0);
	log_info(logger, "Inicializacion enviada correctamente");
}
