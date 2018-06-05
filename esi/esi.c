#include "socketClient.h"
#include "esi.h"

t_config * config_file;

int main(int argc, char **argv){

	ESI* esi = (ESI*) malloc(sizeof(ESI));
	esi->id_ESI = 0;
	esi->cantidadDeLineas = 0;
	unsigned char mensaje_a_planificador_de_mi_info = 18;
	FILE *script;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	bool respuestaCoordinador;
	int numeroDeLinea = 0; //contador de lineas del parser
	int sd;

	configure_logger();

	config_file = config_create("esi.conf");

	//me conecto con el coordinador y el planificador
	int socket_planificador = create_client(config_get_string_value(config_file, "ip_planificador"), config_get_string_value(config_file, "puerto_planificador"));
	int socket_coordinador = create_client(config_get_string_value(config_file, "ip_coordinador"), config_get_string_value(config_file, "puerto_coordinador"));


	//abro el script para lectura
	script = fopen(argv[1], "r");

	if (script == NULL){
		perror("Error al abrir el archivo: \n");
		exit(EXIT_FAILURE);
	}

	//Cuento cuantas lineas tiene mi archivo
	while ((read = getline(&line, &len, script)) != -1) {
		numeroDeLinea++;
	}

	esi->cantidadDeLineas = numeroDeLinea;

	rewind(script);

	//Envio al planificador informacion de mi estado
	sd = send(socket_planificador, &mensaje_a_planificador_de_mi_info, 1, 0);
	if (sd < 0){
		printf("Error en enviar cantidadDeLineas al planificador\n");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, script)) != -1) {
		t_esi_operacion parsed = parse(line); //leo y parseo la primer linea del archivo


		//envio al planificador la cantidad de lineas del archivo y espero la solicitud de ejecucion

		sd = send(socket_planificador, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);

		if (sd < 0){
			printf("Error en enviar cantidadDeLineas al planificador\n");
			exit(EXIT_FAILURE);
		}

		numeroDeLinea--;

		if(parsed.valido){
			//Primero recivo el id de la esi que me envia el planificador
			esi->id_ESI = recv(socket_planificador, &esi->id_ESI , sizeof(esi->id_ESI), 0);

			if(solicitudDeEjecucionPlanificador(socket_planificador)){
				//si el planificador me da el ok, primero guardo clave y valor en la estructura de la esi

				switch(parsed.keyword){
					case GET:
						esi->clave = parsed.argumentos.GET.clave;
				        break;
				    case SET:
				        esi->clave = parsed.argumentos.SET.clave;
				        esi->valor = parsed.argumentos.SET.valor;
				        break;
				    case STORE:
				        esi->clave = parsed.argumentos.STORE.clave;
				        break;
				    default:
				        fprintf(stderr, "No pude interpretar <%s>\n", line);
				        exit(EXIT_FAILURE);
				}
				//envio la esi y recivo la respuesta del coordinador
				respuestaCoordinador = envioYRespuestaCoordinador(socket_coordinador, esi);
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


	free(esi);
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

	int solicitudPlanificador;
	int rc;

	//recivo del planificador el ok para mandar la esi al coordinador
	rc = recv(socket, &solicitudPlanificador, sizeof(solicitudPlanificador), 0);


	if (rc == 0){
		printf("Desconexion con el Planificador\n");
		exit(EXIT_FAILURE);
	}else if (rc == -1){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}else{
		if(solicitudPlanificador != 1){
			free(socket);
			return false;
		}
	}

	//Si el buffer es 1 entonces es OK (segun el protocolo)
	free(socket);
	return true;
}


bool envioYRespuestaCoordinador(int socket, ESI* esi){


	int sd;
	int respuestaCoordinador;
	int rc;

	//envio el id, la clave y el valor al coordinador
	sd = send(socket, &esi->id_ESI, sizeof(esi->id_ESI), 0);
	send(socket, &esi->clave, sizeof(esi->clave), 0);
	send(socket, &esi->valor, sizeof(esi->valor), 0);

	if (sd < 0){
		printf("Error en enviar esi al coordinador\n");
		exit(EXIT_FAILURE);
	}


	//recivo del coordinador la respuesta que debo enviar al planificador
	rc = recv(socket, &respuestaCoordinador, sizeof(respuestaCoordinador), 0);

	if (rc == 0){
		printf("Desconexion con el Coordinador\n");
		exit(EXIT_FAILURE);
	}else if (rc == -1){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}else{
		if(respuestaCoordinador != 7){
			free(socket);
			return false;
		}
	}

	//Si el buffer es 7 entonces es OK (segun el protocolo)
	free(socket);
	return true;
}



void enviarRespuestaAlPlanificador(int socket, bool respuesta){

	int respuestaOk = 2; //Si la respuesta es OK (del coordinador)
	int falloRespuesta = 12;

	if(respuesta){ //respuesta del coordinador
		send(socket, &respuestaOk, sizeof(int), 0); //Envio el 2, segun el protocolo es un OK
	}else{
		send(socket, &falloRespuesta, sizeof(int), 0);
	}

	free(socket);

}
