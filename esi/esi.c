#include "socket_client.h"
#include "esi.h"

t_config * config_file;

int main(int argc, char **argv){

	ESI* esi = (ESI*) malloc(sizeof(ESI));
	esi->id_ESI = 0;
	esi->cantidadDeLineas = 0;
	esi->operacion = 0;
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

	//envio al planificador el id y la cantidad de lineas del archivo y espero la solicitud de ejecucion
	send(socket_planificador, &mensaje_a_planificador_de_mi_info, 1, 0);
    sd = send(socket_planificador, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
	if (sd < 0){
		printf("Error en enviar cantidadDeLineas al planificador\n");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, script)) != -1) {
		t_esi_operacion parsed = parse(line); //leo y parseo la primer linea del archivo

		numeroDeLinea--;

		if(parsed.valido){
			//Primero recivo el id de la esi que me envia el planificador
			//esi->id_ESI = recv(socket_planificador, &esi->id_ESI , sizeof(esi->id_ESI), 0);

			if(solicitudDeEjecucionPlanificador(socket_planificador)){
				//si el planificador me da el ok, primero guardo clave y valor en la estructura de la esi
				esi->operacion = parsed.keyword;
				switch(parsed.keyword){
					case GET:
						esi->clave = parsed.argumentos.GET.clave;
						log_info(logger, "GET Clave: %s", esi->clave);
				        break;
				    case SET:
				        esi->clave = parsed.argumentos.SET.clave;
				        log_info(logger, "SET Clave: %s", esi->clave);
				        esi->valor = parsed.argumentos.SET.valor;
				        log_info(logger, "SET Valor: %s", esi->valor);
				        break;
				    case STORE:
				        esi->clave = parsed.argumentos.STORE.clave;
				        log_info(logger, "STORE Clave: %s", esi->clave);
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

bool envioYRespuestaCoordinador(int socket, ESI* esi){
	int sd, rc;
	unsigned char identificador = esi->operacion;


	int size_clave = strlen(esi->clave);
	int size_valor = 0;
	int messageLength = 5 + size_clave;

	if(esi->operacion == SET) {
		size_valor = strlen(esi->valor);
		messageLength += 4 + size_valor;
	}

	identificador = esi->operacion + 21;
	char * mensajes = (char *) malloc (messageLength);
	memcpy(mensajes, &(identificador), 1);
	memcpy(mensajes+1, &size_clave, 4);
	memcpy(mensajes+5, esi->clave, size_clave);
	printf("Clave: %.*s %d %s\n", size_clave, mensajes+5,size_clave, esi->clave);
	if(esi->operacion == SET) {
		memcpy(mensajes+5+size_clave, &size_valor, 4);
		memcpy(mensajes+9+size_clave, esi->valor, size_valor);
		printf("Valor: %.*s %d %s\n", size_valor, mensajes+9+size_clave, size_valor, esi->valor);
	}

	log_info(logger, "Enviandole a la instancia %d bytes", messageLength);
	sd = send(socket, mensajes, messageLength, 0);
	free(mensajes);

	if (sd < 0){
		printf("Error en enviar esi al coordinador\n");
		exit(EXIT_FAILURE);
	}

	//recivo del coordinador la respuesta que debo enviar al planificador
	rc = recv(socket, &identificador, 1, 0);

	log_info(logger, "El coordinador me dijo %d", identificador);

	if (rc == 0){
		printf("Desconexion con el Coordinador\n");
		exit(EXIT_FAILURE);
	}else if (rc == -1){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}else{
		if(identificador != 7){
			return false;
		}
	}

	//Si el buffer es 7 entonces es OK (segun el protocolo)
	return true;
}



void enviarRespuestaAlPlanificador(int socket, bool respuesta){
	unsigned char message;
	int sd;

	if(respuesta){ //respuesta del coordinador
		message = 2;
		sd =send(socket, &message, 1, 0);//Envio el 2, segun el protocolo es un OK
	} else {
		message = 9;
		sd = send(socket, &message, 1, 0);
	}

	if (sd == -1){
		exit(EXIT_FAILURE);
	}
}
