#include "socketClient.h"
#include <pthread.h>
#include <parsi/parser.h>


void * planificador();
void * coordinador();

t_config * config_file;

int main(int argc, char **argv){
	pthread_t thread_plani, thread_coordi;
	int r1, r2;
	char * line = NULL;

	configure_logger();

	config_file = config_create("esi.conf");

	t_esi_operacion parsed = parse(line);

	r1 = pthread_create(&thread_plani, NULL, planificador, NULL);
	r2 = pthread_create(&thread_coordi, NULL, coordinador, NULL);
	pthread_join(thread_plani, NULL);
	pthread_join(thread_coordi, NULL);

	exit_gracefully(0);
}


void * planificador () {
	printf("Entre al thread del planificador \n");
	int planificador = create_client(config_get_string_value(config_file, "ip_planificador"), config_get_string_value(config_file, "puerto_planificador"));
	send_message(planificador);
	return NULL;
}

void * coordinador () {
	printf("Entre al thread del coordinador \n");
	int coordinador = create_client(config_get_string_value(config_file, "ip_coordinador"), config_get_string_value(config_file, "puerto_coordinador"));
	send_message(coordinador);
	return NULL;
}
