#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "circular.h"

void testAgregarCircular(){

	int cantidadEntradas =20 ;
	int tamanioEntradas = 5;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	*storage =0;
	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));
	t_list* tabla = list_create();

	char unaClave[40] = {"K2005"}; char unaMateria[] = "Sistemas Operativos";
	char otraClave[40] = {"K9521"}; char otraMateria[] = "Sintaxis y Semantica de los Lenguajes";

	struct ClaveValor claveValor;

	strcpy(claveValor.clave,unaClave);
	claveValor.valor = (char*)&unaMateria;
	claveValor.tamanioEntrada = tamanioEntradas;

	SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);


	strcpy(claveValor.clave,otraClave);
	claveValor.valor = (char*)&otraMateria;
	claveValor.tamanioEntrada = tamanioEntradas;

	SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);

	char* unido = "Sistemas Operativos Sintaxis y Semantica de los Lenguajes";


	CU_ASSERT_STRING_EQUAL(storage,unido);

    free(storage);
    list_destroy(tabla);
}


void testRegistrarEnTablaClave(){

	char unaClave[40] = "unaCLAVE";

	struct Dato unDato, *datoObtenido;

	*unDato.clave = unaClave;

	t_list* tabla = list_create();

	registrar_dato_en_tabla(&tabla,&unDato);

	datoObtenido = (struct Dato*)list_get(tabla,0);

	CU_ASSERT_STRING_EQUAL(unDato.clave,datoObtenido->clave);


    list_destroy(tabla);
}

void testSetYRegistrarObtenerClave(){


	int cantidadEntradas =20 ;
	int tamanioEntradas = 5;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));
	t_list* tabla = list_create();

	char unaClave[40] = {"K2005"}; char unaMateria[] = {"Sistemas Operativos"};
	char otraClave[40] = {"K9521"}; char otraMateria[] = {"Sintaxis y Semantica de los Lenguajes"};

	struct ClaveValor claveValor,claveValor2;


	strcpy(claveValor.clave,unaClave);
	claveValor.valor = (char*)&unaMateria;
	claveValor.tamanioEntrada = tamanioEntradas;



	SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);


	strcpy(claveValor2.clave,otraClave);
	claveValor2.valor = (char*)&otraMateria;
	claveValor2.tamanioEntrada = tamanioEntradas;

	SET_circular(&posicionDeLectura,&tabla,&claveValor2,storage,posicionFinDeMemoria);


	struct Dato* unDato = (struct Dato*)list_get(tabla,1);


	CU_ASSERT_STRING_EQUAL(unDato->clave,otraClave);

    free(storage);
    list_destroy(tabla);




}

void testSetYRegistrarObtenerValor(){

	int cantidadEntradas =20 ;
	int tamanioEntradas = 5;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	*storage =0;
	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));
	t_list* tabla = list_create();

	char unaClave[40] = {"K2005"}; char unaMateria[] = {"Sistemas Operativos"};
	char otraClave[40] = {"K9521"}; char otraMateria[] = {"Sintaxis y Semantica de los Lenguajes"};

	struct ClaveValor claveValor,claveValor2;

	strcpy(claveValor.clave,unaClave);
	claveValor.valor = (char*)&unaMateria;
	claveValor.tamanioEntrada = tamanioEntradas;


	SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);


	strcpy(claveValor2.clave,otraClave);
	claveValor2.valor = (char*)&otraMateria;
	claveValor2.tamanioEntrada = tamanioEntradas;

	SET_circular(&posicionDeLectura,&tabla,&claveValor2,storage,posicionFinDeMemoria);


	struct Dato* unDato = (struct Dato*)list_get(tabla,1);

	char* unValorObtenidoDeStorage = malloc(unDato->cantidadDeBytes);
	memcpy(unValorObtenidoDeStorage,unDato->posicionMemoria,unDato->cantidadDeBytes);

	CU_ASSERT_STRING_EQUAL(unValorObtenidoDeStorage,otraMateria);

    free(storage);free(unValorObtenidoDeStorage);
    list_destroy(tabla);



}


int correrTests(){


	  CU_initialize_registry();

	  CU_pSuite prueba = CU_add_suite("Suite de prueba", NULL, NULL);
	  //CU_add_test(prueba, "PrueboAgregarAMemoria", testAgregarCircular);
	  CU_add_test(prueba, "PrueboRegistrarDatoConClaveEnTabla", testRegistrarEnTablaClave);
	  CU_add_test(prueba, "PrueboAgregarYObtenerDeTablaLaClave", testSetYRegistrarObtenerClave);
	  CU_add_test(prueba, "PrueboAgregarYObtenerDeTablaUnValor", testSetYRegistrarObtenerValor);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}


