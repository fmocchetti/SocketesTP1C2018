#include "instancia.h"

int respaldar_informacion_thread(parametros_dump* parametros){

	while(1){

		sleep(parametros->intervaloDeDump);
		pthread_mutex_lock(&lock_dump);


		log_info(logger,"DUMP: ****Comienza el proceso respaldo de Informacion en: %s ****",parametros->puntoDeMontaje);

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
			for(int i = 0 ; i < (tamanioTabla-1); i++){

				unDato = list_get( parametros->tabla, i );

				strcpy(rutaArmada,parametros->puntoDeMontaje);
				strcat(rutaArmada,(const char*)unDato->clave);

				int fd = open(rutaArmada, O_RDWR | O_CREAT, (mode_t)0600);
				if(fd < 0){
					log_error(logger,"DUMP: Error al abrir la ruta, puede que el directorio %s no exista",parametros->puntoDeMontaje);
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
			log_info(logger,"DUMP: ****Finaliza el proceso respaldo de Informacion..****");
			log_info(logger,"DUMP: **Se crearon %d archivo/s**",j);

			pthread_mutex_unlock(&lock_dump);
			free(rutaArmada);
		}

	}

return EXIT_SUCCESS;
}


int main (int argc, char * argv[]) {

//#if 0
	pthread_mutex_init(&lock_dump, NULL);

	if(argc < 2) {
		printf("Parametros invalidos, ingrese el archivo de config.\n");
	}


	t_config * config_file = config_create(argv[1]);

	int intervaloDump = config_get_int_value(config_file, "Intervalo_de_dump");
	char* puntoMontaje = config_get_string_value(config_file, "Montaje");
	char* algoritmo = config_get_string_value(config_file, "Algoritmo");
	int nombre = config_get_int_value(config_file, "Nombre");

	valores_iniciales init;
		unsigned char identificador = 0;
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
		log_info(logger,"INSTANCIA %d: Recibi %d \n",nombre ,identificador);
		recv(server, &messageLength, 4, 0);
		log_info(logger,"INSTANCIA %d: Recibi %d \n", nombre ,messageLength);
		void * message = malloc(messageLength);
		int rec =recv(server, message, messageLength, 0);
		log_info(logger,"INSTANCIA %d: Recibi %d bytes",nombre , rec);

		memcpy(&init.cantidad_entradas, message, 4);
		log_info(logger,"INSTANCIA %d: Cantidad entradas: %d",nombre , init.cantidad_entradas);

		memcpy(&init.tamanioEntrada, message+4, 4);
		log_info(logger,"INSTANCIA %d: Tamanio entradas: %d",nombre , init.tamanioEntrada);

		memcpy(&init.retardo, message+8, 4);
		log_info(logger,"INSTANCIA %d: Retardo: %d",nombre , init.retardo);

		free(message);

		log_info(logger, "INSTANCIA %d: Valores iniciales %d, %d, %d",nombre , init.cantidad_entradas, init.tamanioEntrada, init.retardo);

		log_info(logger, "INSTANCIA %d: Recibi la inicializacion",nombre);





		log_info(logger,"INSTANCIA %d: Estado server %d \n",nombre , server);

		// creo el storage, la tabla y el registro de entradas
		char* storage = (char*)malloc(init.cantidad_entradas*init.tamanioEntrada);

		log_info(logger, "INSTANCIA %d: Cree el storage tamaño: %d",nombre ,init.cantidad_entradas*init.tamanioEntrada);

		*storage = 0;

		t_list* tabla = list_create();

		log_info(logger, "INSTANCIA %d: Cree la tabla",nombre);


		t_list* registro = crear_registro(init.cantidad_entradas);

		log_info(logger, "INSTANCIA %d: Cree registro de entradas",nombre);



		//creo thread dump

		log_info(logger, "INSTANCIA %d: Creo el thread Dump",nombre);

		pthread_t threadDumpeador;

		parametros_dump* parametros = malloc(sizeof(parametros_dump));

		if(parametros == NULL){

			log_error(logger,"INSTANCIA %d: No se ha podido alocar memoria para parametros del dump",nombre);
			return EXIT_FAILURE;
		}
		parametros->puntoDeMontaje = puntoMontaje;
		parametros->tabla = tabla;
		parametros->intervaloDeDump = intervaloDump;

		log_info(logger, "INSTANCIA %d: Cargue los parametros",nombre);

		char* posicionDeLectura = storage;

		char* posicionFinDeMemoria = (storage+(init.cantidad_entradas*init.tamanioEntrada));


		if( pthread_create(&threadDumpeador, NULL, (void *)respaldar_informacion_thread,parametros)) {
			log_error(logger, "INSTANCIA %d: Error creating thread Dump",nombre);
		}



		//-----------------------------------------------------------------------------------------------------------



		int cantidad_claves,seUso = 0;
		t_dictionary* tablaDeRequeridas = dictionary_create();
		recv(server,&identificador,1,0);
		log_info(logger, "INSTANCIA %d: Identificador %d",nombre , identificador);
		if(identificador == 57) {
			recv(server, &cantidad_claves, 4, 0);
			for(int i=0; i < cantidad_claves; i++) {
				recv(server, &size_clave, 4, 0);
				clave = (char *)malloc (size_clave + 1);
				recv(server, clave, size_clave, 0);
				clave[size_clave] = '\0';
				log_info(logger, "INSTANCIA %d: Levantando clave %s",nombre , clave);
				dictionary_put(tablaDeRequeridas,clave,clave);
			}
			//para probar
			//dictionary_put(tablaDeRequeridas,"futbol:ansaldi","futbol:ansaldi");
			//dictionary_put(tablaDeRequeridas,"futbol:messi","futbol:messi");

			levantar_archivos_a_memoria(&storage,init.tamanioEntrada,&tabla,tablaDeRequeridas,posicionDeLectura,posicionFinDeMemoria,puntoMontaje);

			dictionary_destroy_and_destroy_elements(tablaDeRequeridas,freeDeClaves);

			seUso=1;
		}

		if(seUso==0){

			dictionary_destroy(tablaDeRequeridas);

		}


		//-----------------------------------------------------------------------------------------------------------





		while(1){

			/******************************************************************************************************************/









				//recibo operacion

				recv(server, &identificador, 1, 0);

				log_info(logger, "INSTANCIA %d: Identificador %d",nombre , identificador);


				if(identificador == 21) {
					identificador = 42;
					send(server, &identificador, 1, 0);
					log_info(logger, "INSTANCIA %d: Keep alive coordinador",nombre);
					continue;
				}


				//recibo clave
				int rc = recv(server, &size_clave, 4, 0);

				log_info(logger, "INSTANCIA %d: size_clave %d %d",nombre , size_clave, rc);

				clave = (char *)malloc(size_clave + 1);

				if(clave == NULL){
					log_error(logger,"INSTANCIA %d: No se pudo alocar memoria para la clave",nombre);
					return EXIT_FAILURE;
				}

				rc = recv(server, clave, size_clave, 0);
				log_info(logger, "INSTANCIA %d: Clave %d",nombre , rc);

				clave[size_clave] = '\0';

				log_info(logger, "INSTANCIA %d: Clave %s",nombre , clave);

				log_info(logger, "INSTANCIA %d: Identificador %d",nombre , identificador);


				//TODO: Si es un get aca no tiene que recibir nada
				if(identificador == 22){

					//lleno estructura que le paso al set_cicular
					struct ClaveValor claveValor;

					claveValor.tamanioEntrada = init.tamanioEntrada;
					claveValor.cantidadEntradas = init.cantidad_entradas;


					//OJO el campo claveValor.clave es char[40] tamaño fijado de la clave, en el tp  lo pide


					log_info(logger, "INSTANCIA %d: Asigne tamanioClave %d" ,nombre , size_clave);

					strcpy(claveValor.clave,clave);


					log_info(logger, "INSTANCIA %d: Asigne clave: %s",nombre ,clave);



					log_info(logger, "INSTANCIA %d: Asigne las estructuras",nombre);

					recv(server, &size_valor, 4, 0);
					valor = malloc(size_valor + 1);
					recv(server, valor, size_valor, 0);
					valor[size_valor] = '\0';
					log_info(logger, "INSTANCIA %d: Valor %s",nombre , valor);

					claveValor.valor = valor;

					log_info(logger, "INSTANCIA %d: Asigne valor: %s",nombre ,valor);


					log_info(logger, "INSTANCIA %d: Valores iniciales %s, %s, %d",nombre , clave, valor, init.retardo);

					log_info(logger, "INSTANCIA %d: Recibi clave y valor",nombre);

					if(!strcmp((const char*)algoritmo,"CIRC")){
						pthread_mutex_lock(&lock_dump);
						SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);
						pthread_mutex_unlock(&lock_dump);

					}
					else if(!strcmp((const char*)algoritmo,"LRU")){
						pthread_mutex_lock(&lock_dump);
						SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
						pthread_mutex_unlock(&lock_dump);
					}
					else if(!strcmp((const char*)algoritmo,"BSU")){
						pthread_mutex_lock(&lock_dump);
						SET_BSU(&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
						pthread_mutex_unlock(&lock_dump);
					}

					free(valor);

				} else if(identificador==23) {//store


					ruta = malloc(size_ruta);

					if(ruta == NULL){
						log_error(logger,"INSTANCIA %d: No se pudo alocar memoria para la ruta",nombre);
						return EXIT_FAILURE;
					}

					strcpy(ruta, puntoMontaje);

					pthread_mutex_lock(&lock_dump);
					STORE(&tabla,&registro,storage,clave,ruta,init.tamanioEntrada);
					pthread_mutex_unlock(&lock_dump);

					free(ruta);
				}
				else if(identificador==200){ //compactar

					pthread_mutex_lock(&lock_dump);

					compactar(&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,init.tamanioEntrada);

					pthread_mutex_unlock(&lock_dump);

				}
				else if(identificador == 300){// status consola

					log_info(logger, "INSTANCIA %d: Me pidieron el valor de una clave, se buscara si existe",nombre);

					unsigned char id;

					struct Dato* unDato = buscar(tabla,clave);
					if(unDato == NULL){

						log_info(logger, "INSTANCIA %d: La clave no se encontro en la tabla, le aviso al coordinador que no existe",nombre);
						int id = 5000; //cambiar numeroo <------------------

						//no se encontro la clave
						send(server, &id, 1, 0);

					}
					else{


						log_info(logger, "INSTANCIA %d: Se encontro la clave, se le enviara el valor al cordiador",nombre);

						//cmabiar numeroooooooooooo id <------------------
						id = 6000;

						int tamanioBuffer = 1+4+unDato->cantidadDeBytes;
						log_info(logger, "INSTANCIA %d: ¡Se encontro la clave!, se le enviaran %d bytes al coordinador",nombre,tamanioBuffer);
						char* buffer = (char*) malloc (tamanioBuffer);
						memcpy(buffer,&id,1);
						memcpy(buffer+1,&unDato->cantidadDeBytes,4);
						memcpy(buffer+5,unDato->posicionMemoria,unDato->cantidadDeBytes);
						send(server, &buffer, tamanioBuffer, 0);

						free(buffer);

					}



				}
				usleep(init.retardo * 1000);
				identificador = 1;
				send(server, &identificador, 1, 0);
				log_info(logger, "INSTANCIA %d: Le informe al coordinador que termine status %d",nombre , identificador);
				free(clave);
				log_info(logger, "INSTANCIA %d: Ya termine de ejecutar esa instruccion",nombre);
		}
		//libero memoria
	    //free(storage);//free(parametros);
	    //liberar_recursos(&tabla);



  /*******************************************************************************************************************/


//#endif


//-----------ACA LOS TESTS-------------------------------------------------------------------------------------

	//correrTestsCircular();

	//correrTestsLRU();

    //correrTestsDump();

	//correrTestsSTORE();

	return EXIT_SUCCESS;
}






