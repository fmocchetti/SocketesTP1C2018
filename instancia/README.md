# instancia 

***SI USAS LA VM DE LA CATEDRA PARA INSTALAR LAS CUNIT: https://docs.google.com/document/d/17li9WRr3VLbuFI18EBmgYuTmLnj3wushlLf4vODwjzg/edit ***

Instancia

Detallar aca que hace y como se usa


Agregar dato por algoritmo LRU:

SET_LRU recibe la tabla de entradas, la lista de registros, la posicion inicial de memoria, la posicion final y los datos a insertar

SET_LRU(...) inserta un dato en la memoria, si hay lugar en la memoria inserta uno detras del otro, cuando la memoria esta llena comienza la funcion del LRU. Lleva en una estructura auxiliar(lista de Registros) el numero y la cantidad de veces que se accedio a esa entrada. Al pedir insertar un nuevo valor busca la entrada que tenga menos accesos para ubicar el dato atomico en esta.


