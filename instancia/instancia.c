#include "circular.h"
#include "tabla.h"

int main () {


	int tamanio = 69;
	char* storage = (char*)malloc(tamanio);
	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+tamanio);

	char* b[]={"Esta es...\n"};
	char* c[]={"una version Beta...\n"};
	char* d[]={"de insercion Cicular...\n"};
	char* e[]={"<<Aplausos>>\n"};
	//char* f[]={"AAAAA"};//Prueba desbordamiento

	agrego_circular(&posicionDeLectura,*b,storage,posicionFinDeMemoria);
	agrego_circular(&posicionDeLectura,*c,storage,posicionFinDeMemoria);
	agrego_circular(&posicionDeLectura,*d,storage,posicionFinDeMemoria);
	agrego_circular(&posicionDeLectura,*e,storage,posicionFinDeMemoria);
	//agrego_circular(&lectora,*f,m,finDeMemoria); //Prueba desbordamiento

	printf("%s",storage);

	free(storage);

	//***SE PRUEBA EL AGREGADO Y BUSQUEDA DE DATOS REGISTRADOS EN LA TABLA DE ENTRADAS***********************
	// Mas adelate estas funciones estaran incluidas en las funciones de circular.c




	//tabla guarda un puntero a la dir de memoria, la frecuencia de uso y el tamanio
	t_list* tabla = list_create();



	char unaClave[] = {"K2005"}; char unaMateria[] = {"Sistemas Operativos"};
	char otraClave[] = {"K9521"}; char otraMateria[] = {"Sintaxis y Semantica de los Lenguajes"};

	struct Dato unDato, otroDato;
	unDato.valor = &unaMateria;
	unDato.clave = &unaClave;
	otroDato.valor = &otraMateria;
	otroDato.clave = &otraClave;

	registrar_dato_en_tabla(&tabla,&unDato);
	registrar_dato_en_tabla(&tabla,&otroDato);


	//-----------------------------------------UNIT TESTS CASERAS --------------------------------------------------------

	puts("\n UNIT TESTS \n");
	//busca palabra
	if(buscar(tabla,&otraClave)==&otraMateria){
		printf("La clave %s buscada coincide con el valor: %s",otraClave,otraMateria);
	};




return EXIT_SUCCESS;
}
