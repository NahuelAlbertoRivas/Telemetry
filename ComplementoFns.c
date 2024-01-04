#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ComplementoFns.h"

#define CANTV 5
#define AJUSTAR_DATETIME 315975600 // Se detalla el uso de esta macro en la línea (dentro de la fn. ' guardarvBatEnArchivo ')
#define TAM_CADENA_DATETIME 26 // Esta macro servirá para recuperar la fecha-hora en formato de string
#define OFFSET_A_FECHA 100 // Si en un futuro cambia de ubicación (offset) la fecha-hora de cada paquete, se debe modificar esta línea

int generarReporte(const char *nombreArch, size_t tamLote, size_t tamOffset, size_t tamRegistro, void accion(FILE *, FILE *, void *, void *, anexos *, unsigned int, unsigned int*)){
    FILE *pf_Arch;
    anexos vecAnexos[CANTV]; // el concepto de ' anexo ' se da ante la búsqueda de solucionar el inconveniente planteado en la parte 3 del trabajo práctico, aquél refiere a una estructura que implícitamente está dotada del manejo sobre un archivo propio, determinado por un ' thread cronológico ' (línea/período de tiempo, en el caso del primer elemento del vector es el que llevará por defecto el 'thread cronológico principal', osea, la línea cronológica de referencia para saber cuándo un registro irrumpe con el orden temporal). Obsérvese que si no hay inconsistencias temporales entre lectura de registros adyacentes, entonces el primer elemento del vector será equivalente en su plenitud al reporte final, caso contrario ante el mínimo error detectado sobre la temporalidad se crean "anexos auxiliares" (que implícitamente son archivos) que luego se unificarán para formar dicho reporte final
    long int tamArchivo;
    void *data, *dtTime; // ' data ' es el dato que se quiere recuperar, en nuestro caso el voltaje; ' dtTime ' es la fecha-hora asociada a cada paquete
    unsigned int i, cantRegistros, cantUnir = 0, estado = 0; // ' cantUnir ' llevará registro de la cantidad de archivos a combinar en caso de detectar errores ; 'estado ' determinará cada línea de acción a seguir dependiento si => estado = 0: no hay error en trayecto cronológico; estado = 1: se detecta error en secuencia de tiempo; estado = 2: inicia correción de error (una vez se detecta que el datetime del último registro leído es mayor al datetime inicio del thread cronológico, el estado vuelve a 0) obs: luego del estado = 2, si no nos aseguran que lo que vendrá guarda relación cronológica lineal con el final del thread cronológico principal, podríamos pasar a un estado = 3 donde simplemente se crea y registra todos los próximos registros en otro archivo adicional, luego podría verse futuras decisiones
    char nombreAnexo[12];
    pf_Arch = fopen(nombreArch, "rb");
    if(!pf_Arch) { exit(1); }
    fseek(pf_Arch, 0, SEEK_END);
    tamArchivo = ftell(pf_Arch);
    if((tamArchivo % tamLote) != 0){ fclose(pf_Arch); exit(1); }
    cantRegistros = tamArchivo / tamLote;
    strcpy(vecAnexos->nomAsociado, "Anexo0.csv");
    vecAnexos->archAsociado = fopen(vecAnexos->nomAsociado, "wt"); // este elemento en particular -vecAnexos[0] para ser más explícito- (el primero, inicio del vector de anexos), será el que determine el thread cronológico principal
    if(!(vecAnexos->archAsociado)) { fclose(pf_Arch); exit(1); }
    inicializarAnexos(vecAnexos, CANTV);
    (vecAnexos + cantUnir)->creado = 1;
    data = malloc(tamRegistro);
    dtTime = malloc(sizeof(unsigned));
    cantUnir++;
    for(i = 0; i < cantRegistros; i++){
        fseek(pf_Arch, (tamLote * i) + tamOffset, SEEK_SET);
        fread(data, sizeof(data), 1, pf_Arch);
        fseek(pf_Arch, (tamLote * i) + OFFSET_A_FECHA, SEEK_SET);
        fread(dtTime, sizeof(dtTime), 1, pf_Arch);
        if (estado == 0 || estado == 2) {
            accion(pf_Arch, (vecAnexos + cantUnir - 1)->archAsociado, data, dtTime, vecAnexos, cantUnir - 1, &estado); // se pasa una dirección anterior ya que ' cantUnir ' siempre estará 1 lugar adelantado para ya referenciar al próximo elemento que debe ser creado en caso de corresponder
            if(estado == 1) { i--; } // se debe repetir una iteración ya que internamiente en ' guardarBatEnArchivo ' ese registro no impactará en el archivo en cuestión, sino que deberá guardarse en el próximo (éste estará contenido en (vecAnexos + cantUnir))
        }
        else {
            if((estado == 1) && (vecAnexos + cantUnir)->creado == 0) {
                snprintf(nombreAnexo, sizeof(nombreAnexo), "Anexo%d.csv", cantUnir);
                strcpy((vecAnexos + cantUnir)->nomAsociado, nombreAnexo);
                (vecAnexos + cantUnir)->archAsociado = fopen(nombreAnexo, "wt");
                (vecAnexos + cantUnir)->creado = 1;
                cantUnir++; // Esta línea ayudará a detectar cuando ya se termina de corregir el error (ya que estado volverá a 0, y se debe incrementar cantUnir para una próxima situación a corregir); obs: por defecto, el campo ' creado ' de una estructura ' anexos ' es igual a 0, lo que significa que, luego de terminar de tratar el error estado vuelve a valer 0 pero vecAnexos->creado = 1, significando una bandera para el caso en cuestión
                estado = 2;
            } // cantUnir servirá para distinguir archivos en caso de que hayan errores y luego se deba reconstruir un archivo final
        }
    }
    for(i = 0; i < cantUnir; i++){
        fclose((vecAnexos + i)->archAsociado);
    }
    if(cantUnir > 1){ // estado detectará si hubo errores en la secuencia de fechas recibidas (si estado = 1 => hubo error; si = 0, todo correcto
        ordenarAnexosBurbujeo(vecAnexos, cantUnir);
        unificarArchivos(vecAnexos, cantUnir);
    }
    else { rename("Anexo0.csv", "Report.csv"); } // Si no hubo que corregir errores, el primer anexo (' Anexo0.csv ') será el informe final
    free(data);
    free(dtTime);
    fclose(pf_Arch);
    return 1;
}
// Aclaración previa: cada vez que se mencione ' registro ', nos referimos a una tupla (datetime, valor de voltaje), ende la temporalidad de un registro (concepto fundamental en la lógica del algoritmo para el manejo de anexos) refiere a tal campo ' datetime ' (fecha y hora)
void guardarvBatEnArchivo(FILE *pf_Arch, FILE *pf_Report, void *data, void *dtTime, anexos *vecAnexos, unsigned int posAnexoActual, unsigned int *estado){ // retorna un int que informe cuándo [y hasta dónde hubo inconvenientes] en el orden de recepción de datetimes
    unsigned short int raw; // raw es el valor previo (crudo) a obtener el ' valor de ingeniería final '
    float rawFinal;
    unsigned int vBatTime = 0; // [Detalle no determinante] Ver qué decisión efectuar cuando el valor recuperado de ' data ' en ' vBatTime ' se mantiene en 0 debido a errores mencionados en documentación (el sistema está iniciando, el GPS no funciona, error en disposición del contenido en el archivo, entre otros)
    struct tm *newtime; // este dato guardará en cada campo (heredado de la estructura tm) la fecha-hora correspondiente acorde a los segundos recuperados en ' elapsedTime '
    time_t elapsedTime; // este dato se usará para hacer uso de gmtime(), contendrá la fecha-hora del paquete en segundos (ajustados mediante ' AJUSTAR_DATETIME ', explicación del ajuste en línea 75)
    char dateT[TAM_CADENA_DATETIME], *p; // ' dateT ' almacenará en formato string (mediante asctime()) la fecha que se haya recuperado en segundos (previamente guardados en ' elapsedtime ' para poder hacer uso de gmtime())
    raw = *((unsigned short int*)data);
    vBatTime = *((unsigned int *) dtTime); // se debe sumar 315975600 segundos debido a la diferencia entre la fecha en que la documentación informa como referencia (6-1-1980) en contexto a la fecha que maneja la biblioteca time.h (1-1-1970)
    if(isLittleEndian()) { reverseData(&raw, sizeof(raw)); reverseData(&vBatTime, sizeof(vBatTime)); }
    rawFinal = (float)raw * 0.01873128 + (-38.682956);
    elapsedTime = vBatTime + AJUSTAR_DATETIME; // se realiza corrección mencionada en línea 75
    newtime = gmtime(&elapsedTime); // [Detalle no determinante] si hubo un inconveniente, sea cualquiera mencionado en la línea 65, simplemente la fecha asociada a tal registro será ' Sun Jan 6 1980... ', entonces, el error será evidente
    if((vecAnexos + posAnexoActual)->regInicio == 0) { // se verifica si el anexo actual no fue seteado (o bien propiamente iniciado, como su campo explícitamente indica), de corresponder se le asignan los recursos necesarios para que más adelante sea capaz de determinar la validez temporal de cada último registro recuperado
        (vecAnexos + posAnexoActual)->diaActual = newtime->tm_mday;
        (vecAnexos + posAnexoActual)->mesActual = newtime->tm_mon;
        (vecAnexos + posAnexoActual)->anioActual = newtime->tm_year;
        (vecAnexos + posAnexoActual)->regInicio = 1;
    }
    strcpy(dateT, asctime(newtime)); // Se formatea a string la fecha
    p = strrchr(dateT, '\n'); // necesario ya que, por defecto, el formato que retorna asctime() posee un salto de línea antes de terminar el string
    *p = '\0';
    if(((vecAnexos + posAnexoActual)->anioActual >= newtime->tm_year) && ((vecAnexos + posAnexoActual)->mesActual >= newtime->tm_mon) && ((vecAnexos + posAnexoActual)->diaActual > newtime->tm_mday)) { // donde cada campo de (vecAnexos + posAnexoActual) comparado es el último día identificado en el thread cronológico (no siendo éste necesariamente el principal, sino el interno al anexo en cuestión), y newtime es el último registro leído ; en esta línea básicamente detecta cuándo el datetime del último registro leído ya no es menor al datetime (fechaInicial) del archivo anterior
        if(*estado == 0) { *estado = 1; }
    }
    else {
        if(*estado == 0) {
            fprintf(pf_Report, "%s,%2.3f\n", dateT, rawFinal);
            if((vecAnexos + posAnexoActual)->diaActual != newtime->tm_mday) { (vecAnexos + posAnexoActual)->diaActual = newtime->tm_mday; } // Se detecta el día actual (considerando que previamente en la línea 98 nos aseguramos que el datetime del último registro no será menor al datetime actual del registro cronológico), en caso de corresponder se actualiza
        }
        if(*estado == 2) {
            if((newtime->tm_year >= vecAnexos->anioActual) && (newtime->tm_mon >= vecAnexos->mesActual) && (newtime->tm_mday >= vecAnexos->diaActual)) { // se detecta cuándo el último registro leído posee un datetime menor al inicio del thread cronológico principal
                *estado = 0; // una vez que el datetime del último registro deje de ser menor al último datetime del thread cronológico, entonces estado vuelve a 0 (ya no se detecta error)
            }
            else { fprintf(pf_Report, "%s,%2.3f\n", dateT, rawFinal); }
        }
    }
}

