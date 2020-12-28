#include <cuda.h>
#include <stdio.h>
#include <math.h>
#define SIZ 1024
__global__ //이게 device에서 실행될 function 각 thread가 일정량실행
void countnum(int* countarr,int* datarr,int n){ //threadIdx.x+blockDim.x*blockIdx.x
	int i=threadIdx.x+blockDim.x * blockIdx.x;
	if(i<n)
	{
		int num=datarr[i];
		atomicAdd(&countarr[num],1);
	}

	//atomicAdd(&arr[0],1);
	//printf(" %d ",arr[0]);

}
/*__global__
void countbignum(int* countarr,int* datarr,int n){

	int offset=1;
	if(offset<n)
	{
		int num=datarr[offset];
		atomicAdd(&countarr[num],1);
	}
}*/
__host__ void counting_sort(int arr[], int size, int max_val)
{
   // fill in 
	int* counting;
	counting=(int*)malloc(sizeof(int)*size);
	int* counting_d;
	int* datarr_d;//input array
	cudaMalloc((void **)&counting_d,sizeof(int)*max_val);
	cudaMemset(counting_d,0,max_val*(sizeof(int)));
	cudaMalloc((void**) &datarr_d,sizeof(int)*size);
	cudaMemcpy(datarr_d,arr,sizeof(int)*size,cudaMemcpyHostToDevice);
	int blocknum;
	blocknum=ceil((double)size/SIZ);

	//	countnum<<<1024,SIZ>>>(counting_d,datarr_d,size);
	//countnum<<<65535,SIZ>>>(counting_d,datarr_d,size);//1024가 max
	//	countnum<<<2097152,SIZ>>>(counting_d,datarr_d,size);
	//countnum<<<1048576,SIZ>>>(counting_d,datarr_d,size);
	countnum<<<blocknum,SIZ>>>(counting_d,datarr_d,size);
	
	cudaDeviceSynchronize();
	cudaMemcpy(counting,counting_d,sizeof(int)*max_val,cudaMemcpyDeviceToHost);
	int index;
	index=0;
	for(int j=0;j<max_val;j++)
	{
		for(int q=0;q<counting[j];q++)
		{
			arr[index++]=j;
		}
	}
	free(counting);
	cudaFree(counting_d);
	cudaFree(datarr_d);

}

