/*
 * PruebasUnitariasSTORE.c
 *
 *  Created on: 7 jun. 2018
 *      Author: Pablo Monti
 */

#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "STORE.h"
#include "circular.h"
#include "socket_client.h"

void testStore(){

	int cantidadEntradas =20 ;
	int tamanioEntradas = 10;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));
	t_list* tabla = list_create();
	configure_logger();

	char unaClave[40] = {"K2888"}; char unaMateria[] = {"Sistemas Operativos"};

	struct ClaveValor claveValor;


	strcpy(claveValor.clave,unaClave);
	claveValor.valor = (char*)&unaMateria;
	claveValor.tamanioEntrada = tamanioEntradas;



	SET_circular(&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);



	char* ruta;
	*ruta="";

	STORE(tabla,unaClave,ruta,logger);

	CU_ASSERT_STRING_EQUAL(1,1);

    free(storage);
    liberar_recursos(&tabla);


}




int correrTestsSTORE(){


	  CU_initialize_registry();

	  CU_pSuite prueba = CU_add_suite("Suite de prueba store", NULL, NULL);
	  CU_add_test(prueba, "PrueboStore", testStore);


	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}