int isLittleEndian(){
    int r;
    char *pini;
    long l = 0x00010203;
    pini = (char *)&l;
    if(pini[0] == 0x03) { r = 1; } // es little endian
    else if(pini[0] == 0x00) { r = 0; } // es big endian
    return r;
}

void reverseData(void* num, size_t tam){
    char *pi = num, *pf = num + tam - 1;
    char aux;
    while(pi < pf){
        aux = *pf;
        *pf = *pi;
        *pi = aux;
        pi++;
        pf--;
    }
}

void inicializarAnexos(anexos *vec, size_t ce){
    unsigned int i;
    for(i = 0; i < ce; i++){
        (vec + i)->creado = 0;
        (vec + i)->regInicio = 0;
    }
}

void ordenarAnexosBurbujeo(anexos *vec, size_t ce){
    int i, desordenado = 1, cota = ce - 1;
    anexos aux;
    while(desordenado){
        desordenado = 0;
        for(i = 0; i < cota; i++){
            if((vec + i)->anioActual >= (vec + i + 1)->anioActual && (vec + i)->mesActual >= (vec + i + 1)->mesActual && (vec + i)->diaActual > (vec + i + 1)->diaActual){ // Obs: podemos ordenar a partir de su último día
                memcpy(&aux, vec + i, sizeof(aux));
                memcpy(vec + i, vec + i + 1, sizeof(aux));
                memcpy(vec + i + 1, &aux, sizeof(aux));
                desordenado = i;
            }
        }
        cota = desordenado;
    }
}

