#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <ctime>
//#include <cuda_runtime.h>

#define N 1000000
#define MAX_VAL 1000000

extern void counting_sort(int arr[], int, int);

int main()
{
  int array[N];

  for(int i=0;i<N;i++){
      array[i] = rand()%MAX_VAL;
  }
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  counting_sort(array, N, MAX_VAL);
  clock_gettime(CLOCK_MONOTONIC, &end);

  for(int i=0;i<N-1;i++){
      if( array[i] > array[i+1]){
          printf("Not sorted\n");
          exit(1);
      }
  }
  printf("Sorted\n");
  uint64_t elapsed = (end.tv_nsec - start.tv_nsec) + (end.tv_sec - start.tv_sec)*1000000000;
  printf("time(us): %lu\n", elapsed/1000);
}
