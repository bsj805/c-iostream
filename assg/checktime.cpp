#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <memory.h>

#define THRNUM 2
#define BILLION 1000000000L
using namespace std;
using std::thread;
using std::vector;
using std::string;
// aggregate variables
long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

int main(int argc, char* argv[])
{
	struct timespec start, stop;
	if(argc !=2) {
		printf("Usage: %s <infile> <filename> \n",argv[0]);
		exit(EXIT_FAILURE);
	}
	char *fn = argv[1];
	char *inp = argv[2];
	int threadnum;
	
	clock_gettime( CLOCK_REALTIME, &start);
	
	string prog;
	string inptxt;
	prog="./"+fn;
	
	if(fork() == 0 )
	{
		execl("/home/2016310901/assg",prog.c_str(),inp,"4",NULL);
		exit(1);
	}


	clock_gettime(CLOCK_REALTIME, &stop);


	(stop.tv_sec - start.tv_sec) + ((double) (stop.tv_nsec - start.tv_nsec))/BILLION;
	

}
