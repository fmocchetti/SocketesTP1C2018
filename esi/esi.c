#include "socket_client.h"
#include "esi.h"

t_config * config_file;

int main(int argc, char **argv){

	ESI* esi = (ESI*) malloc(sizeof(ESI));
	esi->id_ESI = 0;
	esi->cantidadDeLineas = 0;
	unsigned char mensaje_a_planificador_de_mi_info = 18;
	pthread_t thread_plani, thread_coordi;
	int r1, r2;
	FILE *script;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	bool respuestaCoordinador;
	int numeroDeLinea = 0;

	configure_logger();

	config_file = config_create("esi.conf");


	/*r1 = pthread_create(&thread_plani, NULL, planificador, NULL);
	r2 = pthread_create(&thread_coordi, NULL, coordinador, NULL);
	pthread_join(thread_plani, NULL);
	pthread_join(thread_coordi, NULL);*/

	//me conecto con el coordinador y el planificador
	int socket_planificador = create_client(config_get_string_value(config_file, "ip_planificador"), config_get_string_value(config_file, "puerto_planificador"));
	int socket_coordinador = create_client(config_get_string_value(config_file, "ip_coordinador"), config_get_string_value(config_file, "puerto_coordinador"));


	script = fopen(argv[1], "r");

	if (script == NULL){
		perror("Error al abrir el archivo: \n");
		exit(EXIT_FAILURE);
	}

	//Cuento cuantas lineas tiene mi archivo
	while ((read = getline(&line, &len, script)) != -1) {
		numeroDeLinea++;
	}

	//printf("El numero de lineas es: %s \n", numeroDeLinea);
	esi->cantidadDeLineas = numeroDeLinea;

	rewind(script);

	//envio al planificador el id y la cantidad de lineas del archivo y espero la solicitud de ejecucion
	send(socket_planificador, &mensaje_a_planificador_de_mi_info, 1, 0);
	int sd = send(socket_planificador, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
	if (sd < 0){
		printf("Error en enviar cantidadDeLineas al planificador\n");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, script)) != -1) {
		t_esi_operacion parsed = parse(line);

		numeroDeLinea--;

		if(parsed.valido){
			if(solicitudDeEjecucionPlanificador(socket_planificador)){
				respuestaCoordinador = envioYRespuestaCoordinador(socket_coordinador, esi, mensaje_a_planificador_de_mi_info);
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

	unsigned char buffer;
	int rc;

	//recivo del planificador el ok para mandar la esi al coordinador
	rc = recv(socket, &buffer, sizeof(buffer), 0);

	if (rc == 0){
		printf("Desconexion con el Planificador\n");
		exit(EXIT_FAILURE);
	}else if (rc == -1){
		printf("ERROR bad address\n");
		exit(EXIT_FAILURE);
	}else if(buffer != 1){
		log_error(logger, "El buffer es %d", buffer);
		return false;
	}

	//Si el buffer es 1 entonces es OK (segun el protocolo)
	return true;
}


bool envioYRespuestaCoordinador(int socket, ESI* esi, unsigned char mensaje){
	int sd;
	unsigned char buffer;
	int rc;

	sd = send(socket, &esi->id_ESI, sizeof(esi->cantidadDeLineas), 0);
	send(socket, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
	send(socket, &mensaje, 1, 0);
	if (sd < 0){
		printf("Error en enviar esi al coordinador\n");
		exit(EXIT_FAILURE);
	}


	//recivo del coordinador la respuesta que debo enviar al planificador
	rc = recv(socket, &buffer, sizeof(buffer), 0);

	if (rc == 0){
		printf("Desconexion con el Coordinador\n");
		exit(EXIT_FAILURE);
	}else if (rc == -1){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}else{
		if(buffer != 7){
			return false;
		}
	}

	//Si el buffer es 7 entonces es OK (segun el protocolo)
	return true;
}



void enviarRespuestaAlPlanificador(int socket, bool respuesta){
	unsigned char message;
	int sd;
	respuesta = 1;

	if(respuesta){ //respuesta del coordinador
		message = 2;
		sd =send(socket, &message, 1, 0);//Envio el 2, segun el protocolo es un OK
	}else{
		message = 9;
		sd = send(socket, &message, 1, 0);
	}

	if (sd == -1){
		exit(EXIT_FAILURE);
	}
}
