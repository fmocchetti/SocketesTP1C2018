#include "consola-planificador.h"

void configure_logger();
int connect_to_server(char * ip, char * port);
void wait_hello(int socket);
void enviarServer(int);
char * aQuienMeConecte(int);

int main() {
  configure_logger();
  int socket = connect_to_server(IP, PUERTO);
  wait_hello(socket);
  char * serverDondeMeConecte = aQuienMeConecte(socket);
  printf("%s\n", serverDondeMeConecte);
  enviarServer(socket);




  exit_gracefully(0);
}

int connect_to_server(char * ip, char * port) {
  struct addrinfo hints;
  struct addrinfo *server_info;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

  getaddrinfo(ip, port, &hints, &server_info);  // Carga en server_info los datos de la conexion

  // 2. Creemos el socket con el nombre "server_socket" usando la "server_info" que creamos anteriormente
  int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

  // 3. Conectemosnos al server a traves del socket!
  int res = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen);

  freeaddrinfo(server_info);  // No lo necesitamos mas

  /*
    3.1 Recuerden chequear por si no se pudo contectar.
        Si hubo un error, lo loggeamos y podemos terminar el programa con la funcioncita
        exit_gracefully pasandole 1 como parametro para indicar error ;).
        Pss, revisen los niveles de log de las commons.
  */
  if (res < 0) {
    _exit_with_error(server_socket, "No me pude conectar al servidor", NULL);
  }
  // 4 Logeamos que pudimos conectar y retornamos el socket
  log_info(logger, "Conectado!");
  return server_socket;
}

void _exit_with_error(int socket, char* error_msg, void * buffer) {
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

void configure_logger() {

  logger = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_INFO);
}

void  wait_hello(int socket) {
    char * hola = "Hola!";

    /*
      5.  Ya conectados al servidor, vamos a hacer un handshake!
          Para esto, vamos a, primero recibir un mensaje del
          servidor y luego mandar nosotros un mensaje.
          Deberìamos recibir lo mismo que está contenido en la
          variable "hola". Entonces, vamos por partes:
          5.1.  Reservemos memoria para un buffer para recibir el mensaje.
    */
    char * buffer = (char*) calloc(sizeof(char), strlen(hola) + 1);
    //char * buffer = (char*) calloc(sizeof(char), strlen(hola) + 1);
    /*
          5.2.  Recibamos el mensaje en el buffer.
          Recuerden el prototipo de recv:
          conexión - donde guardar - cant de bytes - flags(si no se pasa ninguno puede ir NULL)
          Nota: Palabra clave MSG_WAITALL.
    */
    int result_recv = recv(socket, buffer, strlen(hola), MSG_WAITALL); //MSG_WAITALL
    printf("Recibi %s\n",buffer);

    /*
          5.3.  Chequiemos errores al recibir! (y logiemos, por supuesto)
          5.4.  Comparemos lo recibido con "hola".
                Pueden usar las funciones de las commons para comparar!
          No se olviden de loggear, cerrar el socket y devolver la memoria que pedimos!
          (si, también si falló algo, tenemos que devolverla, atenti.)
    */
    if(result_recv <= 0) {
      _exit_with_error(socket, "No se pudo recibir hola", buffer);
    }

    if (strcmp(buffer, hola) != 0) {
      _exit_with_error(socket, "No se pudo recibir hola", buffer);
    }

    log_info(logger, "Mensaje de hola recibido: '%s'", buffer);
    free(buffer);
}

void enviarServer(int socket){
while (1) {
  		char mensaje [1000];
  		printf("escribi algo!\n");
  		scanf("%s", mensaje);
  		printf("valor de mensaje %s!\n", mensaje);

  		send(socket, mensaje, strlen(mensaje), 0);


  		char * server_reply = (char*) calloc(sizeof(char), 100);


  		        //Receive a reply from the server
  		        if( recv(socket , server_reply , 200 , 0) < 0)
  		        {
  		            puts("recv failed");
  		            break;
  		        }
  		log_info(logger,"recibi %s de %d\n", server_reply, socket );
  		free(server_reply);
  		}
}

char * aQuienMeConecte(int socket){
	char  mensaje [] = "Identificate";
	send(socket, mensaje, strlen(mensaje), 0);
	printf("envie %s\n",mensaje);
	char * server_reply = (char*) calloc(sizeof(char), 100);
	recv(socket, server_reply, 100, 0);
	printf("recibi %s\n", server_reply);
	return(server_reply);

}
