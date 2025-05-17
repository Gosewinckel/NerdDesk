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

//gaussian blur function
void blur(int height, int width, RGBTRIPLE image[height][width]) {
	// Create copy of image
	RGBTRIPLE buffer[height][width];
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			buffer[i][j] = image[i][j];
		}
	}

	// Create Gaussian kernel
	int radius = 3;
	float sigma = 0.8;
	float kernel[radius*2 + 1];
	float normal_const = 0;
	for(int i = 0; i < radius*2 + 1; i++) {
		kernel[i] = (1/sqrt(2*pow(sigma, 2)))*exp(-pow(i - radius, 2)/2*pow(sigma, 2));
		normal_const += kernel[i];
	}
	for(int i = 0; i < radius*2 + 1; i++) {
		kernel[i] /= normal_const*2;
	}
	// Use kernel as convolution matrix, go though each pixel(except those at the edge, this will be processed later) and multiply the 
	// corresponding surrounding matrix by the kernel values. The values returned will be added together and normalised giving our new pixel value
	for(int i = radius; i < height - radius; i++) {
		for(int j = radius; j < width - radius; j++) {
			float r_temp = 0;
			float g_temp = 0;
			float b_temp = 0;
			for(int x = -radius; x <= radius; x++) {
				r_temp += image[i][j - x].rgbtRed * kernel[x + radius];
				b_temp += image[i][j - x].rgbtBlue * kernel[x + radius];
				g_temp += image[i][j - x].rgbtGreen * kernel[x + radius];
			}
			for(int y = -radius; y <= radius; y++) {
				r_temp += image[i - y][j].rgbtRed * kernel[y + radius];
				b_temp += image[i - y][j].rgbtBlue * kernel[y + radius];
				g_temp += image[i - y][j].rgbtGreen * kernel[y + radius];
			}
			//set buffer to temp vals
			buffer[i][j].rgbtRed = round(r_temp);
			buffer[i][j].rgbtBlue = round(b_temp);
			buffer[i][j].rgbtGreen = round(g_temp);
		}
	}

	//process edge of image
	float r_temp1, r_temp2 = 0;
	float b_temp1, b_temp2 = 0;
	float g_temp1, g_temp2 = 0;
	//process top and bottom edges
	//first process left and right
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < radius; j++) {
			r_temp1 = r_temp2 = 0;
			b_temp1 = b_temp2 = 0;
			g_temp1 = g_temp2 = 0;
			for(int x = -radius; x <= radius; x++) {
				if(j - x < 0) {
					//left side
					r_temp1 += image[i][0].rgbtRed * kernel[x + radius];
					b_temp1 += image[i][0].rgbtBlue * kernel[x + radius];
					g_temp1 += image[i][0].rgbtGreen * kernel[x + radius];
					//right side
					r_temp2 += image[i][width - 1].rgbtRed * kernel[x + radius];
					b_temp2 += image[i][width - 1].rgbtBlue * kernel[x + radius];
					g_temp2 += image[i][width - 1].rgbtGreen * kernel[x + radius];
				}
				else {
					//left side
					r_temp1 += image[i][j - x].rgbtRed * kernel[x + radius];
					b_temp1 += image[i][j - x].rgbtBlue * kernel[x + radius];
					g_temp1 += image[i][j - x].rgbtGreen * kernel[x + radius];
					//right side
					r_temp2 += image[i][width - 1 - j - x].rgbtRed * kernel[x + radius];
					b_temp2 += image[i][width - 1 - j - x].rgbtBlue * kernel[x + radius];
					g_temp2 += image[i][width - 1 - j - x].rgbtGreen * kernel[x + radius];
				}
			}
			for(int y = -radius; y <= radius; y++) {
				if(i - y < 0) {
					//left side
					r_temp1 += image[0][j].rgbtRed * kernel[y + radius];
					b_temp1 += image[0][j].rgbtBlue * kernel[y + radius];
					g_temp1 += image[0][j].rgbtGreen * kernel[y + radius];
					//right side
					r_temp2 += image[0][width - 1 - j].rgbtRed * kernel[y + radius];
					b_temp2 += image[0][width - 1 - j].rgbtBlue * kernel[y + radius];
					g_temp2 += image[0][width - 1 - j].rgbtGreen * kernel[y + radius];
				}
				else {
					//left side
					r_temp1 += image[i - y][j].rgbtRed * kernel[y + radius];
					b_temp1 += image[i - y][j].rgbtBlue * kernel[y + radius];
					g_temp1 += image[i - y][j].rgbtGreen * kernel[y + radius];
					//right side
					r_temp2 += image[i - y][width - 1 - j].rgbtRed * kernel[y + radius];
					b_temp2 += image[i - y][width - 1 - j].rgbtBlue * kernel[y + radius];
					g_temp2 += image[i - y][width - 1 - j].rgbtGreen * kernel[y + radius];
				}
			} 
			buffer[i][j].rgbtRed = round(r_temp1);
			buffer[i][j].rgbtBlue = round(b_temp1);
			buffer[i][j].rgbtGreen = round(g_temp1);
			buffer[i][width - 1 - j].rgbtRed = round(r_temp2);
			buffer[i][width - 1 - j].rgbtBlue = round(b_temp2);
			buffer[i][width - 1 - j].rgbtGreen = round(g_temp2);
		}
	} 
	 for(int i = 0; i < radius; i++) {
		for(int j = radius; j < width - radius; j++) {
			r_temp1 = r_temp2 = 0;
			b_temp1 = b_temp2 = 0;
			g_temp1 = g_temp2 = 0;
			for(int x = -radius; x < radius; x++) {
				//left side
				r_temp1 += image[i][j - x].rgbtRed * kernel[x + radius];
				b_temp1 += image[i][j - x].rgbtBlue * kernel[x + radius];
				g_temp1 += image[i][j - x].rgbtGreen * kernel[x + radius];
				//right side
				r_temp2 += image[height - 1 - i][j - x].rgbtRed * kernel[x + radius];
				b_temp2 += image[height - 1 - i][j - x].rgbtBlue * kernel[x + radius];
				g_temp2 += image[height - 1 - i][j - x].rgbtGreen * kernel[x + radius];
			}
			for(int y = -radius; y < radius; y++) {
				if(i - y < 0) {
					//left
					r_temp1 += image[0][j].rgbtRed * kernel[y + radius];
					b_temp1 += image[0][j].rgbtBlue * kernel[y + radius];
					g_temp1 += image[0][j].rgbtGreen * kernel[y + radius];
					//right
					r_temp2 += image[height - 1][j].rgbtRed * kernel[y + radius];
					b_temp2 += image[height - 1][j].rgbtBlue * kernel[y + radius];
					g_temp2 += image[height - 1][j].rgbtGreen * kernel[y + radius];
				}
				else {
					//left
					r_temp1 += image[i - y][j].rgbtRed * kernel[y + radius];
					b_temp1 += image[i - y][j].rgbtBlue * kernel[y + radius];
					g_temp1 += image[i - y][j].rgbtGreen * kernel[y + radius];
					//right
					r_temp2 += image[height - 1 - i - y][j].rgbtRed * kernel[y + radius];
					b_temp2 += image[height - 1 - i - y][j].rgbtBlue * kernel[y + radius];
					g_temp2 += image[height - 1 - i - y][j].rgbtGreen * kernel[y + radius];
				}
			}
			buffer[i][j].rgbtRed = round(r_temp1);
			buffer[i][j].rgbtBlue = round(b_temp1);
			buffer[i][j].rgbtGreen = round(g_temp1);
			buffer[height - 1 - i][j].rgbtRed = round(r_temp2);
			buffer[height - 1 - i][j].rgbtBlue = round(b_temp2);
			buffer[height - 1 - i][j].rgbtGreen = round(g_temp2);
		}
	 } 
	
	//change pixels in original image
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			RGBTRIPLE *temp = &image[i][j];
			*temp = buffer[i][j];
		}
	}
	return;
}

