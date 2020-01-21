#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "main.h"



//REVISAR CONDICION CUANDO PASAMOS A LA SIGUIENTE IMAGEN
void* prod(void* param){
    //AQUI DEBERIA IR UN WHILE QUE SEA TIPO I<CANTIDAD DE IMAGENES QUE CUBRA TODA LA FUNCION
    while(contIma < cantIma){
        turno = 0;
        vez = 1;
        printf("\n\ncontIma: %d\n\n\n", contIma);
        pthread_mutex_lock(&p);
        //Pipeline
        //READ
        strcpy(fileName, "imagen_");
        sprintf(index2,"%d",contIma+1);
        strcat(fileName,index2);
        pixels = pngRead(fileName);
        printf("\n\nFile: %s ", fileName);
        printf("(%d,%d)\n\n",pixels.m,pixels.n);
        int g;
        int q;
        for(g = 0; g<pixels.m; g++){
            for(q = 0; q<pixels.n; q++){
                printf("%3d",pixels.matrix[g][q]);
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
                printf("HOOLA\n");
                buffer[entra] = pixels.matrix[i];
                //rintf("Valor entra: %d\n", entra);
                entra=0;
                sale = 0;
                if(vez == 1){
                    printf("CHAOO1\n");
                    vez = 0;
                    pthread_mutex_unlock(&c);
                    pthread_mutex_lock(&p);
                    printf("CHAOO1\n");
                }else if ((i+1) == pixels.m){ //Si es la ultima fila de la imagen
                    printf("CHAOO2\n");
                    //printf("finalizando\n");
                    finish = 1;
                    pthread_mutex_unlock(&l);
                    printf("CHAOO2\n");
                }else{
                    printf("CHAOO3\n");
                    pthread_mutex_unlock(&l);
                    pthread_mutex_lock(&p);
                    printf("CHAOO3\n");
                }
                //printf("%d\n", sale);
            }else if ((i+1) == pixels.m){
                entra = 0;
                sale=0;
                //printf("finalizando\n");
                finish = 1;
                pthread_mutex_unlock(&l);
            }
            else{
                buffer[entra] =  pixels.matrix[i];
                //printf("entra: %d\n", entra);
                entra++;
            }
        }
        printf("terminando pro\n");
        pthread_mutex_unlock(&c);
        //pthread_mutex_lock(&p);
        contIma = contIma + 1;
        while(turno != contH);
        //Aquí las consumidoras terminaron
        pthread_mutex_unlock(&p);
        
    }
    printf("termino produ\n");
    return NULL;
}



