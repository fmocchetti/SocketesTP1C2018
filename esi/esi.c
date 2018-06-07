#include "esi.h"
#include <pthread.h>
#include "socketClient.h"

void send_hello(int, ESI*);
void doUntilFinish(int, ESI*);

int main(){
	ESI* esi = (ESI*) malloc(sizeof(ESI));
	//int contestacion_esi;
	esi->id_ESI = 1;
	esi->cantidadDeLineas = 50;
	esi->claveAEjecutar;

	printf("esi mide %d\n",sizeof(ESI));

	configure_logger();

	int server = create_client("127.0.0.1","12345");
	//int coordinador = create_client("127.0.0.2","12345");

	//send_message(server);


	send_hello(server, esi);
	doUntilFinish(server, esi);
	//printf("%d\n",esi->cantidadDeLineas);
	//printf("%d\n", esi->id_ESI);

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
  unsigned char id_mensaje_plani = 18;
  int resultado = send(socket, &id_mensaje_plani, 1, 0);
  send(socket, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
  if (resultado <= 0) {
    /*
      12.1. Recuerden que al salir tenemos que cerrar el socket (ademas de loggear)!
    */
    _exit_with_error(socket, "No se pudo enviar el hola", NULL);
  }
  //recv(socket, &esi->id_ESI, sizeof(esi->id_ESI),0);
  //printf("%d\n", esi->id_ESI);
}

void doUntilFinish(int socket, ESI* esi){
	//recibo permiso de ejecucion
	printf("%d\n", esi->cantidadDeLineas);

	unsigned char permisoDeEjecucion = 0;
	unsigned char resultadoEjecucion = 2;

	while(esi->cantidadDeLineas > 0){
		printf("Haciendo de lo mio \n");
	recv(socket, &permisoDeEjecucion, 1,0);
	if(permisoDeEjecucion == 1){
		esi->cantidadDeLineas --;
	}
	else{
		printf("Error!\n");
	}

	send(socket, &resultadoEjecucion, 1, 0);
	//send(socket, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
	sleep(3);
}
	printf("Mi ID es %d\n",esi->id_ESI);
}
