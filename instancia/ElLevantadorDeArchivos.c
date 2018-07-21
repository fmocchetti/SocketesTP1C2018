/*
 * ElLevantadorDeArchivos.c
 *
 *  Created on: 11 jul. 2018
 *      Author: PabloSMonti
 */
#include "ElLevantadorDeArchivos.h"


char* obtener_valor_de_archivo(char * clave , char* ruta){

	struct stat s;
	int size;
	char* path = (char*)malloc(strlen(ruta)+strlen(clave)+1);
	*path = 0 ;
	strcpy(path,(const char*)ruta);
	strcat(path,(const char*)clave);

	log_info(logger,"UPDATEADOR: Se abrira el archivo ruta: %s",path);

	int fd = open(path,  O_RDONLY, (mode_t)0600);
	if(fd < 0){
		log_error(logger,"UPDATEADOR: Error al abrir la ruta, puede que el directorio %s no exista",path);
		return EXIT_FAILURE;
	}

    int estado = fstat (fd, & s);
    if(estado < 0 ){
		log_error(logger,"UPDATEADOR: Error al calcular tamaño del archivo");
		return EXIT_FAILURE;

    }
    size = s.st_size;
    char* valor = malloc(size+1);
    *valor =0 ;
    char* val = 0;
  	val = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

  	strcpy(valor,val);
  	valor[size]='\0';
	log_info(logger,"UPDATEADOR: El valor obtenido es: %s",valor);

	munmap(valor, size);

	close(fd);
	free(path);


return valor;

}






int levantar_archivos_a_memoria(char** storage,int tamanioEntrada,t_list** tabla,t_dictionary* tablaDeRequeridas,char* posicionDeLectura,char* posicionFinDeMemoria,char* path){


	  struct ClaveValor claveValor;
	  struct dirent * dir;


	  log_info(logger,"UPDATEADOR: Se abrira el directorio %s", path);
	  DIR * directorio = opendir(path);


	  if(directorio==NULL){
		  log_error(logger,"UPDATEADOR: No se encontro el directorio %s", path);
		  return EXIT_FAILURE;
	  }


	  while ((dir = readdir(directorio)) != NULL)
	  {
	      if(dir -> d_type == DT_REG && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0){
	    	  log_info(logger,"UPDATEADOR: Se detecto un archivo llamadado:  %s y se leeran los datos", dir->d_name);

	    	  if(dictionary_has_key(tablaDeRequeridas,dir->d_name))
	    	  {
	    		    log_info(logger,"UPDATEADOR: Se guardara la clave:  %s  en memoria", dir->d_name);

	    		    strcpy(claveValor.clave, (const char*)dir->d_name);

	    		    claveValor.valor = obtener_valor_de_archivo(dir->d_name , path);

	    		    log_info(logger,"UPDATEADOR: Se guardara el valor:  %s  en memoria, correspondiente a la clave", claveValor.valor);

	    		    claveValor.tamanioEntrada = tamanioEntrada;

					SET_circular(-1,&posicionDeLectura,tabla,&claveValor,storage,posicionFinDeMemoria);

					free(claveValor.valor);

	      	  }
	      }
	  }

	  closedir(directorio);
	  log_info(logger,"UPDATEADOR: Finaliza el proceso, se cargaron las claves en storage");
return EXIT_SUCCESS;
}



