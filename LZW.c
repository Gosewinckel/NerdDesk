#include "bitmap.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


struct dict {
	//key is last val in sequence
	int key;
	//sequence of vals separated by ,'s
	char *val;
}; typedef struct dict dict_t;

int hash(int key, size_t dict_size) {
	int scale = (dict_size * 0.67)/256;
	return scale * key;
}

//generates key from a set of vals
int generate_key(char *val) {
	int key = 0;
	int idx = strlen(val) - 1;
	char buff[4];
	buff[3] = '\0';
	int dig = 2;
	while(val[idx] != ',' && val[idx != ' ']) {
		buff[dig] = val[idx];
	}
	key = atoi(buff);
	return key;
}

//return index to a free dictionary entry
int search(int key, char *seq, dict_t dict[], size_t dict_size) {
	//checks region sequence could be held
	for(int i = hash(key, dict_size); dict[i].val != NULL && i < dict_size; i++) {
		//return -1 if entry exists
		if(strcmp(seq, dict[i].val) == 0) {
			return -1;
		}
		//return first available index in dict
		if(dict[i].val == NULL) {
			return i;
		}
	}	
	//in case of error
	return -1;
}

bool check(int idx, char *seq, dict_t dict[]) {
	if(strcmp(seq, dict[idx].val) == 0) {
		return true;
	}
	return false;
}

dict_t *make_dict(int pixels) {
	//allocate enough space for max possible vals + min possible at last hash
	//max represents the maximum possible values the dict needs to hold
	int max = 0;
	int total = 0;
	int i = 0;
	while(true) {
		while(total < pow(256, i) * i && total < pixels) {
			max++;
			total += i;
		}
		i++;
		if(total >= pixels) {
			break;
		}
	}
	//reset total and calculate buffer of size min values
	//min represents the minimum number of values and the maximum number of all the same colour,
	//needs to be added to the end in case every hash is identical
	int min = -1;
	total = 0;
	while(total < pixels) {
		min++;
		total = total + min;
	}
	int size = min + max;
	dict_t *dict = malloc(sizeof(dict_t) * size);
	for(int i = 0; i < size; i++) {
		dict[i].val = NULL;
		dict[i].key = -1;
	}
	return dict;
}

//if able insert set of vals into the dictionary
void insert(char *val, dict_t dict[], size_t dict_size) {
	int key = generate_key(val);
	int ins = search(key, val, dict, dict_size);
	if(ins != -1) {
		dict[ins].val = val;
		dict[ins].key = generate_key(val);
	}
}

//returns name of new file
const char *LZW(FILE *file, char *name) {
	//make new file "file.cbmp", this will be used to store compressed data
	int len = strlen(name) + 2;
	char *c_file_name = malloc(len);
	strcpy(c_file_name, name);
	strcpy(&c_file_name[len - 6], ".cbmp");
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
		printf("Image = NULL\n");
		return NULL;
	}
	int padding = (4 - (width * sizeof(RGBTRIPLE))%4)%4;
	//move through file lines
	for(int j = 0; j < height; j++) {
		fread(image[j], sizeof(RGBTRIPLE), width, file);
		fseek(file, padding, SEEK_CUR);
	}
	fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, temp);
	fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, temp);

	//define initial dictionary
	int pixels = width * height * 3;
	dict_t *dict = make_dict(pixels);
	for(int i = 0; i < 256; i++) {

	}
	
	//find the longest sequence in the dictionary that matches the current input
	int start = 0; 
	int current = 0;
	char *val = "";

	free(dict);
	return c_file_name;
}
