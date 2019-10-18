#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <png.h>
#include <errno.h>
#include <stdarg.h>

#include "main.h"

//Entradas: int cValue: Entero positivo que indica la cantidad de imágenes a procesar
//          char* mValue: Nombre del archivo que contiene el filtro de la convolucion
//          int nValue: Entero positivo entre 0 y 100 que representa el umbral para definir si una imagen es nearlyblack o no
//          int bFlag: 0 o 1 que determina si se muestra en el terminal los resultados de la evaluación nearlyblack (bFlag = 1: muestra los resultados) 
//          int tValue: Entero mayor a 0 que representa la cantidad de hebras que se usaran para realizar al pipeline
//          int hValue: Entero mayor a 0 que representa el tamaño del buffer
//Funcionamiento: Primero prepara las entradas (argv) que requiere para el pipeline. Luego, 
//Salida: --
void pipeline(int cValue, char * mValue, int nValue, int hValue, int tValue, int bFlag){
}

//Entradas: char* filename: Nombre de una imagen en formato .png en escala de grises
//Funcionamiento: Lee una imagen y la guarda el largo, ancho y valor de sus pixeles en una matriz 
//Salida: estructura pixelMatrix que contiene largo, ancho y matriz de pixeles de la imagen
pixelMatrix pngRead(char * fileName){
	//definicion de variables
    png_structp	png_ptr;
    png_infop info_ptr;
    FILE * fp;
    png_uint_32 ancho;
    png_uint_32 alto;
    int profundidadBit;
    int tipoColor;
    int metEntrelaz;
    int metCompres;
    int metFiltro;
    int x;
    png_bytepp rows;
    pixelMatrix matrizPix;

    //apertura del archivo (imagen) para lectura en binario
    fp = fopen (fileName, "rb");
    if (! fp) {
		//printf("Error, el archivo no pudo ser abierto.\n");
    }
    //Se crea la estructura de lectura de PNG
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (! png_ptr) {
		//printf("No se pudo crear la estructura PNG de lectura.\n");
    }
    //Se crea la estructura de informacion de PNG
    info_ptr = png_create_info_struct (png_ptr);
    if (! png_ptr) {
		//printf("No se pudo crear la estructura PNG de informacion.\n");
    }
    //Se inicializan las funciones de entrada y salida predeterminadas para PNG 
    png_init_io (png_ptr, fp);
    //Se lee toda la imagen en memoria
    png_read_png (png_ptr, info_ptr, 0, 0);
    //Se obtinene la informacion desde el fragmento IHDR de la imagen PNG
    png_get_IHDR (png_ptr, info_ptr, & ancho, & alto, & profundidadBit,
		  & tipoColor, & metEntrelaz, & metCompres,
		  & metFiltro);
    //Se obtiene una matriz e punteros que apuntan a los datos de pixeles para cada fila.
    rows = png_get_rows (png_ptr, info_ptr);
    int rowbytes;
    //Se obtiene la cantidad de bytes necesarios para contener una fila de una imagen.
    rowbytes = png_get_rowbytes (png_ptr, info_ptr);

    matrizPix.m = alto;
    matrizPix.n = ancho;
    //Se recorren los punteros de la matriz pixel a pixel y se guardan en una estructura.
    for (x = 0; x < alto; x++) {
		int y;
		png_bytep row;
		row = rows[x];
      for (y = 0; y < ancho; y++) {
        png_byte pixel;
        pixel = row[y];
        //printf("%d", pixel);
        (matrizPix.matrix)[x][y] = (int) pixel;
        
      }
		//printf ("\n");
    }
    fclose(fp);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   	return matrizPix;
}

