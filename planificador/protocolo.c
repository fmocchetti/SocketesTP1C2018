/*
 * protocolo.c
 *
 *  Created on: 11 may. 2018
 *      Author: utnso
 */

#include "protocolo.h"


t_log * logger;

void * wait_content(int socket) {
  /*
    13.   Ahora tenemos que recibir un contenido de tamaño variable
          Para eso, primero tenemos que confirmar que el id corresponde al de una
          respuesta de contenido variable (18) y despues junto con el id de operacion
          vamos a haber recibido el tamaño del contenido que sigue. Por lo que:
  */

  log_info(logger, "Esperando el encabezado del contenido(%ld bytes)", sizeof(ContentHeader));
  // 13.1. Reservamos el suficiente espacio para guardar un ContentHeader
  ContentHeader * header = (ContentHeader*) malloc(sizeof(ContentHeader));

  // 13.2. Recibamos el header en la estructura y chequiemos si el id es el correcto.
  //      No se olviden de validar los errores!

  if (recv(socket, header, sizeof(ContentHeader), 0) <= 0) {
    _exit_with_error(socket, "No se pudo recibir el encabezado del contenido", header);
  }

  if (header->id != 18) {
    _exit_with_error(socket, "Id incorrecto, deberia ser 18", header);
  }

  log_info(logger, "Esperando el contenido (%d bytes)", header->len);

  /*
      14.   Ahora, recibamos el contenido variable. Ya tenemos el tamaño,
            por lo que reecibirlo es lo mismo que veniamos haciendo:
      14.1. Reservamos memoria
      14.2. Recibimos el contenido en un buffer (si hubo error, fallamos, liberamos y salimos
  */

  void * buf = calloc(sizeof(char), header->len + 1);
  if (recv(socket, buf, header->len, MSG_WAITALL) <= 0) {
    free(buf);
    _exit_with_error(socket, "Error recibiendo el contenido", header);
  }

  /*
      15.   Finalmente, no te olvides de liberar la memoria que pedimos
            para el header y retornar el contenido recibido.
  */

  log_info(logger, "Contenido recibido '%s'", (char*) buf);
  free(header);
  return buf;
}


void _exit_with_error(int socket, char* error_msg, char * buffer) {
  if (buffer != NULL) {
    free(buffer);
  }
  log_error(logger, error_msg);
  close(socket);
  exit_gracefully(1);
}

void exit_gracefully(int return_nr) {
  /*
    20.   Siempre llamamos a esta funcion para cerrar el programa.
          Asi solo necesitamos destruir el logger y usar la llamada al
          sistema exit() para terminar la ejecucion
  */
  log_destroy(logger);
  exit(return_nr);
}
