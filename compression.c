#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "LZW.h"

struct file_node {
	char *name;
	struct file_node *next;
}; typedef struct file_node file_node;
	
file_node* make_node(char *data) {
	file_node *new_node = (file_node *)malloc(sizeof(file_node));
	if(new_node == NULL) {
		printf("malloc failed\n");
		return NULL;
	}
	new_node->name = strdup(data);
	return new_node;
}

//compression setup
int compression(void) {
	//linked list for file names
	file_node *head = NULL;	
	//find directory and start pulling file names
	const char *path = "./";
	struct dirent *entry;
	DIR *dir = opendir(path);
	//count is number of nodes
	int count = 0;
	if(dir == NULL) {
		return 1;
	}
	//copy file names into a linked list
	while((entry = readdir(dir)) != NULL) {
		//find and save only .bmp files to ll
		int size = strlen(entry->d_name);
		char sub[5];
		strncpy(sub, entry->d_name + size - 4, 4);
		sub[4] = '\0';
		if(strcmp(sub, ".bmp") == 0) {
			file_node *temp = make_node(entry->d_name);
			temp->next = head;
			head = temp;
			count++;
		}
	}
	closedir(dir);

	//make array of .bmp files
	FILE *images[count];
	char *names[count];
	file_node *temp = head;
	for(int i = 0; i < count; i++) {
		images[i] = fopen(temp->name, "r");
		if(images[i] == NULL) {
			printf("could not open file\n");
			return 2;
		}
		names[i] = temp->name;
		temp = temp->next;
	}
	for(int i = 0; i < count; i++) {
		BITMAPFILEHEADER bf;
		fread(&bf, sizeof(BITMAPFILEHEADER), 1, images[i]);

		BITMAPINFOHEADER bi;
		fread(&bi, sizeof(BITMAPINFOHEADER), 1, images[i]);

		int height = abs(bi.biHeight);
		int width = bi.biWidth;

		RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
		if(image[width] == NULL) {
			for(int j = 0; j < count; j++) {
				fclose(images[j]);
			}	
			free(image);
			return 3;
		}


		int padding = (4 - (width * sizeof(RGBTRIPLE))%4)%4;
		//move through file lines
		for(int j = 0; j < height; j++) {
			fread(image[j], sizeof(RGBTRIPLE), width, images[i]);
			fseek(images[i], padding, SEEK_CUR);
		}
	
		//compression algorythm
		//must free memory returned by LZW
		LZW(images[i], names[i]);

		fclose(images[i]);
		FILE *temp = fopen(names[i], "w");
		if(temp == NULL) {
			printf("could not open temp file\n");
			for(int j = 0; j <= i; j++) {
				fclose(images[j]);
			}
			free(image);
			return 4;
		}
		fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, temp);
		fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, temp);

		for(int j = 0; j < height; j++) {
			fwrite(image[j], sizeof(RGBTRIPLE), width, temp);
			for(int k = 0; k < padding; k++) {
				fputc(0x00, temp);
			}
		}
		free(image);
		fclose(temp);
	}
	return 0;
}