//Entradas:	kernelMatrix kernel:Representa el filtro
//			pixelMatrix pixels: Respresenta una imagen 
//Funcionamiento: Se recorre la matriz de los valores de los pixeles aplicando el concepto de convolucion haciendo uso de una matriz de numeros (kernel) para generar una nueva matriz normalizada.
//Salida: floatPixelMatrix que representa una matriz de flotantes que contiene los valores normalizados.
floatPixelMatrix convolution(kernelMatrix kernel, pixelMatrix pixels){
	floatPixelMatrix floatPixels;
	floatPixels.m = pixels.m;
	floatPixels.n = pixels.n;

	int fila;
	int columna;
	for(fila=0;fila<pixels.m;fila++){
		for(columna=0;columna<pixels.n;columna++){
			float resultado = 0;
			int s1 = 0;
			int s2 = 0;
			int s3 = 0;
			int s4 = 0;
			int s5 = 0;
			int s6 = 0;
			int s7 = 0;
			int s8 = 0;
			int s9 = 0;
			if((fila == 0) && (columna==0)){
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s6 = (pixels.matrix[fila][columna + 1] * (kernel.matrix)[1][2]);
				s8 = (pixels.matrix[fila + 1][columna] * (kernel.matrix)[2][1]);
				s9 = (pixels.matrix[fila +1][columna +1] * (kernel.matrix)[2][2]);
			}
			else if ((fila == (pixels.m - 1)) && (columna == 0)){
				s2 = (pixels.matrix[fila - 1][columna] * (kernel.matrix)[0][1]);
				s3 = (pixels.matrix[fila - 1][columna + 1] * (kernel.matrix)[0][2]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s6 = (pixels.matrix[fila][columna + 1] * (kernel.matrix)[1][2]);
			}
			else if((fila == 0) && (columna == (pixels.n - 1))){
				s4 = (pixels.matrix[fila][columna - 1] * (kernel.matrix)[1][0]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s7 = (pixels.matrix[fila + 1][columna - 1] * (kernel.matrix)[2][0]);
				s8 = (pixels.matrix[fila + 1][columna] * (kernel.matrix)[2][1]);
			}
			else if ((fila == (pixels.m - 1)) && (columna == (pixels.n - 1))){
				s1 = (pixels.matrix[fila - 1][columna - 1] * (kernel.matrix)[0][0]);
				s2 = (pixels.matrix[fila - 1][columna] * (kernel.matrix)[0][1]);
				s4 = (pixels.matrix[fila][columna - 1] * (kernel.matrix)[1][0]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
			}
			else if (columna == 0){
				s2 = (pixels.matrix[fila - 1][columna] * (kernel.matrix)[0][1]);
				s3 = (pixels.matrix[fila - 1][columna + 1] * (kernel.matrix)[0][2]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s6 = (pixels.matrix[fila][columna + 1] * (kernel.matrix)[1][2]);
				s8 = (pixels.matrix[fila + 1][columna] * (kernel.matrix)[2][1]);
				s9 = (pixels.matrix[fila + 1][columna + 1] * (kernel.matrix)[2][2]);
			}
			else if (fila == 0){
				s4 = (pixels.matrix[fila][columna - 1] * (kernel.matrix)[1][0]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s6 = (pixels.matrix[fila][columna + 1] * (kernel.matrix)[1][2]);
				s7 = (pixels.matrix[fila + 1][columna - 1] * (kernel.matrix)[2][0]);
				s8 = (pixels.matrix[fila + 1][columna] * (kernel.matrix)[2][1]);
				s9 = (pixels.matrix[fila + 1][columna + 1] * (kernel.matrix)[2][2]);
			}
			else if (columna == (pixels.n - 1)){
				s1 = (pixels.matrix[fila - 1][columna - 1] * (kernel.matrix)[0][0]);
				s2 = (pixels.matrix[fila - 1][columna] * (kernel.matrix)[0][1]);
				s4 = (pixels.matrix[fila][columna - 1] * (kernel.matrix)[1][0]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s7 = (pixels.matrix[fila + 1][columna - 1] * (kernel.matrix)[2][0]);
				s8 = (pixels.matrix[fila + 1][columna] * (kernel.matrix)[2][1]);
			}
			else if (fila == (pixels.m - 1)){
				s1 = (pixels.matrix[fila - 1][columna - 1] * (kernel.matrix)[0][0]);
				s2 = (pixels.matrix[fila - 1][columna] * (kernel.matrix)[0][1]);
				s3 = (pixels.matrix[fila - 1][columna + 1] * (kernel.matrix)[0][2]);
				s4 = (pixels.matrix[fila][columna - 1] * (kernel.matrix)[1][0]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s6 = (pixels.matrix[fila][columna + 1] * (kernel.matrix)[1][2]);
			}
			else {
				s1 = (pixels.matrix[fila - 1][columna - 1] * (kernel.matrix)[0][0]);
				s2 = (pixels.matrix[fila - 1][columna] * (kernel.matrix)[0][1]);
				s3 = (pixels.matrix[fila - 1][columna + 1] * (kernel.matrix)[0][2]);
				s4 = (pixels.matrix[fila][columna - 1] * (kernel.matrix)[1][0]);
				s5 = (pixels.matrix[fila][columna] * (kernel.matrix)[1][1]);
				s6 = (pixels.matrix[fila][columna + 1] * (kernel.matrix)[1][2]);
				s7 = (pixels.matrix[fila + 1][columna - 1] * (kernel.matrix)[2][0]);
				s8 = (pixels.matrix[fila + 1][columna] * (kernel.matrix)[2][1]);
				s9 = (pixels.matrix[fila + 1][columna + 1] * (kernel.matrix)[2][2]);
			}
			resultado = (s1 + s2 +s3 +s4 +s5 +s6 +s7 +s8 +s9);
			(floatPixels.matrix)[fila][columna] = resultado / 9.0;
		}
	}

	return floatPixels;
}

floatPixelMatrix classifier(floatPixelMatrix floatPixels, int nValue){
    int solution = 0;
    //Se define el valor especifico del umbral
    int size = floatPixels.m * floatPixels.n;
    int threshold = (int)((nValue/100.0)*size);
    //Se obtiene la cantidad de pixeles negros en la matriz de pixeles
    int count = 0;
    for(int i = 0; i < floatPixels.m; i++){
        for(int j = 0; j < floatPixels.n; j++){
            if(floatPixels.matrix[i][j] == 0.0){
                count = count + 1;
                //Si la cantidad de pixeles negros es igual al umbral se retorna 1 
                if(count >= threshold){
                    solution = 1;
                    break;
                }
            }
        }
    }
    //Se define el valor nearlyBlack de la imagen
    if(solution == 1){
        floatPixels.nearlyBlack = 1;
    }
    else{
        floatPixels.nearlyBlack = 0;
    }
    return floatPixels;
}

floatPixelMatrix pooling(floatPixelMatrix floatPixels){
    //Se adecua la matrix de pixeles agregando ceros abajo y a la derecha para que el tamaño de la matriz sea divisible por 3 
    if(floatPixels.m%3==1){
        for(int i = 0; i<floatPixels.n; i++){
            (floatPixels.matrix)[floatPixels.m+1][i]=0.0;
            (floatPixels.matrix)[floatPixels.m+2][i]=0.0;
        }
        floatPixels.m = floatPixels.m + 2;
    }
    else if(floatPixels.m%3==2){
        for(int i = 0; i<floatPixels.n; i++){
            (floatPixels.matrix)[floatPixels.m+1][i]=0.0;
        }
        floatPixels.m = floatPixels.m + 1;
    } //el error puede estar aqui
    else if(floatPixels.n%3==1){
        for(int i = 0; i<floatPixels.m; i++){
            (floatPixels.matrix)[i][floatPixels.n+1]=0.0;
            (floatPixels.matrix)[i][floatPixels.n+2]=0.0;
        }
        floatPixels.n = floatPixels.n + 2;
    }
    else if(floatPixels.n%3==2){
        for(int i = 0; i<floatPixels.m; i++){
            (floatPixels.matrix)[i][floatPixels.n+1]=0.0;
        }
        floatPixels.n = floatPixels.n + 1;
    }
    //Se recorre la matriz de pixeles de 9 en 9, definiendo el valor mayor dentro de ese conjunto
    for(int i = 0; i<(floatPixels.m/3); i++){
        for(int j = 0; j<(floatPixels.n/3); j++){
            float higher = (floatPixels.matrix)[3*i][3*j];
            if((floatPixels.matrix)[3*i+1][3*j] > higher){
                higher = (floatPixels.matrix)[3*i+1][3*j];
            }
            if((floatPixels.matrix)[3*i+2][3*j] > higher){
                higher = (floatPixels.matrix)[3*i+2][3*j];
            }
            if((floatPixels.matrix)[3*i][3*j+1] > higher){
                higher = (floatPixels.matrix)[3*i][3*j+1];
            }

            if((floatPixels.matrix)[3*i+1][3*j+1] > higher){
                higher = (floatPixels.matrix)[3*i+1][3*j+1];
            }

            if((floatPixels.matrix)[3*i+2][3*j+1] > higher){
                higher = (floatPixels.matrix)[3*i+2][3*j+1];
            }

            if((floatPixels.matrix)[3*i][3*j+2] > higher){
                higher = (floatPixels.matrix)[3*i][3*j+2];
            }
            if((floatPixels.matrix)[3*i+1][3*j+2] > higher){
                higher = (floatPixels.matrix)[3*i+1][3*j+2];
            }
            if((floatPixels.matrix)[3*i+2][3*j+2] > higher){
                higher = (floatPixels.matrix)[3*i+2][3*j+2];
            }
            //Se reemplaza el valor mayor en su lugar correspondiente
            (floatPixels.matrix)[i][j]=higher;
        }
    }
    //Se reduce el tamaño de la matriz
    floatPixels.m = (floatPixels.m)/3;
    floatPixels.n = (floatPixels.n)/3;
    return floatPixels;
}

floatPixelMatrix rectification(floatPixelMatrix floatPixels){
//Se eliminan los valores negativos
    for(int i = 0; i<floatPixels.m; i++){
        for(int j = 0; j<floatPixels.n; j++){
            if((floatPixels.matrix)[i][j]<0.0){
                (floatPixels.matrix)[i][j] = 0;
            }
        }
    }
    return floatPixels;
}

void resultsWriter(floatPixelMatrix floatPixels, char * fileName,int bFlag, int countImage){
    //Si se requiere mostrar la evaluacion de nearlyblack se muestra el resultado
    if(bFlag == 1){
        if(floatPixels.nearlyBlack==1){
            printf("|  imagen_%d |     yes      |\n",countImage+1);
        }
        else if(floatPixels.nearlyBlack==0){
            printf("|  imagen_%d |      no      |\n",countImage+1);
        }
    }
    //Se escribe el resultado del pipeline
    int status = writeImage(floatPixels, fileName);
}

//Entrada: Estructura bitmap_t que contiene los pixeles, ancho y alto de la imagen/ posicion en columnas/ posicion en filas.
//Funcionamiento: Extrae el pixel ubicado en la posicion (y,x).
//Salida: pixel.
static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

//Entrada: Estructura bitmap_t que contiene los pixeles, ancho y alto de la imagen/ string que contiene el nombre del archivo (imagen) de salida.
//Funcionamiento: Inicializa todas las estructuras presentes en un archivo PNG para posteriormente ser llenado con los valores 
//      presentes en la estructura bitmap_t.
//Salida: un entero que indica el estado de finalizacion de la función.  
static int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    //Definicion de variables.
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
   
    int status = -1;
    
    int pixel_size = 3;
    int depth = 8;
    //Apertura del archivo
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }
    //Inicializacion de las estructuras requeridas
    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    //Seteo del header del archivo PNG
    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_GRAY,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    //Recorrido y llenado de la matriz de pixeles con los datos de bitmap
    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (u_int8_t) * bitmap->width);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->color;
        }
    }
    
    //Inicializacion, configuracion y escritura de la imagen.
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    

    status = 0;
    //liberacion de la memoria utilizada en el proceso.
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

//Entrada: Matriz de flotantes que contienen los valores de los pixeles / String que indica el nombre de salida.
//Funcionamiento: Se prepara la estructura necesaria (bitmap_t) con los datos de la matriz de flotantes y se escribe la imagen.
//Salida: entero que indica el estado del resultado de la función.
int writeImage(floatPixelMatrix matrizPix, char * fileOut){
    bitmap_t pngOut;
    int xg;
    int yg;
    int status;
    status = 0;

    /* Se crean las struct */
    
    pngOut.width = matrizPix.n;
    pngOut.height = matrizPix.m;
    pngOut.pixels = calloc (pngOut.width * pngOut.height, sizeof (pixel_t));
    if (! pngOut.pixels) {
    return -1;
    }
    //Se recorre y crea una matriz con pixeles
    for (yg = 0; yg < pngOut.height; yg++) {
        for (xg = 0; xg < pngOut.width; xg++) {
            pixel_t * pixel = pixel_at (& pngOut, xg, yg);
            pixel->color = (int) matrizPix.matrix[yg][xg];
        }
    }

    /* Se escribe la imagen */

    if (save_png_to_file (& pngOut, fileOut)) {
        fprintf (stderr, "Error escribiendo archivo.\n");
        status = -1;
    }

    free (pngOut.pixels);
    return status;
}