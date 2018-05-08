/*
 * socketServer.c
 *
 *  Created on: Apr 18, 2018
 *      Author: fmocchetti
 */

#include "socket.h"

t_log * logger;

void configure_logger() {
  logger = log_create("coordinador.log", "coordinador", true, LOG_LEVEL_INFO);
}


void create_server(int max_connections, int timeout, int server_type, int port) {
  int    rc, on = 1;
  int    listen_sd = -1, new_sd = -1;
  int    end_server = FALSE, compress_array = FALSE;
  struct sockaddr_in addr;
  struct pollfd fds[33];
  int    nfds = 1, current_size = 0, i, j;

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

	printf("listen_sd: %d \n", listen_sd);
	while(1) {
		new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0) {
          if (errno != EWOULDBLOCK) {
            log_error(logger,"accept() failed");
          }
          break;
        }

        log_info(logger,"accept() ok");

		pthread_create(&th_receiptMessage, NULL, (void*) connection_thread, NULL);
		pthread_detach(th_receiptMessage);
	}
}

void connection_thread() {
	printf("Hola soy un thread");
	return;
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


/*void * listen_connection() {

	return 0;
}*/
