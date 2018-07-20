#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "LRU.h"
#include "tabla.h"
#include "Dump.h"
int respaldar_informaci(t_list* tabla){



		//sleep(parametros->intervaloDeDump);
		//pthread_mutex_lock(&lock_dump);


		log_info(logger,"****Comienza el proceso respaldo de Informacion en: %s ****","/home/utnso/inst1");

		int tamanioTabla = list_size(tabla);
		if(tamanioTabla <= 0){
			log_info(logger,"**La tabla se encontro vacia, no hay nada para respaldar**");
		//	pthread_mutex_unlock(&lock_dump);
		}
		else{

			struct Dato* unDato;

			int j =0;

			char* rutaArmada = malloc(strlen("/home/utnso/inst1")+40+strlen(getenv("HOME"))+1);
			*rutaArmada = 0;
			for(int i = 0 ; i < tamanioTabla ; i++){

				unDato = list_get( tabla, i );

				strcpy(rutaArmada,"/home/utnso/inst1/");
				strcat(rutaArmada,(const char*)unDato->clave);

				int fd = open(rutaArmada, O_RDWR | O_CREAT, (mode_t)0600);
				if(fd < 0){
				//	log_error(logger,"Error al abrir la ruta, puede que el directorio %s no exista",parametros->puntoDeMontaje);
					return EXIT_FAILURE;
				}
				lseek(fd, unDato->cantidadDeBytes-1, SEEK_SET);
				write(fd, "", 1);
				char *map = mmap(0, unDato->cantidadDeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				memcpy(map, unDato->posicionMemoria, unDato->cantidadDeBytes);
				msync(map, unDato->cantidadDeBytes, MS_SYNC);
				munmap(map, unDato->cantidadDeBytes);
				close(fd);
				log_info(logger,"Se guardo un valor en archivo: %s",rutaArmada);
				j++;

				*rutaArmada = 0;

			}
			log_info(logger,"****Finaliza el proceso respaldo de Informacion..****");
			log_info(logger,"**Se crearon %d archivo/s**",j);

		//	pthread_mutex_unlock(&lock_dump);
			free(rutaArmada);
		}



return EXIT_SUCCESS;
}
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

void testCOMPACTAR(){

	int cantidadEntradas = 8 ;
	int tamanioEntradas = 10;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	*storage =0;

	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));

	t_list* tabla = list_create();

	t_list* registro = crear_registro(cantidadEntradas);

	/*
SET opcion1 fugazzetarellenagrande
SET opcion2 promomuzzarella
SET opcion3 pizzacuatroquesosgrande
SET opcion1 napolitanagrande
SET opcion3 pizzadepalmitos
SET opcion4 pizzadecantimpalo*/

	char clave1[40] = {"opcion1"}; char materia1[] = {"fugazzetarellenagrande"};
	char clave2[40] = {"opcion2"}; char materia2[] = {"promomuzzarella"};
	char clave3[40] = {"opcion3"}; char materia3[] = {"pizzacuatroquesosgrande"};
	char clave4[40] = {"opcion1"}; char materia4[] = {"napolitanagrande"};
	char clave5[40] = {"opcion3"}; char materia5[] = {"pizzadepalmitos"};
	char clave6[40] = {"opcion4"}; char materia6[] = {"pizzadecantimpalo"};
/*	char clave7[40] = {"nintendo:consola:wiiu"}; char materia7[] = {"RIP"};
	char clave8[40] = {"nintendo:consola:nintendo64"}; char materia8[] = {"MarioKart"};
	char clave9[40] = {"sony:consola:ps1"}; char materia9[] = {"FF7"};
	char clave10[40] = {"nintendo:consola:nintendo64"}; char materia10[] = {"PkmnSnap"};
	char clave11[40] = {"sony:consola:ps2"}; char materia11[] = {"GodOfWar"};
	char clave12[40] = {"sony:consola:ps1"}; char materia12[] = {"FF8"};
	char clave13[40] = {"sony:consola:ps3"}; char materia13[] = {"TLOU"};*/
