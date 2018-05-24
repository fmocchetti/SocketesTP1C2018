#include "esi.h"
#include <pthread.h>
#include "socketClient.h"

void send_hello(int, ESI*);
void doUntilFinish(int, ESI*);

int main(){
	ESI* esi = (ESI*) malloc(sizeof(ESI));
	//int contestacion_esi;
	esi->id_mensaje = 18;
	esi->id_ESI = 1;
	esi->cantidadDeLineas = 50;

	printf("esi mide %d\n",sizeof(ESI));

	configure_logger();

	int server = create_client("127.0.0.1","12345");

	//send_message(server);


	send_hello(server, esi);
	doUntilFinish(server, esi);
	getchar();
	//printf("%d\n",esi->cantidadDeLineas);
	//printf("%d\n", esi->id_ESI);

	/*while(esi->cantidadDeLineas > 0){
		recv(socket, &contestacion_esi, sizeof(contestacion_esi),0);
		if(contestacion_esi == 1){
			(esi->cantidadDeLineas) --;
			printf("La nueva cantidad de lineas es %d\n",esi->cantidadDeLineas);
		}
	}
	*/

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
  int resultado = send(socket, &esi->id_mensaje, sizeof(esi->id_mensaje), 0);
  send(socket, &esi->id_ESI, sizeof(esi->id_ESI), 0);
  send(socket, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
  if (resultado <= 0) {
    /*
      12.1. Recuerden que al salir tenemos que cerrar el socket (ademas de loggear)!
    */
    _exit_with_error(socket, "No se pudo enviar el hola", NULL);
  }
  recv(socket, &esi->id_ESI, sizeof(esi->id_ESI),0);
  //printf("%d\n", esi->id_ESI);
}

void doUntilFinish(int socket, ESI* esi){
	//recibo permiso de ejecucion
	printf("%d\n", esi->cantidadDeLineas);

	int permisoDeEjecucion = 0;
	int resultadoEjecucion = 1; // si es 1 es correcto, si es 2 no

	while(esi->cantidadDeLineas > 0){
		printf("Haciendo de lo mio \n");
	recv(socket, &permisoDeEjecucion, sizeof(permisoDeEjecucion),0);
	if(permisoDeEjecucion == 1){
		esi->cantidadDeLineas --;
	}
	else{
		printf("Error!\n");
	}

	send(socket, &resultadoEjecucion, sizeof(resultadoEjecucion), 0);
	send(socket, &esi->cantidadDeLineas, sizeof(esi->cantidadDeLineas), 0);
}
	printf("Mi ID es %d\n",esi->id_ESI);
}
