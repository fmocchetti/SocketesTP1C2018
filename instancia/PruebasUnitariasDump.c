/*
 * PruebasUnitariasDump.c
 *
 *  Created on: 3 jun. 2018
 *      Author: PabloSMonti
 */
#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "Dump.h"
#include "circular.h"
#include "socket_client.h"
void testDumpear(){

	int cantidadEntradas =20 ;
	int tamanioEntradas = 10;
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



	SET_circular(-1,&posicionDeLectura,&tabla,&claveValor,storage,posicionFinDeMemoria);


	strcpy(claveValor2.clave,otraClave);
	claveValor2.valor = (char*)&otraMateria;
	claveValor2.tamanioEntrada = tamanioEntradas;

	SET_circular(-1,&posicionDeLectura,&tabla,&claveValor2,storage,posicionFinDeMemoria);

	configure_logger();


	respaldar_informacion(tabla,"",logger);





	CU_ASSERT_STRING_EQUAL(1,1);

    free(storage);
    liberar_recursos(&tabla);







}




int correrTestsDump(){


	  CU_initialize_registry();

	  CU_pSuite prueba = CU_add_suite("Suite de prueba Dump", NULL, NULL);
	  CU_add_test(prueba, "PrueboDumpear", testDumpear);


	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}

