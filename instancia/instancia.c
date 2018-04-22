#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(int argc , char *argv[]){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(12345);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		return 1;
	}
	char* buffer = malloc(1000);
	recv(cliente, buffer, 1000, 0);
	printf("Recibi %s\n",buffer);

	while (1) {
		char mensaje [1000];
		printf("escribi algo!\n");
		scanf("%s", mensaje);

		send(cliente, mensaje, strlen(mensaje), 0);
		}

		return 0;

	}
