/*
 * socketServer.c
 *
 *  Created on: Apr 18, 2018
 *      Author: fmocchetti
 */

#include "socket.h"

t_log * logger;

void configure_logger() {
  logger = log_create("planificador.log", "Planificador", true, LOG_LEVEL_INFO);
}


void create_server(int max_connections, int timeout) {
  int    rc, on = 1;
  int 	 n = 1;
  int    ejemplo = 0;
  int    listen_sd = -1, new_sd = -1;
  int    end_server = FALSE, compress_array = FALSE;
  int    close_conn;
  char   buffer[80];
  struct sockaddr_in addr;
  struct pollfd fds[33];
  int    nfds = 1, current_size = 0, i, j;
  int 	 config_plani = 1;

  ESI *esi= (ESI*) malloc(sizeof(ESI));
  //ESI *esi;
  ESI *esi2 = (ESI*) malloc(sizeof(ESI));



  /*************************************************************/
  /* Create an AF_INET stream socket to receive incoming       */
  /* connections on                                            */
  /*************************************************************/
  listen_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sd < 0)
  {
	log_error(logger, "socket() failed");
    exit(-1);
  }

  /*************************************************************/
  /* Allow socket descriptor to be reuseable                   */
  /*************************************************************/
  rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
  if (rc < 0)
  {
	log_error(logger, "setsockopt() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  /*************************************************************/
  /* Set socket to be nonblocking. All of the sockets for    */
  /* the incoming connections will also be nonblocking since  */
  /* they will inherit that state from the listening socket.   */
  /*************************************************************/
  rc = ioctl(listen_sd, FIONBIO, (char *)&on);
  if (rc < 0)
  {
	log_error(logger,"ioctl() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  /*************************************************************/
  /* Bind the socket                                           */
  /*************************************************************/
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(SERVER_PORT);
  rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
  if (rc < 0)
  {
	log_error(logger,"bind() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  /*************************************************************/
  /* Set the listen back log                                   */
  /*************************************************************/
  rc = listen(listen_sd, max_connections);
  if (rc < 0)
  {
	log_error(logger,"listen() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(-1);
  }

  /*************************************************************/
  /* Initialize the pollfd structure                           */
  /*************************************************************/
  memset(fds, 0 , sizeof(fds));

  /*************************************************************/
  /* Set up the initial listening socket                        */
  /*************************************************************/
  fds[0].fd = listen_sd;
  fds[0].events = POLLIN;
  /*************************************************************/
  /* Initialize the timeout to 3 minutes. If no               */
  /* activity after 3 minutes this program will end.           */
  /* timeout value is based on milliseconds.                   */
  /*************************************************************/
  timeout = (3 * 60 * 1000);

  /*************************************************************/
  /* Loop waiting for incoming connects or for incoming data   */
  /* on any of the connected sockets.                          */
  /*************************************************************/
  do
  {
    /***********************************************************/
    /* Call poll() and wait 3 minutes for it to complete.      */
    /***********************************************************/
	  printf("mensaje de esi>%d\n", esi->id_mensaje);
	  printf("%d\n",sizeof(*esi));
	 printf("Waiting on poll()...\n");
    rc = poll(fds, max_connections, timeout);

    /***********************************************************/
    /* Check to see if the poll call failed.                   */
    /***********************************************************/
    if (rc < 0)
    {
      log_error(logger,"poll() failed");
      break;
    }

    /***********************************************************/
    /* Check to see if the 3 minute time out expired.          */
    /***********************************************************/
    if (rc == 0)
    {
      log_info(logger,"  poll() timed out.  End program.\n");
      break;
    }


    /***********************************************************/
    /* One or more descriptors are readable.  Need to          */
    /* determine which ones they are.                          */
    /***********************************************************/
    current_size = nfds;
    for (i = 0; i < current_size; i++)
    {
      /*********************************************************/
      /* Loop through to find the descriptors that returned    */
      /* POLLIN and determine whether it's the listening       */
      /* or the active connection.                             */
      /*********************************************************/
      if(fds[i].revents == 0)
        continue;

      /*********************************************************/
      /* If revents is not POLLIN, it's an unexpected result,  */
      /* log and end the server.                               */
      /*********************************************************/
      if(fds[i].revents != POLLIN)
      {
    	log_error(logger,"  Error! revents = %d\n", fds[i].revents);
        end_server = TRUE;
        break;

      }
      if (fds[i].fd == listen_sd)
      {
        /*******************************************************/
        /* Listening descriptor is readable.                   */
        /*******************************************************/
        log_info(logger,"Listening socket is readable\n");

        /*******************************************************/
        /* Accept all incoming connections that are            */
        /* queued up on the listening socket before we         */
        /* loop back and call poll again.                      */
        /*******************************************************/
        do
        {
          /*****************************************************/
          /* Accept each incoming connection. If              */
          /* accept fails with EWOULDBLOCK, then we            */
          /* have accepted all of them. Any other             */
          /* failure on accept will cause us to end the        */
          /* server.                                           */
          /*****************************************************/
          new_sd = accept(listen_sd, NULL, NULL);
          if (new_sd < 0)
          {
            if (errno != EWOULDBLOCK)
            {
              log_error(logger,"accept() failed");
              end_server = TRUE;
            }
            break;
          }

          /*****************************************************/
          /* Add the new incoming connection to the            */
          /* pollfd structure                                  */
          /*****************************************************/

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


           //-----------	Aca tendria que meter al cliente en la lista que corresponda ---------------------

           	rc = recv(new_sd, &esi->id_mensaje, sizeof(esi->id_mensaje),0);

           	if(rc < 0) {
           		if (errno != EWOULDBLOCK) {
               	   	   log_error(logger, "  recv() failed");
               	   	   close_conn = TRUE;
               	   	   }
           	}

           	esi->socket_esi = new_sd;

           	printf("El socket de la ESI es %d\n",esi->socket_esi);

           	//Chequeo si el mensaje recibido de la ESI es el correcto
           	           	if(esi->id_mensaje != 18){
           	           		log_error(logger, "id de mensaje incorrecto");
           	           		close_conn = TRUE;
           	           	    }

           	//Si lo es, sigo recibiendo
           	rc = recv(new_sd, &esi->id_ESI, sizeof(esi->id_ESI),0);
           	if(rc < 0) {
           	           		if (errno != EWOULDBLOCK) {
           	               	   	   log_error(logger, "  recv() failed");
           	               	   	   close_conn = TRUE;
           	               	   	   }
           	           	}
           	rc = recv(new_sd, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas),0);
           	if(rc < 0) {
           	           		if (errno != EWOULDBLOCK) {
           	               	   	   log_error(logger, "  recv() failed");
           	               	   	   close_conn = TRUE;
           	               	   	   }
           	           	}

           	// Le indico a la nueva ESI el ID que le corresponde

           	esi->id_ESI = n;
           	send(new_sd, &esi->id_ESI, sizeof(esi->id_ESI), 0);
           	//printf("%d\n", esi->id_ESI);
           	n++;

           	//agrego el nuevo proceso a la cola de listos

           	sem_wait(&mutex_listos);
            list_add(listos, (ESI*)esi);
            sem_post(&mutex_listos);
            printf("sabe\n");

            //hacer un signal hacerle saber al algoritmo que se encolo un nuevo proceso.
            sem_post(&new_process);

          /*****************************************************/
          /* Loop back up and accept another incoming          */
          /* connection                                        */
          /*****************************************************/
        } while (new_sd != -1);
      } else {

          /*********************************************************/
          /* This is not the listening socket, therefore an        */
          /* existing connection must be readable                  */
          /*********************************************************/

    	  log_info(logger, "  Descriptor %d is readable\n", fds[i].fd);
    	  close_conn = FALSE;


    	  int z = 0;
    	  for(z=0;z<sizeof(buffer);z++){
    		  buffer[z] = 0;
    	  }

          rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
          if (rc < 0) {
            if (errno != EWOULDBLOCK) {
              log_error(logger, "  recv() failed");
              close_conn = TRUE;
            }
          }

          if (rc == 0) {
        	log_info(logger, "  Connection closed\n");
            close_conn = TRUE;
          }

          log_info(logger, "  %d bytes received\n", rc);

          log_info(logger, " recibido: %s \n", buffer);

          /*****************************************************/
          /* Echo the data back to the client                  */
          /*****************************************************/
		  /*rc = send(fds[i].fd, buffer, sizeof(buffer), 0);
		  log_info(logger,"Enviado: %s", buffer);*/


          if (rc < 0)
          {
        	log_error(logger,"  send() failed" );
            close_conn = TRUE;
            //break;
          }

        if (close_conn)
        {
          shutdown(fds[i].fd,SHUT_RDWR);
          fds[i].fd = -1;
          compress_array = TRUE;
        }


      }  /* End of existing connection is readable             */
    } /* End of loop through pollable descriptors              */

    /***********************************************************/
    /* If the compress_array flag was turned on, we need       */
    /* to squeeze together the array and decrement the number  */
    /* of file descriptors. We do not need to move back the    */
    /* events and revents fields because the events will always*/
    /* be POLLIN in this case, and revents is output.          */
    /***********************************************************/
    if (compress_array)
    {
      compress_array = FALSE;
      for (i = 0; i < nfds; i++)
      {
        if (fds[i].fd == -1)
        {
          for(j = i; j < nfds; j++)
          {
            fds[j].fd = fds[j+1].fd;
          }
          nfds--;
        }
        if(!nfds) nfds = 1;
      }
    }

  } while (end_server == FALSE); /* End of serving running.    */

  /*************************************************************/
  /* Clean up all of the sockets that are open                  */
  /*************************************************************/
  for (i = 0; i < nfds; i++)
  {
    if(fds[i].fd >= 0)
      shutdown(fds[i].fd, SHUT_RDWR);
  }
}
