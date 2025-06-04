#include "bitmap.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


struct dict {
	//key is last val in sequence
	long key;
	//sequence of vals separated by ,'s
	char *val;
}; typedef struct dict dict_t;

long hash(long key, size_t dict_size) {
	long scale = (dict_size * 0.67)/256;
	return scale * key;
}

//generates key from a set of vals
long generate_key(char *val) {
	long key = 0;
	int idx = strlen(val) - 2;
	char buff[10];
	while(val[idx] != ',') {
		char *dig = &val[idx];
		int pos = strlen(buff);
		buff[pos] = *dig;
		buff[pos + 1] = '\0';
		idx--;
	}
	int buff_len = strlen(buff);
	char rev[buff_len];
	for(int i = 0; i < buff_len; i++) {
		rev[i] = buff[buff_len - i - 1];
	}
	rev[buff_len] = '\0';
	key = atoi(rev);
	return key;
}

//return index to a free dictionary entry
int search(char *seq, dict_t dict[], size_t dict_size) {
	//checks region sequence could be held
	long key = generate_key(seq);
	for(int i = hash(key, dict_size); dict[i].val != NULL && i < dict_size; i++) {
		//return -1 if entry exists
		if(strcmp(seq, dict[i].val) == 0) {
			return -1;
		}
		//return first available index in dict
		if(strcmp(dict[i].val, ",") == 0) {
			printf("%d\n", i);
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

size_t dict_size(long pixels) {
	//allocate enough space for max possible vals + min possible at last hash
	//max represents the maximum possible values the dict needs to hold
	long max = 0;
	long total = 0;
	long i = 0;
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
	long min = -1;
	total = 0;
	while(total < pixels) {
		min++;
		total = total + min;
	}
	long size = min + max;
	return size;
}

dict_t *make_dict(long size) {
	dict_t *dict = malloc(sizeof(dict_t) * size);
	for(long i = 0; i < size; i++) {
		dict[i].val = NULL;
		dict[i].key = -1;
	}
	return dict;
}

//if able insert set of vals into the dictionary
void insert(char *val, dict_t dict[], long idx) {
	int key = generate_key(val);
	dict[idx].val = val;
	dict[idx].key = key;
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
	long pixels = width * height * 3;
	long size_of_dict = dict_size(pixels);
	dict_t *dict = make_dict(size_of_dict);
	for(int i = 0; i < 256; i++) {
		char init_buff[10];
		sprintf(init_buff, ",%d,", i);
		long dict_key = generate_key(init_buff);
		long dict_idx = hash(dict_key, size_of_dict);
		insert(init_buff, dict, dict_idx);
	}
	
	//find the longest sequence in the dictionary that matches the current input
	long max_pix_count = 0;
	long max_val_len = 0;
	while(max_pix_count < pixels) {
		max_val_len++;
		max_pix_count += max_val_len;
	}
	char max_num[15];
	sprintf(max_num, "%ld", size_of_dict);
	int max_size_of_val = strlen(max_num);

	char *idx_last_known_seq;
	char *output = malloc(size_of_dict * (max_size_of_val + 1));  //holds total output	
	char *sequence = malloc(max_val_len * sizeof(char) * 4);	  //holds current longest sequence
	char *next_seq = malloc(max_val_len * sizeof(char) * 4);      //holds next sequence
	sequence[0] = next_seq[0] = output[0] = ',';
	sequence[1] = next_seq[1] = output[1] = '\0';
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			char buff[5];
			sprintf(buff, "%d,", image[i][j].rgbtRed);
			strcat(next_seq, buff);
			long dict_idx = search(next_seq, dict, size_of_dict);
			printf("%s\n", next_seq);	

			//in case of new sequence, add to dictionary, 
			if(dict_idx != -1) {
				printf("test1\n");
				insert(next_seq, dict, dict_idx);
				char new_idx[15];
				sprintf(new_idx, "%ld,", dict_idx);
				strcat(output, idx_last_known_seq);
				memset(next_seq, 0, strlen(next_seq));
				memset(sequence, 0, strlen(sequence));
			}
			
			//in case of known sequence
			else {
				//printf("test2\n");
				char known_idx[5];
				sprintf(known_idx, "%ld,", dict_idx);
				idx_last_known_seq = known_idx;
				strcat(sequence, buff);
			}
		}
	}	
	
	free(output);
	free(sequence);
	free(next_seq);
	free(dict);
	return c_file_name;
}
