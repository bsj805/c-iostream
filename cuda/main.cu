#include <stdio.h>
#include <cuda.h>

__global__ void cuda_hello(){
    //printf("Hello World from GPU!\n");
	int i=0;
	i+=1;
}

int main() {
    cuda_hello<<<65537,1024>>>();
    cudaDeviceSynchronize();
    return 0;
}

