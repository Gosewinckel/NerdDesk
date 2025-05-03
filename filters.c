#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "filters.h"

void greyscale(int height, int width, RGBTRIPLE image[height][width]) {
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			uint8_t avg = 0;
			avg = avg + (image[i][j].rgbtRed + image[i][j].rgbtBlue + image[i][j].rgbtGreen)/3;
			image[i][j].rgbtRed = avg;
			image[i][j].rgbtBlue = avg;
			image[i][j].rgbtGreen = avg;
		}
	}
	return;
}

void reflect(int height, int width, RGBTRIPLE image[height][width]) {
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width/2; j++) {
			RGBTRIPLE temp = image[i][j];
			image[i][j] = image[i][width - (j + 1)];
			image[i][width - (j + 1)] = temp;
		}
	}
	return;
}

void blur(int height, int width, RGBTRIPLE image[height][width]) {
	return;
}

void edges(int height, int width, RGBTRIPLE image[height][width]) {
	return;
}
