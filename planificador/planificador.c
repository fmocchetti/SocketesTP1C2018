#include "socket.h"
#include "protocolo.h"




#define IDENTIDAD "planificador"

int main (int argc, char *argv[])
{
	configure_logger();
	printf("%s\n",IDENTIDAD);
	create_server(32, 3 * 60 * 1000);
	return 0;
}

