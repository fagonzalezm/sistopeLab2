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
        printf("\n\ncontIma: %d\n\n\n", contIma);
        
        strcpy(fileName, "imagen_");
        sprintf(index2,"%d",contIma+1);
        strcat(fileName,index2);
        pixels = pngRead(fileName);
        printf("\n\nFile: %s ", fileName);
        printf("(%d,%d)\n\n",pixels.m,pixels.n);
        for(int p = 0; p<pixels.m; p++){
            for(int q = 0; q<pixels.n; q++){
                printf("%3d",pixels.matrix[p][q]);
            }
            printf("\n");
        }
        
        
        cantFilas = pixels.m;
        cantCol = pixels.n;
        filasPorHebra = cantFilas/cantHebras;
        filasHebraFinal = filasPorHebra + (cantFilas - (filasPorHebra * cantHebras));
        printf("\n\n\nfilasPorHebra: %d\n",filasPorHebra);
        printf("filasHebraFinal: %d\n\n\n", filasHebraFinal);

        int i;
        for(i=0;i<pixels.m;i++){ //Cantidad de filas
            if(entra == (tamanoB - 1)){
                printf("buffer llenito\n");
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
            printf("soy prod y ya pasaron todas las hebras cons\n");
        }else if(auxHebra < cantHebras){
            pthread_mutex_unlock(&c);
        }else{

        }
        printf("terminamos un ciclo while\n");
        pthread_mutex_lock(&wh);
        printf("pasamos el lock w\n");
    }
    if (cantIma == contIma){
        printf("fuera del while prod\n");
        }
    return NULL;
}


//Entrada: (void* param) Entrada de las hebras consumidoras, no se usa.
//Funcionamiento: Funciona como una hebra consumidora. Recibe las filas de la matriz de la imagen, entregadas por la hebra productora. Luego, realiza el pipeline
//Salida: (void*) Escribe el resultado en disco y, de ser necesario, muestra por pantalla el resultado
void* consum(void* param){
    while(contIma < cantIma){
        pthread_mutex_lock(&c);
        printf("cruzado loc c\n");
        printf("cantIma%d\n", cantIma );
        contH++;
        auxHebra++;
        int cor;
        int id;
        id = contH;
        if(contH == cantHebras){
            printf("last hebra\n");
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
                    printf("finished\n");
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
        printf("Hebra: %d\n", contH);
        printf("Cor: %d\n", cor);
        for(z=0;z<cor;z++){
            for(w=0;w<cantCol;w++){
                printf("%3d", matrizAux[z][w]);
            }
            printf("\n");
        }
        turno++;

        if(contH == cantHebras){
            printf("termino la ejecucion de todas las hebras cons\n");
            contIma = contIma + 1;
        }else{
            printf("abrimos paso a la sigueinte cons\n");
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
        for(z=0;z<cor;z++){
            for(w=0;w<cantCol;w++){
                printf("%3d", (int)(localFloatPixel->matrix)[z][w]);
            }
            printf("\n");
        }

        pthread_barrier_wait(&barrera);


        //RECTIFICATION
        localFloatPixel = rectification(localFloatPixel);
        for(z=0;z<cor;z++){
            for(w=0;w<cantCol;w++){
                printf("%3d", (int)(localFloatPixel->matrix)[z][w]);
            }
            printf("\n");
        }

        
        pthread_barrier_wait(&barrera);
        
        pthread_mutex_lock(&et2);

        //POOLING
        localFloatPixel = pooling(localFloatPixel);
        for(z=0;z<localFloatPixel->m;z++){
            for(w=0;w<localFloatPixel->n;w++){
                printf("%3d", (int)(localFloatPixel->matrix)[z][w]);
            }
            printf("\n");
        }
        pthread_mutex_unlock(&et2);

        pthread_barrier_wait(&barrera);


        //CLASSIFIER
        pthread_mutex_lock(&calculo);
        colAct = localFloatPixel->n;  //columnas de la hebra despues de pool
        filAct = filAct + localFloatPixel->m;  //filas de la hebra despues de pool
        pthread_mutex_unlock(&calculo);
        printf("calculacion de los tamanos actuales\n");
        pthread_barrier_wait(&barrera);
        printf("ya calculamos los tamanos actuales\n");
        pthread_mutex_lock(&asig);
            if(hebraMem == 1){
                hebraMem = 0;
                hebraRes = id;
                printf("hebra mem\n");
                //Asignacion de memoria para la matriz imagen resultante
                resultante.matrix = (float**) malloc((sizeof(float)) * filAct);
                resultante.m = filAct;
                resultante.n = colAct;
            }

        pthread_mutex_unlock(&asig);
        printf("antes de while\n");
        printf("%d\n", id);
        printf("turno hebra: %d\n", turnoHebra);
        while (turnoHebra != id);
        pthread_mutex_lock(&escri);
        turnoHebra++;
        int fila;
        //printf("cantidad filas: %d\n", localFloatPixel->m);
        for(fila = 0; fila < (localFloatPixel->m); fila++){
            //printf("fila escritura: %d\n", fila);
            //printf("posEsc: %d\n", posEsc);
            resultante.matrix[posEsc] = localFloatPixel->matrix[fila];
            posEsc++; 
        }
        pthread_mutex_unlock(&escri);
        pthread_barrier_wait(&barrera);
        pthread_mutex_lock(&clas);
        if(hebraRes == id){
            ultima = 0;
            resultante = classifier(resultante, umbralC);
            printf("\n\nCLASSIFIER: ");
            printf("(%d,%d)\n\n",resultante.m,resultante.n);
            for(int p = 0; p<resultante.m; p++){
                for(int q = 0; q<resultante.n; q++){
                    printf("%3d",(int) resultante.matrix[p][q]);
                }
                printf("\n");
            }
            printf("casi escribo\n");
            strcpy(fileName, "out_");
            sprintf(index2,"%d",contIma);
            strcat(fileName,index2);
            resultsWriter(resultante, fileName, bFlag, contIma);
            printf("\n\nRESULTSWRITER: ");
            printf("(%d,%d)\n\n",resultante.m,resultante.n);
            for(int p = 0; p<resultante.m; p++){
                for(int q = 0; q<resultante.n; q++){
                    printf("%3d",(int) resultante.matrix[p][q]);
                }
                printf("\n");
            }
        }
        pthread_mutex_unlock(&clas);

        pthread_barrier_wait(&barrera);
        if (hebraRes == id){
            free(resultante.matrix);
        }
        free(pixelAux);
        free(matrizAux);
        free(floatMatrizAux);
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
            
            printf("bar\n");
            pthread_mutex_unlock(&wh);
            printf("wh desbloqueado\n");
            pthread_mutex_unlock(&p);
            printf("desbloqueaos p\n");
        }
        
    }
    if (cantIma == contIma){
        printf("fuera del while cons\n");
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
        printf("cantHebras; %d\n", cantHebras);

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
        //buffer= (int**)malloc((sizeof(int*))*tamanoB);

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