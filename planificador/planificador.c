#include "socketServer.h"

int main (int argc, char *argv[])
{
	char identidad []= "planificador";
	configure_logger();
	printf("%s",identidad);
	createServer(32, 3 * 60 * 1000, identidad);
	return 0;
}
