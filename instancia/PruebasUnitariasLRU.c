#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "LRU.h"
#include "tabla.h"

void testAgregarDatos(){

	int cantidadEntradas = 3 ;
	int tamanioEntradas = 20;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	*storage =0;

	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));

	t_list* tabla = list_create();

	t_list* registro = crear_registro(cantidadEntradas);

	char unaClave[40] = {"K2005"}; char unaMateria[] = {"Sistemas Operativos"};
	char otraClave[40] = {"K9521"}; char otraMateria[] = {"Matematica Superior"};

	struct ClaveValor claveValor,claveValor2;

	strcpy(claveValor.clave,unaClave);
	claveValor.valor = (char*)&unaMateria;
	claveValor.tamanioEntrada = tamanioEntradas;

	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);

	strcpy(claveValor2.clave,otraClave);
	claveValor2.valor = (char*)&otraMateria;
	claveValor2.tamanioEntrada = tamanioEntradas;

	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor2);

	struct Registro* unRegistro = list_get(registro,0);

	CU_ASSERT_EQUAL(unRegistro->referenciado,1);

	liberar_registros(&registro);
	liberar_recursos(&tabla);
	free(storage);

}


void testReemplazarDatos(){

	int cantidadEntradas = 2 ;
	int tamanioEntradas = 20;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	*storage =0;

	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));

	t_list* tabla = list_create();

	t_list* registro = crear_registro(cantidadEntradas);

	char unaClave[40] = {"K2005"}; char unaMateria[] = {"Sistemas Operativos"};
	char otraClave[40] = {"K9521"}; char otraMateria[] = {"Matematica Superior"};

	struct ClaveValor claveValor,claveValor2;

	strcpy(claveValor.clave,unaClave);
	claveValor.valor = (char*)&unaMateria;
	claveValor.tamanioEntrada = tamanioEntradas;

	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);

	strcpy(claveValor2.clave,otraClave);
	claveValor2.valor = (char*)&otraMateria;
	claveValor2.tamanioEntrada = tamanioEntradas;

	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor2);

	//modifico el registro por fuera para simular que una entrada fue accedida mas que otra

	struct Registro* unRegistro = list_get(registro,0);
	unRegistro->referenciado += 1;


	//inserto dos veces el valor uno pra probar reemplazo
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);

	struct Registro* otroRegistro = list_get(registro,0);

	CU_ASSERT_EQUAL(otroRegistro->referenciado,2);

	liberar_registros(&registro);
	liberar_recursos(&tabla);
	free(storage);




}



int correrTestsLRU(){


	  CU_initialize_registry();

	  CU_pSuite prueba = CU_add_suite("Suite de prueba LRU", NULL, NULL);
	  CU_add_test(prueba, "PrueboAgreagarDatosSinReemplazo",testAgregarDatos);
	  CU_add_test(prueba, "PrueboAgreagarDatosConReemplazo",testReemplazarDatos);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}
