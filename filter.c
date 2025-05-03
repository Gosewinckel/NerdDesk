#include <stdio.h>
#include <stdlib.h>
#include "filters.h"


int main(void) {
	//get images
	FILE *images[4];
	images[0] = fopen("images/300px-Unsettled_Tom.bmp", "r");
	images[1] = fopen("images/Altair_8800_Computer.bmp", "r");
	images[2] = fopen("images/dune-passover-hed-3.bmp", "r");
	images[3] = fopen("images/rick-astley-600-d31de1cb3fb248e6b714e5fa789b104d.bmp", "r");
	for(int i = 0; i < 4; i++) {
		if(images[i] == NULL) {
			for(int j = 0; j <= i; j++) {
				fclose(images[j]);
			}
			printf("Could not open file %d\n", i);
			return 1;
		}
	}

	//make space for output files
	FILE *edits[4];
	edits[0] = fopen("tom.bmp", "w");
	edits[1] = fopen("alt.bmp", "w");
	edits[2] = fopen("paul.bmp", "w");
	edits[3] = fopen("rick.bmp", "w");
	for(int i = 0; i < 4; i++) {
		if(edits[i] == NULL) {
			for(int j = 0; j <= i; j++) {
				fclose(edits[j]);
			}
			printf("Cloud not allocate space\n");
			return 2;
		}
	}
	
	for(int i = 0; i < 4; i++) {
		//read file header
		BITMAPFILEHEADER bf;
		fread(&bf, sizeof(BITMAPFILEHEADER), 1, images[i]);
		
		//read info header
		BITMAPINFOHEADER bi;
		fread(&bi, sizeof(BITMAPINFOHEADER), 1, images[i]);
		
		//ensure file is of the correct type
		if(bf.bfType != 0x4d42 || bi.biBitCount != 24 || bi.biCompression != 0) {
			printf("%d\n", i);
			printf("Type: %d, OffBits: %d, Size: %d, BitCount: %d, Compression: %d\n", bf.bfType, bf.bfOffBits, bi.biSize, bi.biBitCount, bi.biCompression);
			for(int x = 0; x < 4; x++) {
				fclose(images[x]);
				fclose(edits[x]);
			}
			return 3;
		}
		//image dimensions
		int height = abs(bi.biHeight);
		int width  = bi.biWidth;
		
		//Allocote mem for image
		RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
		if(image == NULL) {
			printf("Not enough memory to store image\n");
			for(int x = 0; x < 4; x++) {
				fclose(images[x]);
				fclose(edits[x]);
			}
			return 4;
		}
		
		int padding = (4 - (width * sizeof(RGBTRIPLE))%4)%4;
		printf("%d/n", bi.biBitCount);

		//move through file lines
		for(int j = 0; j < height; j++) {
			fread(image[j], sizeof(RGBTRIPLE), width, images[i]);
			fseek(images[i], padding, SEEK_CUR);
		}

		//apply filters to image in order 
		edges(height, width, image);
		blur(height, width, image);
		//greyscale(height, width, image);
		reflect(height, width, image);
		
		fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, edits[i]);
		fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, edits[i]);

		for(int j = 0; j < height; j++) {
			fwrite(image[j], sizeof(RGBTRIPLE), width, edits[i]);
			for(int k = 0; k < padding; k++) {
				fputc(0x00, edits[i]);
			}
		}
		free(image);
		fclose(images[i]);
		fclose(edits[i]);
	}
	return 0;
}
