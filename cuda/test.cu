#include <iostream>
#include <math.h>
#include <stdio.h>
#include <cuda.h>

__global__ void add(int n, float* x, float* y){
    for(int i = 0; i < n; i++){
        y[i] = x[i] + y[i];
    }

}

int main(){

    int N = 1000;
    int i, j, k;
    float* x, *x_d;
    float* y, *y_d;
    int size = N * sizeof(float);
    float maxError = 0.0;

    x = (float*)malloc(sizeof(float)*N);
    y = (float*)malloc(sizeof(float)*N);
    cudaMalloc(&x_d, N*sizeof(float));
    cudaMalloc(&y_d, N*sizeof(float));

    for(i = 0; i < N; i++){
        x[i] = 1.0;
        y[i] = 2.0;
    }
    cudaMemcpy(x_d, x, size, cudaMemcpyHostToDevice);
    cudaMemcpy(y_d, y, size, cudaMemcpyHostToDevice);

    add<<<10, 10>>>(N, x_d, y_d);

    cudaDeviceSynchronize();
    cudaMemcpy(y, y_d, size, cudaMemcpyDeviceToHost);

    for(i = 0; i < N; i++) {
	printf("y[%d]: %f\n", i, y[i]);
        maxError = max(maxError, abs(y[i]-3.0));
    }

    printf("max Error %9.5lf\n", maxError);

    cudaFree(x_d);
    cudaFree(y_d);
    return 0;

}