int unificarArchivos(anexos *vecAnexos, size_t cantUnir){ // Unir todos los archivos auxiliares en caso de haber requerido creación dinámica
    FILE *pf_finalReport; // donde ' archInicial ' es nuestro ' aux.csv ' el cual en primer instancia almacenaría todos los registros consecutivos en caso de no haber inconsistencias; en esta instancia, evidentemente se han detectado errores, ende tenemos que leer ' aux.csv ' para cargar desde el inicio
    int i, j;
    char linea[100];
    pf_finalReport = fopen("Report.csv", "wt");
    for(i = 0; i < cantUnir; i++){
        (vecAnexos + i)->archAsociado = fopen((vecAnexos + i)->nomAsociado, "rt");
        if(!((vecAnexos + i)->archAsociado)) {
            for(j = 0; j < i; j++){ fclose((vecAnexos + j)->archAsociado); }
            return 0;
        }
    }
    for(i = 0; i < cantUnir; i++){
        while(fgets(linea, sizeof(linea), (vecAnexos + i)->archAsociado)){ fputs(linea, pf_finalReport); }
    }
    for(i = 0; i < cantUnir; i++){ fclose((vecAnexos + i)->archAsociado); }
    fclose(pf_finalReport);
    for(i = 0; i < cantUnir; i++){ remove((vecAnexos + i)->nomAsociado); }
    return 1;
}

/*struct tm* createCustomDatetime(){ Estas funciones se implementarán si se desease trabajar de manera unificada con los datetime, como se plantea en la definición de la estructura ' anexo '
    struct tm *t = malloc(sizeof(*t));
    t->tm_mday = 20; // estos valores no son necesarios, simplemente verifican el correcto funcionamiento
    t->tm_mon = 2;
    t->tm_year = 120;
    return t;
}

void liberarCustomDate(anexos *vec, size_t ce){
    int i;
    for(i = 0; i < ce; i++){ free((vec + i)->fechaInicial); }
}*/