/*	char clave14[40] = {"K9521"}; char materia14[] = {"Matematica Superior"};
	char clave15[40] = {"K2005"}; char materia15[] = {"Sistemas Operativos"};
	char clave16[40] = {"K9521"}; char materia16[] = {"Matematica Superior"};
	char clave17[40] = {"K2005"}; char materia17[] = {"Sistemas Operativos"};
	char clave18[40] = {"K9521"}; char materia18[] = {"Matematica Superior"};
	char clave19[40] = {"K2005"}; char materia19[] = {"Sistemas Operativos"};
	char clave20[40] = {"K9521"}; char materia20[] = {"Matematica Superior"};
	char clave21[40] = {"K2005"}; char materia21[] = {"Sistemas Operativos"};
	char clave22[40] = {"K9521"}; char materia22[] = {"Matematica Superior"};*/


	struct ClaveValor claveValor,claveValor2;
	claveValor.cantidadEntradas = cantidadEntradas;

	strcpy(claveValor.clave,clave1);
	claveValor.valor = (char*)&materia1;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave2);
	claveValor.valor = (char*)&materia2;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave3);
	claveValor.valor = (char*)&materia3;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave4);
	claveValor.valor = (char*)&materia4;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave5);
	claveValor.valor = (char*)&materia5;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	//respaldar_informaci(tabla);
/*
	log_info(logger,"%s",storage);
	log_info(logger,"%s",storage+30);
	log_info(logger,"%s",storage+50);
*//*
	struct Dato* d,*d2,*d3;

	d = list_get(tabla,0);
	d2 = list_get(tabla,1);
	d3 = list_get(tabla,2);



	log_info(logger,"%s",d->posicionMemoria);
	log_info(logger,"%s",d2->posicionMemoria);
	log_info(logger,"%s",d3->posicionMemoria);

*/
	strcpy(claveValor.clave,clave6);
	claveValor.valor = (char*)&materia6;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);


	struct Dato* d,*d2,*d3;

	d = list_get(tabla,0);
	d2 = list_get(tabla,1);
	d3 = list_get(tabla,2);



	log_info(logger,"%s",d->posicionMemoria);
	log_info(logger,"%s",d2->posicionMemoria);
	log_info(logger,"%s",d3->posicionMemoria);

	//respaldar_informaci(tabla);



	liberar_registros(&registro);
	liberar_recursos(&tabla);
	free(storage);




}


