#include "socketClient.h"
#include "esi.h"

//t_config * config_file;

int main(int argc, char **argv){
	//pthread_t thread_plani, thread_coordi;
	//int r1, r2;
	FILE *script;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	/*configure_logger();

	config_file = config_create("esi.conf");


	r1 = pthread_create(&thread_plani, NULL, planificador, NULL);
	r2 = pthread_create(&thread_coordi, NULL, coordinador, NULL);
	pthread_join(thread_plani, NULL);
	pthread_join(thread_coordi, NULL);*/


	script = fopen(argv[1], "r");

	if (script == NULL){
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, script)) != -1) {
		t_esi_operacion parsed = parse(line);

		if(parsed.valido){
			if(solicitudDeEjecucionPlanificador() == true){
				printf("La solicitud del coordinador fue true");
				enviarAlCoordinador(parsed);
				if (respuestaCoordinador() == true){
					printf("Respuesta del coordinador fue exito");
					enviarRespuestaAlPlanificador();
				}else{
					printf("Respuesta del coordinador fue fallo");
					exit(EXIT_FAILURE);
				}
		    }else{
		    	printf("El planificador no te quiere");
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


/*void * planificador () {
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
}*/


bool solicitudDeEjecucionPlanificador(){


	return true;
}


void enviarAlCoordinador(t_esi_operacion parsed){


}


bool respuestaCoordinador(){

	return true;
}

void enviarRespuestaAlPlanificador(){


}
