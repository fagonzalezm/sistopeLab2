#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "main.h"


//Entrada: (void* param) es la entrada que va a tener una hebra productora, no se usa
//Funcionamiento: Funciona como una hebra productora. Sirve para realizar la lectura de la imagen
//Salida: (void*) Le va entregando a las hebras consumidoras filas de la matriz de la imagen
void* prod(void* param){
    while(contIma < cantIma){
        pthread_mutex_lock(&p);
        bar = 1;
        turno = 0;
        vez = 1;
        
        strcpy(fileName, "imagen_");
        sprintf(index2,"%d",contIma+1);
        strcat(fileName,index2);
        pixels = pngRead(fileName);
        
        cantFilas = pixels.m;
        cantCol = pixels.n;
        filasPorHebra = cantFilas/cantHebras;
        filasHebraFinal = filasPorHebra + (cantFilas - (filasPorHebra * cantHebras));

        int i;
        for(i=0;i<pixels.m;i++){ //Cantidad de filas
            if(entra == (tamanoB - 1)){
                buffer[entra] = pixels.matrix[i];
                entra=0;
                sale = 0;
                if(vez == 1){
                    vez = 0;
                    pthread_mutex_unlock(&c);
                    pthread_mutex_lock(&p);
                }else if ((i+1) == pixels.m){ //Si es la ultima fila de la imagen
                    finish = 1;
                    pthread_mutex_unlock(&l);
                }else{
                    pthread_mutex_unlock(&l);
                    pthread_mutex_lock(&p);
                }
            }else if ((i+1) == pixels.m){
                entra = 0;
                sale=0;
                finish = 1;
                pthread_mutex_unlock(&l);
            }
            else{
                buffer[entra] =  pixels.matrix[i];
                entra++;
            }
        }

        if(auxHebra == cantHebras){
        }else if(auxHebra < cantHebras){
            pthread_mutex_unlock(&c);
        }else{

        }
        pthread_mutex_lock(&wh);
    }
    if (cantIma == contIma){
        }
    return NULL;
}


