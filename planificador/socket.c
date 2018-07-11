/*
 * socketServer.c
 *
 *  Created on: Apr 18, 2018
 *      Author: fmocchetti
 */

#include "socket.h"

void configure_logger() {
  logger = log_create("coordinador.log", "coordinador", true, LOG_LEVEL_INFO);
}


void create_server(int max_connections, int timeout, int server_type, int port) {
  int    rc, on = 1;
  int    listen_sd = -1;
  struct sockaddr_in addr;
  struct pollfd fds[33];

  listen_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sd < 0) {
	log_error(logger, "socket() failed");
    exit(-1);
  }

  rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
  if (rc < 0) {
	log_error(logger, "setsockopt() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  if(!server_type) {
	  rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	  if (rc < 0) {
		log_error(logger,"ioctl() failed");
		shutdown(listen_sd, SHUT_RDWR);
		exit(-1);
	  }
  }


  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);
  rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
  if (rc < 0) {
	log_error(logger,"bind() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  rc = listen(listen_sd, max_connections);
  if (rc < 0) {
	log_error(logger,"listen() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  memset(fds, 0 , sizeof(fds));
  fds[0].fd = listen_sd;
  fds[0].events = POLLIN;

  //funcion que decide como se va a implementar el servicio de listen, si por hilos o por poll
  if(!server_type) {
	  listen_on_poll(fds, max_connections, timeout, listen_sd);
  } else {
	  thread_on_connection(listen_sd);
  }
}

void thread_on_connection(int listen_sd) {
	int new_sd = -1;					// Escuchar sobre socket_client, nuevas conexiones sobre newConnection
	pthread_t th_receiptMessage;		// hilo para crear receptor de mensajes
	char * buffer;
	unsigned char message = 0;
    thread_handle_struct * connection_arguments = malloc(sizeof(thread_handle_struct));
    int    rc, on = 1;
    int    n = 1;
    int    end_server = FALSE, compress_array = FALSE;
    int    close_conn;
    alpha = atoi(config_get_string_value(config_file, "alpha_planificacion"));
    estimacion_inicial = atoi(config_get_string_value(config_file, "estimacion_inicial"));

	while(1) {
		buffer = (char *) malloc(sizeof(char));
        log_info(logger,"Waiting new connection...");
		new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0) {
          if (errno != EWOULDBLOCK) {
            log_error(logger,"accept() failed");
          }
          break;
        }

        log_info(logger,"accept() ok");
        message = IDENTIFY;

        printf("%d envie x bytes \n", (int) sizeof(message));

        send(new_sd , &message, sizeof(message), 0);

        log_info(logger,"  Waiting for the client to identify\n");
        //sleep(10);
        rc = recv(new_sd, buffer, sizeof(buffer), 0);
        if (rc < 0) {
           if (errno != EWOULDBLOCK) {
             log_error(logger, "  recv() failed");
           }
        }

        log_info(logger," The client is an: %d", *buffer);
        //connection_arguments->socket = new_sd;
        //connection_arguments->identidad = *buffer;

		//-----------	Aca tendria que meter al cliente en la lista que corresponda ---------------------
		unsigned char id_mensaje_esi;
		ESI *esi= (ESI*) malloc(sizeof(ESI));
		ESI *esi2= (ESI*) malloc(sizeof(ESI));
		//Recibo ID del mensaje de la ESI
		rc = recv(new_sd, &id_mensaje_esi, 1,0);
		esi->socket_esi = new_sd;

		printf("El socket de la ESI es %d\n",esi->socket_esi);
		printf("El mensaje es %u\n",id_mensaje_esi);

		//Chequeo si el mensaje recibido de la ESI es el correcto
		if(id_mensaje_esi != 18){
			 _exit_with_error(socket, "id de mensaje incorrecto", NULL);
		}


		//Si lo es, sigo recibiendo

		rc = recv(new_sd, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas),0);
		if (rc <= 0) {
			_exit_with_error(new_sd, "El socket murio", NULL);
		}
		// Le indico a la nueva ESI el ID que le corresponde

		esi->id_ESI = n;
		send(new_sd, &esi->id_ESI, sizeof(esi->id_ESI), 0);
		//printf("el nuevo ID de la esi fue mandado\n");
		n++;

		//esi->rafaga = calculoProxRafaga((float)alpha,(float)estimacion_inicial,(float)esi->cantidadDeLineas);
		esi->rafaga = 0;
		esi->lineas_ejecutadas = 0;
		esi->estimacion_rafaga = (float)estimacion_inicial;
		esi->espera = 0;
		printf("!!!!!!!!!!!!!!!!!!!!!estimacion rafaga de %f!!!!!!!!!!!!!!!!!!\n",esi->estimacion_rafaga);
		printf("!!!!!!!!!!!!!!!!!!!!!rafaga de %f!!!!!!!!!!!!!!!!!!\n",esi->rafaga);

		esi2->socket_esi = esi->socket_esi;
		esi2->id_ESI = esi->id_ESI;
		printf("id esi %d\n",esi2->id_ESI);
		esi2->cantidadDeLineas = esi->cantidadDeLineas;
		esi2->rafaga = esi->rafaga;
		esi2->lineas_ejecutadas = esi->lineas_ejecutadas;
		esi2->estimacion_rafaga = esi->estimacion_rafaga;
		esi2->espera = esi->espera;
		//free(esi);

		//agrego el nuevo proceso a la cola de listos
		printf("Agregando a la lista de ready\n");
		list_add(listos, (ESI*)esi2);
		printf("Agregado!\n");
		//esi2 = (ESI*) list_get(listos, 0);
		//printf("Cantidad de lineas por ejecutar: %d\n", esi->cantidadDeLineas);

		//semaforo para indicar que hay un nuevo proceso listo para su ejecucion
		sem_post(&new_process);
		replanificar = 1;

        free(buffer);
	}
}


void listen_on_poll(struct pollfd * fds, int max_connections, int timeout, int listen_sd) {
  int rc;
  int nfds = 1, current_size = 0, i;
  int end_server = FALSE;
  int new_sd = -1;
  int close_conn;
  char buffer[80];

  do {
    printf("Waiting on poll()...\n");
    rc = poll(fds, max_connections, timeout);

    if (rc < 0) {
      log_error(logger,"poll() failed");
      break;
    }

    if (rc == 0) {
      log_info(logger,"  poll() timed out.  End program.\n");
      break;
    }

    current_size = nfds;
    for (i = 0; i < current_size; i++) {
      if(fds[i].revents == 0) continue;

      if(fds[i].revents != POLLIN) {
    	log_error(logger,"  Error! revents = %d\n", fds[i].revents);
        end_server = TRUE;
        break;
      }

      if (fds[i].fd == listen_sd) {
        log_info(logger,"Listening socket is readable\n");
        do {
          new_sd = accept(listen_sd, NULL, NULL);
          if (new_sd < 0) {
            if (errno != EWOULDBLOCK) {
              log_error(logger,"accept() failed");
              end_server = TRUE;
            }
            break;
          }


          log_info(logger,"  New incoming connection - %d\n", new_sd);

          fds[nfds].fd = new_sd;
          fds[nfds].events = POLLIN;
          nfds++;

          send(new_sd , "identify", strlen("identify"), 0);

          log_info(logger,"  Waiting for the client to identify\n");

          rc = recv(new_sd, buffer, sizeof(buffer), 0);
          if (rc < 0) {
             if (errno != EWOULDBLOCK) {
               log_error(logger, "  recv() failed");
               close_conn = TRUE;
             }
          }

          log_info(logger," The client is an: %s",buffer);


          //Crear thread con esa conexion

        } while (new_sd != -1);
      }
    }
  } while (end_server == FALSE); /* End of serving running.    */


  for (i = 0; i < nfds; i++)
  {
    if(fds[i].fd >= 0)
      shutdown(fds[i].fd, SHUT_RDWR);
  }
}

void connection_thread(void * connection_arguments) {
	/*thread_handle_struct args = *(thread_handle_struct *) connection_arguments;
	int socket_local = args.socket, identidad_local = args.identidad;
	log_info(logger, "New thread created");

	if(identidad_local == IDENTIFY_INSTANCIA) {
		_instancia(socket_local);
	} else if(identidad_local == IDENTIFY_ESI) {
		_esi(socket_local);
	} else if(identidad_local == PLANIFICADOR) {
		_planificador(socket_local);
	}*/

	return;
}

int connect_to_server(char * ip, char * port) {
	struct addrinfo hints;
	struct addrinfo * server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

	log_info(logger, "Trying to connect to: %s : %s \n", ip, port);

	getaddrinfo(ip, port, &hints, &server_info);  // Carga en server_info los datos de la conexion

	int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(server_socket < 0) {
		log_error(logger, "Failed creating socket %d\n", errno);
		_exit_with_error(server_socket, "No me pude conectar al servidor", NULL);
	}

	int res = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);  // No lo necesitamos mas

	if (res < 0) {
		_exit_with_error(server_socket, "No me pude conectar al servidor", NULL);
	}

	log_info(logger, "Conectado!");

	return server_socket;
}

