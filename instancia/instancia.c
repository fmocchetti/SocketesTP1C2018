#include "instancia.h"


int respaldar_informacion_thread(parametros_dump* parametros){

	while(1){

		sleep(parametros->intervaloDeDump);
		pthread_mutex_lock(&lock_dump);


		log_info(logger,"****Comienza el proceso respaldo de Informacion en: %s ****",parametros->puntoDeMontaje);

		int tamanioTabla = list_size(parametros->tabla);
		if(tamanioTabla <= 0){
			log_info(logger,"**La tabla se encontro vacia, no hay nada para respaldar**");
			pthread_mutex_unlock(&lock_dump);
		}
		else{

			struct Dato* unDato;

			int j =0;

			char* rutaArmada = malloc(strlen(parametros->puntoDeMontaje)+40+strlen(getenv("HOME"))+1);
			*rutaArmada = 0;
			for(int i = 0 ; i < tamanioTabla ; i++){

				unDato = list_get( parametros->tabla, i );

				strcpy(rutaArmada,parametros->puntoDeMontaje);
				strcat(rutaArmada,(const char*)unDato->clave);

				int fd = open(rutaArmada, O_RDWR | O_CREAT, (mode_t)0600);
				if(fd < 0){
					log_error(logger,"Error al abrir la ruta, puede que el directorio %s no exista",parametros->puntoDeMontaje);
					return EXIT_FAILURE;
				}
				lseek(fd, unDato->cantidadDeBytes-1, SEEK_SET);
				write(fd, "", 1);
				char *map = mmap(0, unDato->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				memcpy(map, unDato->posicionMemoria, unDato->cantidadDeBytes);
				msync(map, unDato->cantidadDeBytes, MS_SYNC);
				munmap(map, unDato->cantidadDeBytes);
				close(fd);
				log_info(logger,"Se guardo un valor en archivo: %s",rutaArmada);
				j++;

				*rutaArmada = 0;

			}
			log_info(logger,"****Finaliza el proceso respaldo de Informacion..****");
			log_info(logger,"**Se crearon %d archivo/s**",j);

			pthread_mutex_unlock(&lock_dump);
			free(rutaArmada);
		}

	}

return EXIT_SUCCESS;
}


int main (int argc, char * argv[]) {


	pthread_mutex_init(&lock_dump, NULL);

	if(argc < 2) {
		printf("Parametros invalidos, ingrese el archivo de config.\n");
	}


	t_config * config_file = config_create(argv[1]);

	int intervaloDump = atoi(config_get_string_value(config_file, "Intervalo_de_dump"));
	char* puntoMontaje = config_get_string_value(config_file, "Montaje");
	char* algoritmo = config_get_string_value(config_file, "Algoritmo");
	int nombre = config_get_int_value(config_file, "Nombre");

	valores_iniciales init;
		char identificador = 0;
		char * clave= 0;
		char * valor= 0;
		char * ruta= 0;
		int size_clave = 0, size_valor = 0,size_ruta =  0;



		//size clave + 1 de "/"
		size_ruta = (strlen(puntoMontaje));

		configure_logger();


		int server = create_client(config_get_string_value(config_file, "ip_coordinador"), config_get_string_value(config_file, "puerto_coordinador"));

		send(server, &nombre, sizeof(nombre), 0);

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





		log_info(logger,"Estado server %d \n", server);

		// creo el storage, la tabla y el registro de entradas
		char* storage = (char*)malloc(init.cantidad_entradas*init.tamanioEntrada);

		log_info(logger, "Cree el storage tamaño: %d",init.cantidad_entradas*init.tamanioEntrada);

		*storage = 0;

		t_list* tabla = list_create();

		log_info(logger, "Cree la tabla");

//		if(!strcmp((const char*)algoritmo,"LRU")){
			t_list* registro = crear_registro(init.cantidad_entradas);

			log_info(logger, "Cree registro de entradas");
//		}


		//creo thread dump

		log_info(logger, "Creo el thread Dump");

		pthread_t threadDumpeador;

		parametros_dump* parametros = malloc(sizeof(parametros_dump));

		if(parametros == NULL){

			log_error(logger,"No se ha podido alocar memoria para parametros del dump");
			return EXIT_FAILURE;
		}
		parametros->puntoDeMontaje = puntoMontaje;
		parametros->tabla = tabla;
		parametros->intervaloDeDump = intervaloDump;

		log_info(logger, "Cargue los parametros");

		char* posicionDeLectura = storage;

		char* posicionFinDeMemoria = (storage+(init.cantidad_entradas*init.tamanioEntrada));


		if( pthread_create(&threadDumpeador, NULL, (void *)respaldar_informacion_thread,parametros)) {
			log_error(logger, "Error creating thread Dump");
		}



		//-----------------------------------------------------------------------------------------------------------

		/*

		int cantidad_claves;
		t_dictionary* tablaDeRequeridas = dictionary_create();
		recv(server,&identificador,1,0);
		if(identificador == 200) {
			recv(server, &cantidad_claves, 4, 0);
			for(int i=0; i < cantidad_claves; i++) {

				recv(server, &size_clave, 4, 0);
				clave = (char *)malloc (size_clave + 1);
				recv(server, clave, size_clave, 0);
				clave[size_clave] = '\0';
				dictionary_put(tablaDeRequeridas,clave,clave);
			}

			//para probar
			//dictionary_put(tablaDeRequeridas,"futbol:ansaldi","futbol:ansaldi");
			//dictionary_put(tablaDeRequeridas,"futbol:messi","futbol:messi");

			levantar_archivos_a_memoria(&storage,init.tamanioEntrada,&tabla,tablaDeRequeridas,posicionDeLectura,posicionFinDeMemoria,puntoMontaje);
		}
*/


		//-----------------------------------------------------------------------------------------------------------





		while(1){

			/******************************************************************************************************************/









				//recibo operacion

				recv(server, &identificador, 1, 0);

				log_info(logger, "Identificador %d", identificador);


				//recibo clave
				int rc = recv(server, &size_clave, 4, 0);

				log_info(logger, "size_clave %d %d", size_clave, rc);

				clave = (char *)malloc(size_clave + 1);

				if(clave == NULL){
					log_error(logger,"No se pudo alocar memoria para la clave");
					return EXIT_FAILURE;
				}

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


					//OJO el campo claveValor.clave es char[40] tamaño fijado de la clave, en el tp  lo pide


					log_info(logger, "Asigne tamanioClave %d" , size_clave);

					strcpy(claveValor.clave,clave);


					log_info(logger, "Asigne clave: %s",clave);



					log_info(logger, "Asigne las estructuras");

					recv(server, &size_valor, 4, 0);
					valor = malloc(size_valor + 1);
					recv(server, valor, size_valor, 0);
					valor[size_valor] = '\0';
					log_info(logger, "Valor %s", valor);

					claveValor.valor = valor;

					log_info(logger, "Asigne valor: %s",valor);


					log_info(logger, "Valores iniciales %s, %s, %d", clave, valor, init.retardo);

					log_info(logger, "Recibi clave y valor");

					pthread_mutex_lock(&lock_dump);
					SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);
					//SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
					pthread_mutex_unlock(&lock_dump);

					log_info(logger,"El valor del storage es: %s",storage);

					free(valor);

				} else if(identificador==23) {//store


					ruta = malloc(size_ruta);

					if(ruta == NULL){
						log_error(logger,"No se pudo alocar memoria para la ruta");
						return EXIT_FAILURE;
					}

					strcpy(ruta, puntoMontaje);

					pthread_mutex_lock(&lock_dump);
					STORE(&tabla,&registro,storage,clave,ruta,init.tamanioEntrada);
					pthread_mutex_unlock(&lock_dump);

					free(ruta);
				}
				usleep(init.retardo * 1000);
				identificador = 1;
				send(server, &identificador, 1, 0);
				log_info(logger, "Le informe al coordinador que termine status:%d", identificador);
				free(clave);
				log_info(logger, "Ya termine de ejecutar esa instruccion");
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






