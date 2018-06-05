#include "esi.h"
#include <pthread.h>
#include "socketClient.h"

void send_hello(int, ESI*);


typedef struct {
	int cantidad_entradas;
	int size_key;
	int retardo;
} valores_iniciales;


int main(){
	char identificador = 0;
	valores_iniciales init;
	ESI* esi = (ESI*) malloc(sizeof(ESI));

	esi->id_mensaje = 18;
	esi->id_ESI = 1;
	esi->cantidadDeLineas = 10;

	configure_logger();

	int server = create_client("127.0.0.1","12346");

	log_info(logger, "Estado server %d \n", server);

	recv(server, &identificador, 1, 0);
	size_t messageLength;
	recv(server, &messageLength, 4, 0);
	void *message = malloc(messageLength);
	recv(server, message, messageLength, 0);

	memcpy(&init.cantidad_entradas, message, 4);
	memcpy(&init.size_key, message+4, 4);
	memcpy(&init.retardo, message+8, 4);

	log_info(logger, "Valores iniciales %d, %d, %d", init.cantidad_entradas, init.size_key, init.retardo);

	log_info(logger, "Recibi la inicializacion");


	send_message(server);

	/*for(int i = 0; i < 10; i++) {
		send_hello(server, esi);
		sleep(10);
	}*/


	exit_gracefully(0);

}

void send_hello(int socket, ESI* esi) {
  log_info(logger, "Enviando info actual de la ESI");
  /*
    10.   Ahora SI nos toca mandar el hola con los datos del alumno.
          Pero nos falta algo en nuestra estructura, el id_mensaje del protocolo.
          Segun definimos, el tipo de id para un mensaje de tamaño fijo con
          la informacion del alumno es el id 99
  */


  /*
    10.1. Como algo extra, podes probar enviando caracteres invalidos en el nombre
          o un id de otra operacion a ver que responde el servidor y como se
          comporta nuestro cliente.
  */

  // alumno.id = 33;
  // alumno.nombre[2] = -4;

  /*
    12.   Finalmente, enviemos la estructura por el socket!
          Recuerden que nuestra estructura esta definida como __attribute__((packed))
          por lo que no tiene padding y la podemos mandar directamente sin necesidad
          de un buffer y usando el tamaño del tipo Alumno!
  */
  int resultado = send(socket, esi, sizeof(ESI), 0);
  if (resultado <= 0) {
    /*
      12.1. Recuerden que al salir tenemos que cerrar el socket (ademas de loggear)!
    */
    _exit_with_error(socket, "No se pudo enviar el hola", NULL);
  }
}
