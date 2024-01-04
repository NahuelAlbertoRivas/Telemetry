#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include "ComplementoFns.h"

// el primer par�metro se corresponde con el nombre del archivo
// el segundo, con el tama�o de cada "lote" (lo que en el enunciado se menciona como "registros o paquetes")
// el tercero, el offset hasta la ubicaci�n exacta del dato que se desea generar el informe
// finalmente, se pasa la funci�n a efectuar (leer la segunda observaci�n debajo para mayor contexto)

int main(int argc, char* argv[])
{
    generarReporte(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), guardarvBatEnArchivo);
    return 0;
}

/* Obs:
1) Se setea en argumentos del proyecto el nombre del archivo.
2) Si bien el programa lleva por defecto la funci�n ' guardarvBatEnArchivo ', podr�a implementarse un quinto par�metro que sea
el selector de operaci�n a realizar (por ejemplo, si se pasa un 1, entonces el usuario pretende asociar el reporte
particularmente a los registros de voltaje -por lo que la fn. pasada por par�metro ser�a efectivamente ' guardarvBatEnArchivo '-;
si pasa un 2, entonces pretende asociar el reporte a los registros de "timeMaxFreePowerCrtl" -ende se pasar�a otra funci�n
llamada, por ejemplo, ' guardarTimeMaxEnArchivo '-; etc). ->
switch(argv[4]){
       case 1: �dem a la sentencia 11
       break;
       case 2: generarReporte(argv[0], argv[1], argv[2], argv[3], guardarTimeMaxEnArchivo);
       break;
       etc...
}
*/
