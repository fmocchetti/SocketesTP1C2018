#include "socketClient.h"
#include "esi.h"

t_config * config_file;

int main(int argc, char **argv){

	pthread_t thread_plani, thread_coordi;
	int r1, r2;
	FILE *script;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	bool respuestaCoordinador;

	configure_logger();

	config_file = config_create("esi.conf");


	/*r1 = pthread_create(&thread_plani, NULL, planificador, NULL);
	r2 = pthread_create(&thread_coordi, NULL, coordinador, NULL);
	pthread_join(thread_plani, NULL);
	pthread_join(thread_coordi, NULL);*/

	int socket_planificador = create_client(config_get_string_value(config_file, "ip_planificador"), config_get_string_value(config_file, "puerto_planificador"));
	int socket_coordinador = create_client(config_get_string_value(config_file, "ip_coordinador"), config_get_string_value(config_file, "puerto_coordinador"));


	script = fopen(argv[1], "r");

	if (script == NULL){
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, script)) != -1) {
		t_esi_operacion parsed = parse(line);

		if(parsed.valido){
			if(solicitudDeEjecucionPlanificador(socket_planificador)){
				respuestaCoordinador = envioYRespuestaCoordinador(socket_coordinador, parsed);
				enviarRespuestaAlPlanificador(socket_planificador, respuestaCoordinador);
		    }else{
		    	printf("El planificador fallo\n");
		    	exit(EXIT_FAILURE);
		    }

		    destruir_operacion(parsed);
		}else{
			fprintf(stderr, "La linea <%s> no es valida\n", line);
		    exit(EXIT_FAILURE);
		}

	}

	fclose(script);

	if (line)
		free(line);

	return EXIT_SUCCESS;

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


bool solicitudDeEjecucionPlanificador(int socket){

	int buffer;
	int rc;

	rc = recv(socket, buffer, sizeof(buffer), 0);

	if (rc == 0){
		printf("Desconexion con el Planificador");
		exit(EXIT_FAILURE);
	}else if (rc == -1){
		printf("ERROR");
		exit(EXIT_FAILURE);
	}else{
		if(buffer != 1){
			free(socket);
			return false;
		}
	}


	free(socket);
	return true;
}


bool envioYRespuestaCoordinador(int socket, t_esi_operacion parsed){

	//int send(int socket, void * mensaje, int cant, int flags); //retorna cantidad de bytes enviados o -1 en caso de error
	//int recv(int socket, void * buffer, int cant, int flags); //retorna la cantidad de bytes recibidos, 0 ante desconexión del servidor y -1 ante un error.

	/*char header[1] = ‘\0’;
	int payloadSize;
	if (strcmp(header, “0”)) // 0 fallo, 1 exito
		payloadSize = 64;
	else
		payloadSize = 64;

	recv(socket, header, payloadSize, 0);*/ //Y, por último, leemos el contenido de lo que resta del paquete:


	return true;
}



void enviarRespuestaAlPlanificador(int socket, bool respuesta){

	int sd;

	if(respuesta){
		sd = send(socket, 2, sizeof(int), 0);
	}else{
		sd = send(socket, 9, sizeof(int), 0);
	}

	if (sd == -1){
		_exit_with_error(socket, "No se pudo enviar", NULL);
	}

	free(socket);
}
