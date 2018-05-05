#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "circular.h"

void testAgregarCircular(){

	int tamanio = 200;
	char* storage = (char*)malloc(tamanio);
	*storage =0;
	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+tamanio);
	t_list* tabla = list_create();

	char unaClave[] = {"K2005"}; char unaMateria[] = {"Sistemas Operativos"};
	char otraClave[] = {"K9521"}; char otraMateria[] = {"Sintaxis y Semantica de los Lenguajes"};

	SET_circular(&posicionDeLectura,&tabla,&unaMateria,storage,posicionFinDeMemoria);
	SET_circular(&posicionDeLectura,&tabla,&otraMateria,storage,posicionFinDeMemoria);

	char* unido = "Sistemas OperativosSintaxis y Semantica de los Lenguajes";

	CU_ASSERT_STRING_EQUAL(storage,unido);

    free(storage);
    list_destroy(tabla);
}



int correrTests(){


	  CU_initialize_registry();

	  CU_pSuite prueba = CU_add_suite("Suite de prueba", NULL, NULL);
	  CU_add_test(prueba, "PrueboAgregarAMemoria", testAgregarCircular);
	 // CU_add_test(prueba, "dos", test2);
	  //CU_add_test(prueba, "tres", test3);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}