void wait_hello(int socket) {
	unsigned char buffer = 0;

	log_info(logger, "Waiting handshake");
    int result_recv = recv(socket, &buffer, 1, MSG_WAITALL); //MSG_WAITALL

	//printf("Recibi %d\n",buffer);

    if(result_recv <= 0) {
      _exit_with_error(socket, "No se pudo recibir hola", buffer);
    }

    if (buffer != IDENTIFY) {
      _exit_with_error(socket, "No es el mensaje que se esperaba", buffer);
    }

    buffer = PLANIFICADOR;
	send(socket, &buffer, sizeof(buffer), 0);
}

void send_message(int socket){
	char mensaje[1000];

	char * server_reply = NULL;

	while (1) {

		printf("escribi algo!\n");

  		scanf("%s", mensaje);

  		printf("valor de mensaje %s!\n", mensaje);

  		send(socket, mensaje, strlen(mensaje), 0);

  		log_info(logger,"envie %s\n", mensaje);

  		server_reply = (char *)calloc(sizeof(char), 1000);

		//Receive a reply from the server
		if(recv(socket , server_reply , 200 , 0) < 0)
		{
			puts("recv failed");

			break;
		}

  		log_info(logger,"recibi %s\n", server_reply);

  		free(server_reply);
  	}
}

// esto no va aca
int create_client(char * ip, char * port){
	 int socket_server = 0;

	 socket_server = connect_to_server(ip, port);

	 wait_hello(socket_server);

	 return socket_server;
}
