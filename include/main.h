#ifndef _MAIN_H
#define _MAIN_H

//Structures
typedef struct pixelMatrix{
	int n;
	int m;
	int matrix[600][600];
}pixelMatrix;

typedef struct floatPixelMatrix{
	int nearlyBlack;
	int n;
	int m;
	float matrix[600][600];
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

//Functions
void pipeline(int cValue, char * mValue, int nValue, int hValue, int tValue, int bFlag);
pixelMatrix pngRead(char * fileName);
floatPixelMatrix convolution(kernelMatrix kernel, pixelMatrix pixels);
void resultsWriter(floatPixelMatrix floatPixels, char * fileName,int bFlag, int countImage);
floatPixelMatrix rectification(floatPixelMatrix floatPixels);
floatPixelMatrix pooling(floatPixelMatrix floatPixels);
floatPixelMatrix classifier(floatPixelMatrix floatPixels, int nValue);
pixel_t * pixel_at (bitmap_t * bitmap, int x, int y);
int save_png_to_file (bitmap_t *bitmap, const char *path);
int writeImage(floatPixelMatrix matrizPix, char * fileOut);
void preparation(char * mValue);

#endif