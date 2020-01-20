#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <png.h>
#include <errno.h>
#include <stdarg.h>

pthread_barrier_t barrera;
pthread_mutex_t c;
pthread_mutex_t p;
pthread_mutex_t l;
int cantHebras;
int tamanoB;
int cantIma;
int contIma;
int** buffer;
int entra;
int sale;
int** matrizprueba;
int** matrizAux; 
int contH;
int vez;
int finish;


void* prod(void* param){
	//AQUI DEBERIA IR UN WHILE QUE SEA TIPO I<CANTIDAD DE IMAGENES QUE CUBRA TODA LA FUNCION
	pthread_mutex_lock(&p);
	int i;
	for(i=0;i<20;i++){ //Cantidad de filas
		if(entra == (tamanoB - 1)){
			printf("buffer llenito\n");
			buffer[entra] = matrizprueba[i];
			//rintf("Valor entra: %d\n", entra);
			entra=0;
			sale = 0;
			if(vez == 1){
				vez = 0;
				pthread_mutex_unlock(&c);
				pthread_mutex_lock(&p);
			}else if ((i+1) == 20){
				//printf("finalizando\n");
				finish = 1;
				pthread_mutex_unlock(&l);
			}else{
				pthread_mutex_unlock(&l);
				pthread_mutex_lock(&p);
			}
			//printf("%d\n", sale);
		}else if ((i+1) == 20){
				entra = 0;
				sale=0;
				//printf("finalizando\n");
				finish = 1;
				pthread_mutex_unlock(&l);
		}
		else{
			buffer[entra] = matrizprueba[i];
			//printf("entra: %d\n", entra);
			entra++;
		}
	}
	//printf("terminando pro\n");
	pthread_mutex_unlock(&c);
	//pthread_mutex_lock(&p);
	//printf("termino produ\n");
	return NULL;
}

//Consume la cantidad q le corresponde a la hebra, es la variable cor (de prueba puse 4)
void* consum(void* param){
	//AQUI DEBERIA IR UN WHILE QUE SEA TIPO I<CANTIDAD DE IMAGENES QUE CUBRA TODA LA FUNCION
	pthread_mutex_lock(&c);
	matrizAux = (int**) malloc((sizeof(int*)) * 4);
	int cor=4;
	int x;
	int j;
	for(j=0;j<cor;j++){
		matrizAux[j] = (int*)malloc((sizeof(int)) *20);
	}
	while(x<cor){
		if(sale == (tamanoB - 1)){
			matrizAux[x] = buffer[sale] ;
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
	contH++;
	int z;
	int w;
	printf("Hebra: %d\n", contH);
	for(z=0;z<cor;z++){
		for(w=0;w<20;w++){
			printf("%d | ", matrizAux[z][w]);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&c);
	//printf("termino consu\n");
	//AQUI HACIA ABAJO HAY Q AGREGAR LAS BARRERAS Y LAS ETAPAS 
	
	return NULL;
}


int main(){
	printf("Cant hebras\n" );
	scanf("%d",&cantHebras);
	printf("tamanio buffer\n" );
	scanf("%d",&tamanoB);
	printf("Cant imagenes\n" );
	scanf("%d",&cantIma);


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
	pthread_t produ;
	pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * cantHebras);
	buffer= (int**)malloc((sizeof(int*))*tamanoB);

	matrizprueba= (int**)malloc((sizeof(int*))*20);

	int f;
	for(f=0;f<20;f++){
		matrizprueba[f]=(int*)malloc((sizeof(int)) *20);
	}

	//MATRIZ PARA PROBRAR SOLAMENTE.
	int fj;
	int kj;
	for(fj=0;fj<20;fj++){
		for(kj=0;kj<20;kj++){
			if(fj == 14){
				matrizprueba[fj][kj]= 6;
			}
			else{
				matrizprueba[fj][kj]= 5;
			}
			
		}
	}

	//int init = pthread_barrier_init(&barrera, NULL, cantHebras);***********************************
	//printf("%d\n",init );


	pthread_create(&produ,NULL,prod,(void*) 1);//********************************************

	int i;
	for(i=0;i<cantHebras;i++){
		pthread_create(&threads[i],NULL, consum, (void*) 1);
	}
	int j;
	//pthread_barrier_destroy;
	for(j=0;j<cantHebras;j++){
		pthread_join(threads[j], NULL);
	}

	pthread_join(produ,NULL);
	
	return 1;
}