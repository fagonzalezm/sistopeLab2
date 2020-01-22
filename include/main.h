#ifndef _MAIN_H
#define _MAIN_H
#include <pthread.h>

//Structures
typedef struct pixelMatrix{
	int id;
	int n;
	int m;
	int matrix[600][600];
}pixelMatrixImage;

typedef struct pixelMatrix2{
	int id;
	int n;
	int m;
	int ** matrix;
}pixelMatrix;

typedef struct pixelMatrixThread{
	int n;
	int m;
	int matrix[10][600];
}pixelMatrixThread;

typedef struct floatPixelMatrix{
	int id;
	int nearlyBlack;
	int n;
	int m;
	float ** matrix;
}floatPixelMatrix;

typedef struct kernelMatrix{
	int matrix[3][3];
}kernelMatrix;


typedef struct
{
    u_int8_t color;
}
pixel_t;
    
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;

//Shared variables
kernelMatrix kernel; //Read only
int classificationCounter;
//Shared variables threads
pthread_barrier_t barrera;
/*pthread_barrier_t barrera2;
pthread_barrier_t barrera3;
pthread_barrier_t barrera4;
pthread_barrier_t barrera5;*/
pthread_mutex_t c;
pthread_mutex_t p;
pthread_mutex_t l;
pthread_mutex_t wh;
pthread_mutex_t et2;
pthread_mutex_t escri;
pthread_mutex_t clas;
pthread_mutex_t calculo;
pthread_mutex_t asig;
int cantHebras;
int tamanoB;
int cantIma;
int contIma;
int** buffer;
int bar;
int entra;
int sale;
int ultima;
int** matrizprueba;
int filasHebraFinal;
int filasPorHebra;
int hebraRes;
//pixelMatrixThread * matrizAux; 
int contH;
int vez;
int finish;
int cantFilas;
int cantCol;
pixelMatrixImage pixels;
char fileName[20];
char index2[14];
int bFlag;
int turno;
int auxHebra;
int colAct;
int filAct;
int hebraMem;
int turnoHebra;
int posEsc;
int umbralC;
floatPixelMatrix * resultante;


//Functions
void pipeline(int cValue, char * mValue, int nValue, int hValue, int tValue, int bFlag);
pixelMatrixImage pngRead(char * fileName);
floatPixelMatrix * convolution(kernelMatrix kernel, int ** pixels, int cantCol, int cantFil, int id);
void resultsWriter(floatPixelMatrix * floatPixels, char * fileName,int bFlag, int countImage);
floatPixelMatrix * rectification(floatPixelMatrix * floatPixels);
floatPixelMatrix * pooling(floatPixelMatrix * floatPixels);
floatPixelMatrix * classifier(floatPixelMatrix * floatPixels, int nValue);
pixel_t * pixel_at (bitmap_t * bitmap, int x, int y);
int save_png_to_file (bitmap_t *bitmap, const char *path);
int writeImage(floatPixelMatrix * matrizPix, char * fileOut);
void preparation(char * mValue);

#endif