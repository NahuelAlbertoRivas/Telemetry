#ifndef COMPLEMENTOFNS_H_INCLUDED
#define COMPLEMENTOFNS_H_INCLUDED

// Ac� bibliotecas

typedef struct{
   FILE *archAsociado;
   char nomAsociado[12];
   //struct tm *fechaInicial; // si a futuro se eligiese manejar los datetimes de manera unificada mediante la estructura tm entonces habilitaremos este campo y dejaremos sin efecto ' diaActual ', ' mesActual ' y ' anioActual '; tanto este campo como los �ltimos tres mencionados determinar�n el inicio del thread cronol�gico asociado al anexo en cuesti�n
   // anexos *sigAnexo; // a futuro se podr�a implementar un sistema de referenciamiento al siguiente anexo mediante punteros, sin necesidad de realizar ordenamientos
   unsigned int creado; // indicar� si ya fue creado el anexo
   unsigned int regInicio; // indicar� que es la primer lectura del archivo, ende guardaremos la fechaInicial
   int diaActual; // tanto este campo como los dos siguientes servir�n para registrar inconsistencias en caso de que el/los �ltimos registros recibidos no sean cronologicamente correctos; obs: cuando se use la estructura tm, �stos se deshabilitan
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
