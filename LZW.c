#include "LZW.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


struct dict {
	//key will be number of values in sequence
	int key;
	//string form of sequence of number vals
	char *val;
}; typedef struct dict dict_t;

int hash(int key) {
	//make sure the index starts at 0 not 1
	if(key == 1) {
		return 0;
	} 
	return pow(256, key - 1);
}

//return index to a dictionary entry
int search(int key, char *seq, dict_t dict[]) {
	//checks region sequence could be held
	for(int i = hash(key); dict[i].val != NULL; i++) {
		//return index of stored val
		if(strcmp(seq, dict[i].val) == 0) {
			return i;
		}
		if(dict[i].val == NULL) {
			return i;
		}
	}	
	//in case of first val
	return 0;
}

bool check(int idx, char *seq, dict_t dict[]) {
	if(strcmp(seq, dict[idx].val) == 0) {
		return true;
	}
	return false;
}

void insert(int idx, int key, char *seq, dict_t dict[]) {
	dict_t *entry =(dict_t*) malloc(sizeof(dict_t));
	entry->key = key;
	entry->val = seq;
	dict[idx] = *entry;
	return;
}

//recursive function that takes the entire input and checks inputs until it cant find a match
char *new_word();

//returns name of new file
const char *LZW(FILE *file, char *name) {
	//make new file "file.cbmp", this will be used to store compressed data
	int len = strlen(name) + 2;
	char *c_file_name = malloc(len);
	strcpy(c_file_name, name);
	strcpy(&c_file_name[len - 6], ".cbmp");
	printf("%s\n", c_file_name);
	FILE *temp = fopen(c_file_name, "w");

	BITMAPFILEHEADER bf;
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, file);

	BITMAPINFOHEADER bi;
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, file);

	int height = abs(bi.biHeight);
	int width = bi.biWidth;

	RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
	if(image == NULL) {
		fclose(temp);
		free(image);
		return NULL;
	}

	int padding = (4 - (width * sizeof(RGBTRIPLE))%4)%4;
	//move through file lines
	for(int j = 0; j < height; j++) {
		fread(image[j], sizeof(RGBTRIPLE), width, file);
		fseek(file, padding, SEEK_CUR);
	}

	//define initial dictionary
	int pixels = width * height * 3;
	int idx = 0;
	int total = 0;
	while(total < pixels) {
		idx++;
		total += pow(256, idx);
	}
	dict_t dictionary[total];
	for(int i = 0; i < total; i++) {
		dictionary[i].val = "";
	}
	for(int i = 0; i < 256; i++) {
		dictionary[i].val = (char *)i;
	}

	//define dictionary
	return c_file_name;
}