void testFINAL(){

	int cantidadEntradas = 8 ;
	int tamanioEntradas = 10;
	char* storage = (char*)malloc(cantidadEntradas*tamanioEntradas);
	*storage =0;

	char* posicionDeLectura = storage;
	char* posicionFinDeMemoria = (storage+(cantidadEntradas*tamanioEntradas));

	t_list* tabla = list_create();

	t_list* registro = crear_registro(cantidadEntradas);

	/*
SET nintendo:consola:nes Mario
SET nintendo:consola:snes SuperMario
SET nintendo:consola:nintendo64 Mario64
SET nintendo:consola:gamecube FinalFantasyTactics
SET nintendo:consola:wii MarioGalaxy
SET nintendo:consola:wii MarioGalaxy2
SET nintendo:consola:wiiu RIP
SET nintendo:consola:nintendo64 MarioKart
SET sony:consola:ps1 FF7
SET nintendo:consola:nintendo64 PkmnSnap
SET sony:consola:ps2 GodOfWar
SET sony:consola:ps1 FF8
SET sony:consola:ps3 TLOU
SET sony:consola:ps2 GodOfWar2
SET sony:consola:ps4 Bloodborne
SET sony:consola:ps3 GodOfWar3
SET microsoft:consola:xbox Halo
SET microsoft:consola:xbox360 Halo
SET microsoft:consola:xboxone Halo
SET nintendo:consola:switch ZeldaBOTW*/

	char clave1[40] = {"nintendo:consola:nes"}; char materia1[] = {"Mario"};
	char clave2[40] = {"nintendo:consola:snes"}; char materia2[] = {"SuperMario"};
	char clave3[40] = {"nintendo:consola:nintendo64"}; char materia3[] = {"Mario64"};
	char clave4[40] = {"nintendo:consola:gamecube"}; char materia4[] = {"FinalFantasyTactics"};
	char clave5[40] = {"nintendo:consola:wii"}; char materia5[] = {"MarioGalaxy"};
	char clave6[40] = {"nintendo:consola:wii"}; char materia6[] = {"MarioGalaxy2"};
	char clave7[40] = {"nintendo:consola:wiiu"}; char materia7[] = {"RIP"};
	char clave8[40] = {"nintendo:consola:nintendo64"}; char materia8[] = {"MarioKart"};
	char clave9[40] = {"sony:consola:ps1"}; char materia9[] = {"FF7"};
	char clave10[40] = {"nintendo:consola:nintendo64"}; char materia10[] = {"PkmnSnap"};
	char clave11[40] = {"sony:consola:ps2"}; char materia11[] = {"GodOfWar"};
	char clave12[40] = {"sony:consola:ps1"}; char materia12[] = {"FF8"};
	char clave13[40] = {"sony:consola:ps3"}; char materia13[] = {"TLOU"};
/*	char clave14[40] = {"K9521"}; char materia14[] = {"Matematica Superior"};
	char clave15[40] = {"K2005"}; char materia15[] = {"Sistemas Operativos"};
	char clave16[40] = {"K9521"}; char materia16[] = {"Matematica Superior"};
	char clave17[40] = {"K2005"}; char materia17[] = {"Sistemas Operativos"};
	char clave18[40] = {"K9521"}; char materia18[] = {"Matematica Superior"};
	char clave19[40] = {"K2005"}; char materia19[] = {"Sistemas Operativos"};
	char clave20[40] = {"K9521"}; char materia20[] = {"Matematica Superior"};
	char clave21[40] = {"K2005"}; char materia21[] = {"Sistemas Operativos"};
	char clave22[40] = {"K9521"}; char materia22[] = {"Matematica Superior"};*/


	struct ClaveValor claveValor,claveValor2;
	claveValor.cantidadEntradas = cantidadEntradas;

	strcpy(claveValor.clave,clave1);
	claveValor.valor = (char*)&materia1;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave2);
	claveValor.valor = (char*)&materia2;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave3);
	claveValor.valor = (char*)&materia3;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave4);
	claveValor.valor = (char*)&materia4;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave5);
	claveValor.valor = (char*)&materia5;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave6);
	claveValor.valor = (char*)&materia6;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave7);
	claveValor.valor = (char*)&materia7;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave8);
	claveValor.valor = (char*)&materia8;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave9);
	claveValor.valor = (char*)&materia9;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave10);
	claveValor.valor = (char*)&materia10;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave11);
	claveValor.valor = (char*)&materia11;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);
	strcpy(claveValor.clave,clave12);
	claveValor.valor = (char*)&materia12;
	claveValor.tamanioEntrada = tamanioEntradas;
	SET_LRU(&registro,&tabla,storage,&posicionDeLectura,posicionFinDeMemoria,&claveValor);



	liberar_registros(&registro);
	liberar_recursos(&tabla);
	free(storage);




}


int correrTestsLRU(){

	configure_logger();
	  CU_initialize_registry();

	  CU_pSuite prueba = CU_add_suite("Suite de prueba LRU", NULL, NULL);
	 // CU_add_test(prueba, "PrueboAgreagarDatosSinReemplazo",testAgregarDatos);
	  //CU_add_test(prueba, "PrueboAgreagarDatosConReemplazo",testReemplazarDatos);
	//  CU_add_test(prueba, "PrueboFINAL",testFINAL);
	  CU_add_test(prueba, "PrueboFINAL",testCOMPACTAR);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}
