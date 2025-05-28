#include "filter.h"
#include "compile.h"
#include "compression.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>

//Runs each test function and times performance based on clock speed then prints result
//Will update so that result is a decimal result based on a high performing modern CPU
void benchmark(void) {
	clock_t ticks;
	//Start benchmark tests
	printf("Starting benchmark\n\n");
	sleep(1);
	//start image filter test
	printf("========================================\n");
	printf("Starting image filtering\n");
	ticks = clock();
	filter();
	ticks = clock() - ticks;
	printf("Time taken: %fs\n", (float)ticks/	CLOCKS_PER_SEC);
	printf("========================================\n");
	//start compiler test
	//clock() cannot be used a system command starts an external process, time() must be used instead
	struct timespec start, end;
	double compile_time;
	printf("Starting compiler\n");
	clock_gettime(CLOCK_MONOTONIC, &start);
	//compile();
	clock_gettime(CLOCK_MONOTONIC, &end);
	compile_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
	printf("Time Taken: %fs\n", (float)compile_time);
	printf("========================================\n");
	//start compression test
	printf("Starting file compression\n");
	ticks = clock();
	compression();
	ticks = clock() - ticks;
	printf("Time taken: %fs\n", (float)ticks/ CLOCKS_PER_SEC);
	return;
}