//Entrada: (void* param) Entrada de las hebras consumidoras, no se usa.
//Funcionamiento: Funciona como una hebra consumidora. Recibe las filas de la matriz de la imagen, entregadas por la hebra productora. Luego, realiza el pipeline
//Salida: (void*) Escribe el resultado en disco y, de ser necesario, muestra por pantalla el resultado
void* consum(void* param){
    while(contIma < cantIma){
        pthread_mutex_lock(&c);
        contH++;
        auxHebra++;
        int cor;
        int id;
        id = contH;
        if(contH == cantHebras){
            cor = filasHebraFinal;
        }else{
            cor = filasPorHebra;
        }
        pixelMatrix * pixelAux = (pixelMatrix*)malloc(sizeof(pixelMatrix)); 
        int** matrizAux= (int**)malloc((sizeof(int*)) * cor);
        pixelAux->matrix=matrizAux;
        pixelAux->id = id;
        float** floatMatrizAux= (float**)malloc((sizeof(float*)) * cor);
        int x=0;
        while(x<cor){
            if(sale == (tamanoB - 1)){
                matrizAux[x] = buffer[sale];
                if(finish == 1){
                    x = cor;
                }
                else{
                    pthread_mutex_unlock(&p);
                    pthread_mutex_lock(&l);
                }
            }
            else{
                matrizAux[x] = buffer[sale] ;
                sale++;
            }
            x++;
        }
        int z;
        int w;
        turno++;

        if(contH == cantHebras){
            contIma = contIma + 1;
        }else{
            pthread_mutex_unlock(&c);
        }
        pthread_barrier_wait(&barrera);

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////   PIPELINE    ////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////

        //CONVOLUTION
        floatPixelMatrix * localFloatPixel = convolution(kernel, matrizAux, cantCol, cor, id);
        localFloatPixel->m= cor;
        localFloatPixel->n = cantCol;

        pthread_barrier_wait(&barrera);


        //RECTIFICATION
        localFloatPixel = rectification(localFloatPixel);

        
        pthread_barrier_wait(&barrera);
        
        pthread_mutex_lock(&et2);

        //POOLING
        localFloatPixel = pooling(localFloatPixel);
        pthread_mutex_unlock(&et2);

        pthread_barrier_wait(&barrera);


        //CLASSIFIER
        pthread_mutex_lock(&calculo);
        colAct = localFloatPixel->n;  //columnas de la hebra despues de pool
        filAct = filAct + localFloatPixel->m;  //filas de la hebra despues de pool
        pthread_mutex_unlock(&calculo);
        pthread_barrier_wait(&barrera);
        pthread_mutex_lock(&asig);
            if(hebraMem == 1){
                hebraMem = 0;
                hebraRes = id;
                //Asignacion de memoria para la matriz imagen resultante
                //resultante.matrix = (float**) malloc((sizeof(float)) * filAct);
                resultante->m = filAct;
                resultante->n = colAct;
            }

        pthread_mutex_unlock(&asig);
        while (turnoHebra != id);
        pthread_mutex_lock(&escri);
        turnoHebra++;
        int fila;
        for(fila = 0; fila < (localFloatPixel->m); fila++){
            (resultante->matrix)[posEsc] = (localFloatPixel->matrix)[fila];
            posEsc++; 
        }
        pthread_mutex_unlock(&escri);
        pthread_barrier_wait(&barrera);
        pthread_mutex_lock(&clas);
        if(hebraRes == id){
            ultima = 0;
            resultante = classifier(resultante, umbralC);
            strcpy(fileName, "out_");
            sprintf(index2,"%d",contIma);
            strcat(fileName,index2);
            resultsWriter(resultante, fileName, bFlag, contIma);
        }
        pthread_mutex_unlock(&clas);

        pthread_barrier_wait(&barrera);
        if (hebraRes == id){
        }
        if((bar == 1) && (id == 1)){
            bar = 0;
            vez = 1;
            contH = 0;
            finish = 0;
            hebraMem = 1;
            ultima = 1;
            turnoHebra = 1;
            posEsc = 0;
            colAct = 0;  //columnas de la hebra despues de pool
            filAct = 0;
            
            pthread_mutex_unlock(&wh);
            pthread_mutex_unlock(&p);
        }
        
    }
    if (cantIma == contIma){
        }
    
    return NULL;
}

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
    resultante = (floatPixelMatrix *)malloc(sizeof(floatPixelMatrix *));
    resultante->matrix = (float**) malloc((sizeof(float*)) * 600);
    for(int p = 0; p<600; p++){
        (resultante->matrix)[p] = (float*) malloc((sizeof(float)) * 600);
    }

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
        cantIma = cValue;
        umbralC = nValue;
        preparation(mValue); //Se abre el archivo txt con el kernel y se crea el kernel
        tamanoB = tValue;
        buffer= (int**)malloc((sizeof(int*))*tamanoB);
        pthread_barrier_init(&barrera, NULL, cantHebras);

        pthread_mutex_init(&c,NULL);
        pthread_mutex_init(&p,NULL);
        pthread_mutex_init(&l,NULL);
        pthread_mutex_init(&wh,NULL);
        pthread_mutex_init(&et2,NULL);
        pthread_mutex_init(&escri,NULL);
        pthread_mutex_init(&clas,NULL);
        pthread_mutex_init(&calculo,NULL);
        pthread_mutex_init(&asig,NULL);

        pthread_mutex_lock(&c);
        pthread_mutex_lock(&l);
        pthread_mutex_lock(&wh);

        contIma=0;
        entra = 0;
        sale = 0;
        contH = 0;
        vez = 1;
        finish = 0;
        turno = 0;
        auxHebra = 0;
        turnoHebra = 1;
        hebraMem = 1;
        posEsc = 0;
        ultima = 1;
        hebraRes = 0;
        pthread_t produ;
        pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * cantHebras);

        pthread_create(&produ,NULL,prod,(void*) 1);

        int i;
        for(i=0;i<cantHebras;i++){
            pthread_create(&threads[i],NULL, consum, (void*) 1);
        }

        int j;
        for(j=0;j<cantHebras;j++){
            pthread_join(threads[j], NULL);
        }
        pthread_join(produ,NULL);
    }
    return 0;
}