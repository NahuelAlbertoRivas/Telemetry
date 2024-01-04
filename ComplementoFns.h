#ifndef COMPLEMENTOFNS_H_INCLUDED
#define COMPLEMENTOFNS_H_INCLUDED

// Acá bibliotecas

typedef struct{
   FILE *archAsociado;
   char nomAsociado[12];
   //struct tm *fechaInicial; // si a futuro se eligiese manejar los datetimes de manera unificada mediante la estructura tm entonces habilitaremos este campo y dejaremos sin efecto ' diaActual ', ' mesActual ' y ' anioActual '; tanto este campo como los últimos tres mencionados determinarán el inicio del thread cronológico asociado al anexo en cuestión
   // anexos *sigAnexo; // a futuro se podría implementar un sistema de referenciamiento al siguiente anexo mediante punteros, sin necesidad de realizar ordenamientos
   unsigned int creado; // indicará si ya fue creado el anexo
   unsigned int regInicio; // indicará que es la primer lectura del archivo, ende guardaremos la fechaInicial
   int diaActual; // tanto este campo como los dos siguientes servirán para registrar inconsistencias en caso de que el/los últimos registros recibidos no sean cronologicamente correctos; obs: cuando se use la estructura tm, éstos se deshabilitan
   int mesActual;
   int anioActual;
}anexos;

int generarReporte(const char *, size_t, size_t, size_t, void accion(FILE *, FILE *, void *, void *, anexos *, unsigned int, unsigned int*));
void guardarvBatEnArchivo(FILE *, FILE *, void *, void *, anexos *, unsigned int, unsigned int*);
void leerRegistroYfecha(FILE *, FILE *);
int isLittleEndian();
void reverseData(void* num, size_t tam);
void inicializarAnexos(anexos *, size_t);
void ordenarAnexosBurbujeo(anexos *, size_t);
int unificarArchivos(anexos *, size_t);
/*struct tm* createCustomDatetime();
void liberarCustomDate(anexos *, size_t);*/

#endif // COMPLEMENTOFNS_H_INCLUDED
