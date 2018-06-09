#include "instancia.h"
typedef struct {
	int cantidad_entradas;
	int tamanioEntrada;
	int retardo;
} valores_iniciales;

pthread_mutex_t lock_dump;


int respaldar_informacion_thread(parametros_dump* parametros){

	while(1){

		sleep(parametros->intervaloDeDump);
		pthread_mutex_unlock(&lock_dump);

		log_info(parametros->logger,"Comienza el proceso respaldo de Informacion en: %s",parametros->puntoDeMontaje);

		int tamanioTabla = list_size(parametros->tabla);
		if(tamanioTabla <= 0){
			log_error(parametros->logger,"La tabla se encontro vacia, o error al obtener dimension de tabla");
			return(-1);
		}

		struct Dato* unDato;

		int j =0;

		for(int i = 0 ; i < tamanioTabla ; i++){

			unDato = list_get( parametros->tabla, i );
			//char* ruta = puntoDeMontaje;
			//const char* clave =(char*) unDato->clave;
			//ruta = strcat(ruta,clave);
			char* ruta = unDato->clave;
			int fd = open(ruta, O_RDWR | O_CREAT, (mode_t)0600);
			lseek(fd, unDato->cantidadDeBytes-1, SEEK_SET);
			write(fd, "", 1);
			char *map = mmap(0, unDato->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			memcpy(map, unDato->posicionMemoria, unDato->cantidadDeBytes);
			msync(map, unDato->cantidadDeBytes, MS_SYNC);
			munmap(map, unDato->cantidadDeBytes);
			close(fd);
			log_info(parametros->logger,"Se guardo un valor en archivo: %s",ruta);
			j++;

		}
		log_info(parametros->logger,"Finaliza el proceso respaldo de Informacion..");
		log_info(parametros->logger,"Se crearon %d archivos",j);

		pthread_mutex_lock(&lock_dump);

	}

return EXIT_SUCCESS;
}

int STORE(t_list* tabla,char* clave,char* ruta,t_log* logger){



	pthread_mutex_unlock(&lock_dump);

	struct Dato* claveBuscada = buscar(tabla,clave);

	if(claveBuscada == NULL){

		log_info(logger,"No se encontro la Clave en STORE: %s", clave);
		return (1);

	}

	//char* ruta = puntoDeMontaje;
	//const char* clave =(char*) unDato->clave;
	//ruta = strcat(ruta,clave);


	char* rutaArmada = malloc(sizeof(ruta)+sizeof(clave)+1);

	//memcpy(rutaArmada,ruta,sizeof(ruta));
	//memcpy(rutaArmada+sizeof(ruta),'/',1);
	//memcpy(rutaArmada+sizeof(clave)+1,clave,sizeof(clave));
	strcpy(rutaArmada,ruta);
	strcpy(rutaArmada,"/");
	strcpy(rutaArmada,clave);

	log_info(logger,"Se guardara el archivo en ruta: %s",rutaArmada);


	int fd = open(rutaArmada, O_RDWR | O_CREAT, (mode_t)0600);
	log_info(logger,"Se crea el archivo: %d",fd);
	lseek(fd, claveBuscada->cantidadDeBytes-1, SEEK_SET);
	write(fd, "", 1);
	char *map = mmap(0, claveBuscada->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(map, claveBuscada->posicionMemoria, claveBuscada->cantidadDeBytes);
	msync(map, claveBuscada->cantidadDeBytes, MS_SYNC);
	munmap(map, claveBuscada->cantidadDeBytes);
	close(fd);
	free(rutaArmada);
	log_info(logger,"Se cierra el archivo");

	pthread_mutex_lock(&lock_dump);

return 0;
}




//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
int SET_circular(char** posicionDeLectura,t_list** tabla,struct ClaveValor* claveValor,char* primeraPosicionMemoria,char* posicionFinalMemoria){

	pthread_mutex_unlock(&lock_dump);
	struct Dato unDato;

	int longitudS = strlen(claveValor->valor);
	int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	//si no hay lugar para todo el string lo parto y coloco lo que entra y el resto al principio
	//de la memoria
	if( no_hay_lugar( espacioAOcupar, *posicionDeLectura, posicionFinalMemoria) ){

		int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

		memcpy(*posicionDeLectura,claveValor->valor,espacioRestante);

		cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);

		actualizarTabla(tabla,longitudS - espacioRestante);

		registrar_dato_en_tabla(tabla,&unDato);

		*posicionDeLectura=primeraPosicionMemoria;

		memcpy(*posicionDeLectura,((claveValor->valor)+espacioRestante),espacioAOcupar-espacioRestante);

		*posicionDeLectura += (espacioAOcupar-espacioRestante);

		return 0;

	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,espacioAOcupar);

	cargar_info_en_dato(&unDato,*posicionDeLectura/*,longitudS*/,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);

	*posicionDeLectura += espacioAOcupar;

	pthread_mutex_lock(&lock_dump);

}

// chequeo si el espacio libre es menor al tamaño requerido
int no_hay_lugar(int tamanio,char* posicionDeLectura,char* posicionFinalMemoria){

return ((posicionFinalMemoria - posicionDeLectura)<tamanio);
}

int calcular_cantidad_entradas(int longitudS,int tamEntrada){

	int resto = longitudS % tamEntrada;
	int resultado = longitudS / tamEntrada;
	if(tamEntrada > longitudS){

		return 1;
	}
	else if(resto > 0){

		return resultado + 1;
	}
return resultado;
}

void cargar_info_en_dato(struct Dato* unDato,char* posicionDeLectura,struct ClaveValor* claveValor){

	int longitudS = strlen(claveValor->valor);

	(unDato)->posicionMemoria = posicionDeLectura;
	(unDato)->cantidadDeBytes = longitudS;
	(unDato)->frecuenciaUso = 0;
	strcpy((char*)(unDato->clave),claveValor->clave);
	//memcpy((unDato->clave),claveValor->clave,40);
}

int main () {


	pthread_mutex_init(&lock_dump, NULL);
/*
	t_config * config_file = config_create("instancia.conf");

	char* intervaloDump = config_get_string_value(config_file, "Intervalo de dump: ");
	char* puntoMontaje = config_get_string_value(config_file, "Montaje: ");

*/







	valores_iniciales init;
		char identificador = 0;
		char * clave= 0;
		char * valor= 0;
		char * ruta= 0;
		int size_clave = 0, size_valor = 0,size_ruta =  0;

		configure_logger();

		int server = create_client("127.0.0.1","12346");

		log_info(logger,"Estado server %d \n", server);

		// creo el storage y la tabla
		char* storage = (char*)malloc(init.cantidad_entradas*init.tamanioEntrada);

		log_info(logger, "Cree el storage");

		*storage =0;

		t_list* tabla = list_create();




		//creo thread dump

		log_info(logger, "Cree el thread Dump");

		pthread_t threadDumpeador;

		parametros_dump* parametros = malloc(sizeof(parametros_dump));

		parametros->logger = logger;
		parametros->puntoDeMontaje = "";
		parametros->storage = storage;
		parametros->tabla = tabla;
		parametros->intervaloDeDump = 10;//intervaloDump; //tomar del recv


		if( pthread_create(&threadDumpeador, NULL, (void *)respaldar_informacion_thread,parametros)) {
			log_error(logger, "Error creating thread Dump");
		}

		//fin creo thread


		recv(server, &identificador, 1, 0);
		int messageLength;
		log_info(logger,"Recibi %d \n", identificador);
		recv(server, &messageLength, 4, 0);
		log_info(logger,"Recibi %d \n", messageLength);
		void * message = malloc(messageLength);
		int rec =recv(server, message, messageLength, 0);
		log_info(logger,"Recibi %d bytes", rec);

		memcpy(&init.cantidad_entradas, message, 4);
		log_info(logger,"Cantidad entradas: %d", init.cantidad_entradas);

		memcpy(&init.tamanioEntrada, message+4, 4);
		log_info(logger,"Tamanio entradas: %d", init.tamanioEntrada);

		memcpy(&init.retardo, message+8, 4);
		log_info(logger,"Retardo: %d", init.retardo);

		free(message);

		log_info(logger, "Valores iniciales %d, %d, %d", init.cantidad_entradas, init.tamanioEntrada, init.retardo);

		log_info(logger, "Recibi la inicializacion");


		while(1){

			/******************************************************************************************************************/





				char* posicionDeLectura = storage;

				char* posicionFinDeMemoria = (storage+(init.cantidad_entradas*init.tamanioEntrada));



				//creo thread dump
			/*
				log_info(logger, "Cree el thread Dump");
				pthread_t threadDumpeador;

				parametros_dump* parametros = malloc(sizeof(parametros_dump));

				parametros->logger = logger;
				parametros->puntoDeMontaje = "";
				parametros->storage = storage;
				parametros->tabla = tabla;
				parametros->intervaloDeDump = 10; //tomar del recv


				if( pthread_create(&threadDumpeador, NULL, (void *)respaldar_informacion_thread,parametros)) {
					log_error(logger, "Error creating thread Dump");
				}


			*/

				//recibo operacion

				recv(server, &identificador, 1, 0);

				log_info(logger, "Identificador %d", identificador);


				//recibo clave
				int rc = recv(server, &size_clave, 4, 0);

				log_info(logger, "size_clave %d %d", size_clave, rc);

				clave = (char *)malloc(size_clave + 1);

				rc = recv(server, clave, size_clave, 0);
				log_info(logger, "Clave %d", rc);

				clave[size_clave] = '\0';

				log_info(logger, "Clave %s", clave);

				log_info(logger, "Identificador %d", identificador);

				//TODO: Si es un get aca no tiene que recibir nada
				if(identificador == 22){

					//lleno estructura que le paso al set_cicular
					struct ClaveValor claveValor;

					claveValor.tamanioEntrada = init.tamanioEntrada;

					int tamanioClave,tmananioValor;

					//OJO el campo claveValor.clave es char[40] tamaño fijado de la clave, en el tp  lo pide

					tamanioClave = (int) size_clave;

					log_info(logger, "Asigne tamanioClave %d" , size_clave);
						//memcpy(&(claveValor.clave), clave, size_clave);
					strcpy(claveValor.clave,clave);


					log_info(logger, "Asigne claveValor.clave");
					tmananioValor = (int) size_clave;

					log_info(logger, "Asigne tmananioValor");
					claveValor.valor = malloc(size_valor);
					//memcpy(&(claveValor.valor), valor, size_valor);
					log_info(logger, "Asigne claveValor.valor");

					log_info(logger, "Asigne las estructuras");

					recv(server, &size_valor, 4, 0);
					valor = malloc(size_valor + 1);
					recv(server, valor, size_valor, 0);
					valor[size_valor] = '\0';
					log_info(logger, "Valor %s", valor);

					claveValor.valor=valor;

					//memcpy(&init.retardo, message+12+tamanioClave+tmananioValor, 4);

					log_info(logger, "Valores iniciales %s, %s, %d", clave, valor, init.retardo);

					log_info(logger, "Recibi clave y valor");


				//	pthread_mutex_unlock (& ​​lock);
					//inserto en memoria
					SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);
				//	pthread_mutex_unlock (& ​​lock);


					//vemos que se guardo
					puts(storage);
				} else if(identificador==23) {//store
					/*recv(server, &size_ruta, 4, 0);
					log_info(logger, "Size Valor %s", size_ruta);
					ruta = malloc(size_ruta + 1);
					recv(server, ruta, size_ruta, 0);
					ruta[size_ruta] = '\0';
					log_info(logger, "Valor %s", ruta);*/

					ruta = malloc(strlen(""));
					strcpy(ruta, "");
					/*
					struct ClaveValor claveValor;
					claveValor.tamanioEntrada = init.tamanioEntrada;
					strcpy(claveValor.clave, clave);

					valor = malloc(strlen("pabloPuto"));
					strcpy(valor,"pabloPuto");
					claveValor.valor = valor;

					SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);
					*/
					STORE(tabla,clave,ruta,logger);
				}
				usleep(init.retardo * 1000);
				identificador = 1;
				send(server, &identificador, 1, 0);
		}
		//libero memoria
	    //free(storage);//free(parametros);
	    //liberar_recursos(&tabla);



  /*******************************************************************************************************************/





//-----------ACA LOS TESTS-------------------------------------------------------------------------------------

	//correrTestsCircular();

	//correrTestsLRU();

    //correrTestsDump();

	//correrTestsSTORE();

	return EXIT_SUCCESS;
}






