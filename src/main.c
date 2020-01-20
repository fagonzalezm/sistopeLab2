#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "main.h"

//Entradas: En argv se debe ingresar -c <Cantidad de imagenes> -m <Nombre del archivo con el filtro> -n <Umbral para determinar nearlyblack> -t <Cantidad de hebras> -h <Tamaño del buffer> <-b si se requiere mostrar la evaluación de nearlyblack en el terminal>.
//Funcionamiento: Realiza el pipeline. Primero, se leen los argumentos del main usando getopt. Luego, se evaluan si los parametros son adecuados. Finalmente, se ejecuta el pipeline, delegando la lectura de imagenes a este proceso.
//Salida: --
int main(int argc, char **argv){
    //Lectura de  las entradas del main
    int cValue=-1;
    char *mValue = NULL;
    int nValue=-1;
    bFlag = 0;
    int hValue = 0;
    int tValue = 0;

    opterr = 0;
    int flag;
    while ((flag = getopt(argc, argv, "c:m:n:h:t:b")) != -1){
        switch (flag){
            case 'c':
                cValue = atoi(optarg);
                cantIma = cValue;
                if(cValue < 1){
                    fprintf(stderr, "La bandera -c debe tener de argumento un número entero positivo.\n");
                    abort();
                }
                break;
            case 'm':
                mValue = optarg;
                int mValueLength = strlen(mValue);
                if(mValue[mValueLength-1] != 't' || mValue[mValueLength-2] != 'x' || mValue[mValueLength-3] != 't' || mValue[mValueLength-4] != '.'){
                    fprintf(stderr, "La bandera -m debe tener de argumento un archivo .txt.\n");
                    abort();
                }
                break;
            case 'n':
                nValue = atoi(optarg);
                if(nValue < 0 || nValue > 100){
                    fprintf(stderr, "La bandera -n debe tener de argumento un número entero entre 0 y 100.\n");
                    abort();
                }
                break;
            case 'b':
                bFlag = 1;
                break;
            case 't':
                tValue = atoi(optarg);
                tamanoB = tValue;
                if(tValue < 1){
                    fprintf(stderr, "La bandera -t debe tener de argumento un número entero mayor a 0. \n");
                    abort();
                }
                break;
            case 'h':
                hValue = atoi(optarg);
                cantHebras = hValue;
                if(hValue < 1){
                    fprintf(stderr, "La bandera -h debe tener de argumento un número entero mayor a 0. \n");
                    abort();
                }
                break;
            case '?':
                if(optopt == 'c'){
                    fprintf(stderr, "La bandera -%c requiere un argumento.\n", optopt);
                }
                else if (isprint(optopt)){
                    fprintf(stderr, "Bandera '-%c' desconocida.\n", optopt);
                }
                else{
                    fprintf (stderr, "Unknown option character '\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }
    //printf("cValue = %d, mValue = %s, nValue = %d, bFlag = %d, tValue = %d, hValue = %d\n", cValue, mValue, nValue, bFlag, tValue, hValue);
    int index;
    for (index = optind; index < argc; index++){
        printf("No hay bandera para el argumento %s\n", argv[index]);
    }
    FILE * file;
    if(cValue == -1){
        fprintf (stderr, "Se debe ingresar la cantidad de imágenes usando la bandera -c\n");
        abort();
    }
    else if(mValue == NULL){
        fprintf (stderr, "Se debe ingresar el nombre del archivo de la máscara del filtro usando la bandera -m\n");
        abort();
    }
    else if(nValue == -1){
        fprintf (stderr, "Se debe ingresar el umbral para la clasificación\n");
        abort();
    }
    else if( (file = fopen (mValue, "r")) == NULL){
        fprintf (stderr, "El archivo %s no existe\n", mValue);
        fclose(file);
        abort();
    }
    else{
        pipeline(cValue, mValue, nValue, hValue, tValue, bFlag);
    }
    return 0;
}