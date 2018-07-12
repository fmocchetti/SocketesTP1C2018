#include "socket.h"

#define IDENTIDAD "coordinador"

int main (int argc, char *argv[])
{
	configure_logger();

	printf("%s\n",IDENTIDAD);

	config_file = config_create("coordinador.conf");

	pthread_mutex_init(&mutex, NULL);
	sem_init(&mutex_planificador, 1, 0);
	sem_init(&mutex_instancia, 1, 0);
	sem_init(&mutex_status, 1, 0);
	instancia_to_find = total_instancias = 0;
	list_instances = list_create();
	diccionario_claves = dictionary_create();
	cantidad_entradas = config_get_int_value(config_file, "cantidad_entradas");
	size_key = config_get_int_value(config_file, "size_entrada");
	retardo = config_get_int_value(config_file, "retardo");
	algoritmo_elegido =  config_get_int_value(config_file, "algoritmo_distribucion");

	create_server(32, 20 * 60 * 1000, THREAD_CONNECTION, config_get_int_value(config_file, "listening_port"));

	exit_gracefully(0);
}