void edges(int height, int width, RGBTRIPLE image[height][width]) {
	RGBTRIPLE buffer[width][height];
	float r_tempx, r_tempy = 0;
	float b_tempx, b_tempy = 0;
	float g_tempx, g_tempy = 0;
	for(int i = 1; i < height - 2; i++) {
		for(int j = 1; j < width - 2; j++) {
			r_tempx = r_tempy = 0;
			b_tempx = b_tempy = 0;
			g_tempx = g_tempy = 0;
			if(r_tempx != 0) {
				printf("%f\n", r_tempx);
			}
			//Gx
			for(int y = i - 1; y <= i + 1; y++) {
				r_tempx += (image[y][j - 1].rgbtRed * (-2 + abs(i - y))) + (image[y][j + 1].rgbtRed * (2 - abs(i - y)));
				b_tempx += image[y][j - 1].rgbtBlue * (-2 + abs(i - y)) + image[y][j + 1].rgbtBlue * (2 - abs(i - y));
				g_tempx += image[y][j - 1].rgbtGreen * (-2 + abs(i - y)) + image[y][j + 1].rgbtGreen * (2 - abs(i - y));
			}
			//Gy
			for(int x = j - 1; x <= j + 1; x++) {
				r_tempy += image[i - 1][x].rgbtRed * (-2 + abs(j - x)) + image[i + 1][x].rgbtRed * (2 - abs(j - x));
				b_tempy += image[i - 1][x].rgbtBlue * (-2 + abs(j - x)) + image[i + 1][x].rgbtBlue * (2 - abs(j - x));
				g_tempy += image[i - 1][x].rgbtGreen * (-2 + abs(j - x)) + image[i + 1][x].rgbtGreen * (2 - abs(j - x));
			}
			//calculate final vals
			int r_final = round(sqrt(pow(r_tempx, 2) + pow(r_tempy, 2)));
			int b_final = round(sqrt(pow(b_tempx, 2) + pow(b_tempy, 2)));
			int g_final = round(sqrt(pow(g_tempx, 2) + pow(g_tempy, 2)));
			if(r_final > 255) {
				r_final = 255;
			}
			if(b_final > 255) {
				b_final = 255;
			}
			if(g_final > 255) {
				g_final = 255;
			}
			buffer[i][j].rgbtRed = r_final;
			buffer[i][j].rgbtBlue = b_final;
			buffer[i][j].rgbtGreen = g_final;
		}
	}

	//set image equal to buffer
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			image[i][j] = buffer[i][j];
		}
	}
}