//Consume la cantidad q le corresponde a la hebra, es la variable cor (de prueba puse 4)
void* consum(void* param){
    //AQUI DEBERIA IR UN WHILE QUE SEA TIPO I<CANTIDAD DE IMAGENES QUE CUBRA TODA LA FUNCION
    while(contIma < cantIma){
        pthread_mutex_lock(&c);
        contH++;
        int cor;
        if(contH == cantHebras){
            cor = filasHebraFinal;
        }else{
            cor = filasPorHebra;
        }
        int** matrizAux= (int**)malloc((sizeof(int*)) * cor);
        int x;
        while(x<cor){
            if(sale == (tamanoB - 1)){
                matrizAux[x] = buffer[sale];
                //printf("sale: %d\n", sale);
                //printf("x: %d\n", x);
                if(finish == 1){
                    x = cor;
                    //printf("cor\n");
                }
                else{
                    pthread_mutex_unlock(&p);
                    pthread_mutex_lock(&l);
                }
                //while(sale != 0);
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
                printf("%d | ", matrizAux[z][w]);
            }
            printf("\n");
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
    
        //printf("termino consu\n");
        //AQUI HACIA ABAJO HAY Q AGREGAR LAS BARRERAS Y LAS ETAPAS
/*
        floatPixelMatrix floatPixels = convolution(kernel,*matrizAux);
        printf("\n\nCONVOLUTION: ");
        printf("(%d,%d)\n\n",floatPixels.m,floatPixels.n);
        for(int p = 0; p<floatPixels.m; p++){
            for(int q = 0; q<floatPixels.n; q++){
                printf("%3d",(int) floatPixels.matrix[p][q]);
            }
            printf("\n");
        }
        pthread_barrier_wait(&barrera1);

        //RECTIFICATION
        floatPixels = rectification(floatPixels);
        printf("\n\nRECTIFICATION: ");
        printf("(%d,%d)\n\n",floatPixels.m,floatPixels.n);
        for(int p = 0; p<floatPixels.m; p++){
            for(int q = 0; q<floatPixels.n; q++){
                printf("%3d",(int) floatPixels.matrix[p][q]);
            }
            printf("\n");
        }

        pthread_barrier_wait(&barrera2);

        //POOLING
        floatPixels = pooling(floatPixels);
        printf("\n\nPOOLING: ");
        printf("(%d,%d)\n\n",floatPixels.m,floatPixels.n);
        for(int p = 0; p<floatPixels.m; p++){
            for(int q = 0; q<floatPixels.n; q++){
                printf("%3d",(int) floatPixels.matrix[p][q]);
            }
            printf("\n");
        }

        pthread_barrier_wait(&barrera3);

        //CLASSIFIER
        floatPixels = classifier(floatPixels, nValue);
        printf("\n\nCLASSIFIER: ");
        printf("(%d,%d)\n\n",floatPixels.m,floatPixels.n);
        for(int p = 0; p<floatPixels.m; p++){
            for(int q = 0; q<floatPixels.n; q++){
                printf("%3d",(int) floatPixels.matrix[p][q]);
            }
            printf("\n");
        }

        pthread_barrier_wait(&barrera4);

        //RESULTSWRITER
        strcpy(fileName, "out_");
        sprintf(index2,"%d",i+1);
        strcat(fileName,index2);
        resultsWriter(floatPixels, fileName, bFlag, i);
        printf("\n\nRESULTSWRITER: ");
        printf("(%d,%d)\n\n",floatPixels.m,floatPixels.n);
        for(int p = 0; p<floatPixels.m; p++){
            for(int q = 0; q<floatPixels.n; q++){
                printf("%3d",(int) floatPixels.matrix[p][q]);
            }
            printf("\n");
        }

        pthread_barrier_wait(&barrera5);

    //pthread_barrier_destroy;*/
    ////pthread_barrier_wait(&barrera);
    turno++;
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
        cantIma = cValue;
        //contIma = 0;
        preparation(mValue); //Se abre el archivo txt con el kernel y se crea el kernel
        tamanoB = tValue;
        buffer= (int**)malloc((sizeof(int*))*tamanoB);
        //pipeline(cValue, mValue, nValue, hValue, tValue, bFlag);
        pthread_barrier_init(&barrera, NULL, cantHebras);
        printf("cantHebras; %d\n", cantHebras);
        /*pthread_barrier_init(&barrera2, NULL, cantHebras);
        pthread_barrier_init(&barrera3, NULL, cantHebras);
        pthread_barrier_init(&barrera4, NULL, cantHebras);
        pthread_barrier_init(&barrera5, NULL, cantHebras);*/

        pthread_mutex_init(&c,NULL);
        pthread_mutex_init(&p,NULL);
        pthread_mutex_init(&l,NULL);

        pthread_mutex_lock(&c);
        pthread_mutex_lock(&l);

        contIma=0;
        entra = 0;
        sale = 0;
        contH = 0;
        vez = 1;
        finish = 0;
        turno = 0;
        pthread_t produ;
        pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * cantHebras);
        buffer= (int**)malloc((sizeof(int*))*tamanoB);

        pthread_create(&produ,NULL,prod,(void*) 1);

        int i;
        for(i=0;i<cantHebras;i++){
            pthread_create(&threads[i],NULL, consum, (void*) 1);
        }

        int j;
        for(j=0;j<cantHebras;j++){
            pthread_join(threads[j], NULL);
        }
        printf("no final\n");
        pthread_join(produ,NULL);
        printf("final\n");
    }
    return 0;
}