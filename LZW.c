#include "bitmap.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


struct dict {
	uint64_t key;
	//sequence of vals separated by ,'s
	uint8_t *sequence;
	//size of val array
	size_t seq_len;
}; typedef struct dict dict_t;

uint64_t hash(uint8_t seq[], size_t seq_len) {
	uint64_t hash = 14695981039346656037u; 
	for(int i = 0; i < seq_len; i++) {
		hash ^= seq[i];
		hash *= 1099511628211u;
	}
	return hash;
}

bool check_seq(uint8_t seq[], size_t seq_len, dict_t dict[], uint64_t key) {
	if(seq_len != dict[key].seq_len) {
		return false;
	}
	for(int i = 0; i < seq_len; i++) {
		if(seq[i] != dict[key].sequence[i]) {
			return false;
		}
	}
	return true;
}

//return index to next free dictionary element of index of existing sequence
//check_sequence will return true if sequences are identical i.e. idx is the existing val
int search(uint64_t key,  dict_t dict[], size_t dict_size, uint8_t seq[], size_t seq_len, bool *check_sequence) {
	key = key % dict_size;
	for(uint64_t i = key; i < dict_size; i++) {
		//if dict location is empty, return location
		if(dict[i].sequence == NULL) {
			return i;
		}
		*check_sequence = check_seq(seq, seq_len, dict, key);
		//if sequence is correct return the index
		if(*check_sequence == true) {
			return i;
		}
	}
	//in case of error
	return -1;
}

//make dictionary and calculate size of dictionary size in dict_size pointer
dict_t *make_dict(long pixels, size_t *dict_size) {
	*dict_size = 2;
	for(int i = 1; i < pixels; i++) {
		*dict_size *= 2;
		if(*dict_size >= pixels) {
			break;
		}
	}
	dict_t *dict = malloc(sizeof(dict_t) * *dict_size);
	if(dict == NULL) {
		printf("failed to allocate memory\n");
		return NULL;
	}
	for(int i = 0; i < *dict_size; i++) {
		dict[i].key = -1;
		dict[i].sequence = NULL;
		dict[i].seq_len = 0;
	}
	return dict;
}

//if able insert set of vals into the dictionary
void insert(dict_t dict[], uint64_t key, uint8_t sequence[], size_t seq_len, int idx) {
	dict[idx].key = key;
	dict[idx].sequence = sequence;
	dict[idx].seq_len = seq_len;
	return;
}

void free_dict(dict_t dict[], size_t dict_size) {
	for(int i = 0; i < dict_size; i++) {
		if(dict[i].sequence != NULL) {
			free(dict[i].sequence);
		}
	}
	free(dict);
	return;
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
	size_t size = 0;
	size_t *dict_size = &size;
	dict_t *dict = make_dict(pixels, dict_size);
	for(int i = 0; i < 256; i++) {
		uint8_t *init_seq = malloc(sizeof(uint8_t));
		*init_seq = i;
		uint64_t key = hash(init_seq, 1);
		bool *check_sequence;
		int idx = search(key, dict, *dict_size, init_seq, 1,  check_sequence);
		insert(dict, key, init_seq, 1, idx);
	}
	
	//loop through image 3 times to find red green and blue values consecutively
	//find longest sequence and save dictionary index to output
	uint64_t *output = malloc(sizeof(uint64_t) * pixels/2);
	size_t out_len = 0;
	uint8_t *next_seq = malloc(sizeof(uint64_t) * pixels/2);
	size_t seq_len = 0;
	int last_known_idx = 0;
	//red vals
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			uint8_t current = image[i][j].rgbtRed;
			next_seq[seq_len] = current;
			seq_len++;
			uint64_t next_key = hash(next_seq, seq_len);
			bool known = false;
			bool *is_known_seq = &known;
			int idx = search(next_key, dict, *dict_size, next_seq, seq_len, is_known_seq);

			//if -1 something went wrong
			if(idx == -1) {
				printf("something went wrong\n");
				return NULL;
			}
			//new dict entry
			if(*is_known_seq == false) {
				output[out_len] = last_known_idx;
				out_len++;
				uint8_t *new_seq = malloc(sizeof(uint8_t) * seq_len);
				for(int x = 0; x < seq_len; x++) {
					new_seq[x] = next_seq[x];
				}
				insert(dict, next_key, new_seq, seq_len, idx);
				seq_len = 0;
			}
			//is known sequence
			if(*is_known_seq) {
				last_known_idx = idx;
			}
		}
	}

	//green vals
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			uint8_t current = image[i][j].rgbtGreen;
			next_seq[seq_len] = current;
			seq_len++;
			uint64_t next_key = hash(next_seq, seq_len);
			bool known = false;
			bool *is_known_seq = &known;
			int idx = search(next_key, dict, *dict_size, next_seq, seq_len, is_known_seq);

			//if -1 something went wrong
			if(idx == -1) {
				printf("something went wrong\n");
				return NULL;
			}
			//new dict entry
			if(*is_known_seq == false) {
				output[out_len] = last_known_idx;
				out_len++;
				uint8_t *new_seq = malloc(sizeof(uint8_t) * seq_len);
				for(int x = 0; x < seq_len; x++) {
					new_seq[x] = next_seq[x];
				}
				insert(dict, next_key, new_seq, seq_len, idx);
				seq_len = 0;
			}
			//is known sequence
			if(*is_known_seq) {
				last_known_idx = idx;
			}
		}
	}

	//blue vals
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			uint8_t current = image[i][j].rgbtBlue;
			next_seq[seq_len] = current;
			seq_len++;
			uint64_t next_key = hash(next_seq, seq_len);
			bool known = false;
			bool *is_known_seq = &known;
			int idx = search(next_key, dict, *dict_size, next_seq, seq_len, is_known_seq);

			//if -1 something went wrong
			if(idx == -1) {
				printf("something went wrong\n");
				return NULL;
			}
			//unknown sequence
			if(*is_known_seq == false) {
				output[out_len] = last_known_idx;
				out_len++;
				uint8_t *new_seq = malloc(sizeof(uint8_t) * seq_len);
				for(int x = 0; x < seq_len; x++) {
					new_seq[x] = next_seq[x];
				}
				insert(dict, next_key, new_seq, seq_len, idx);
				seq_len = 0;
			}
			//is known sequence
			if(*is_known_seq) {
				last_known_idx = idx;
			}
		}
	}
	fwrite(output, sizeof(uint8_t), out_len, temp);
	fclose(temp);

	free(output);
	free(next_seq);
	free_dict(dict, *dict_size);
	return c_file_name;
}
