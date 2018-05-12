#include "socket.h"
#include "protocolo.h"




#define IDENTIDAD "planificador"

int main (int argc, char *argv[])
{
	//Creo las listas a utilizar
	listos = list_create();
	bloqueados = list_create();
	ejecucion = list_create();
	terminados = list_create();

	//Configuro el log a utilizar
	configure_logger();

	printf("%s\n",IDENTIDAD);// borrar

	//Creo el server
	create_server(32, 3 * 60 * 1000);
	return 0;
}

