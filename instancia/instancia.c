#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circular.h"

int main () {


	int tamanio = 67;
	char* m = (char*)malloc(tamanio);
	char* lectora = m;
	char* finDeMemoria = (m+tamanio);

	char* b[]={"Esta es...\n"};
	char* c[]={"una version Beta...\n"};
	char* d[]={"de insercion Cicular...\n"};
	char* e[]={"<<Aplausos>>"};
	//char* f[]={"!"};

	agrego_circular(&lectora,*b,m,finDeMemoria);
	agrego_circular(&lectora,*c,m,finDeMemoria);
	agrego_circular(&lectora,*d,m,finDeMemoria);
	agrego_circular(&lectora,*e,m,finDeMemoria);
	//agrego_circular(&lectora,*f,m,finDeMemoria);

	printf("%s",m);

	free(m);

return EXIT_SUCCESS;
}
