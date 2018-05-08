#include "socket.h"

#define IDENTIDAD "coordinador"

int listening_port = 0;
int cantidad_entradas = 0;
int size_key = 0;
int retardo = 0;
enum {	LSU, EL, KE};

int main (int argc, char *argv[])
{
	configure_logger();

	printf("%s\n",IDENTIDAD);

	create_server(32, 20 * 60 * 1000, THREAD_CONNECTION, 12345);

	return 0;
}
