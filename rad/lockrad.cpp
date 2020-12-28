#include <iostream>
#include <fstream>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <string>
#include <vector>
#include <memory.h>
#include <ctime>
using namespace std;

#define BILLION  1000000000L
#define NTHREAD 2
int N;
//int *count;
string *temp;
omp_lock_t lock[256];
void msdcu(string* a, int lo, int hi, int d);
void msdcu(string* a, int lo, int hi, int d)
{

	if(hi<=lo+1)return;
	//cout<<lo<<"arr="<<hi<<"d="<<d<<endl;for(int i=0;i<N;i++)	{		cout<<"["<<i<<"]"<<a[i]<<" ";	}	cout<<endl;
	//int sharedcount[256]={0};
	int count[256]={0}; int subcount[256]={0};
	//int count[256]={0}; int subcount[256]={0};
	#pragma omp parallel for shared(a,lo,hi,d,count,lock)
	for(int i=lo;i<hi;i++)
	{
		int t;
		t=a[i].at(d)+1;
		//#pragma omp atmoic it doesn't give atomicity to array element. 
		//we use fine grained locking here
		omp_set_lock(&(lock[t]));
		count[t]+=1;
		omp_unset_lock(&(lock[t]));
		//cout<<"t"<<t<<" ";
	}
	
	
	//cout<<"count="<<endl;for(int i=48;i<59;i++)	{		cout<<"["<<i<<"]"<<count[i]<<" ";	}	cout<<endl;

	for(int k=1;k<256;k++)//prefix sum
		count[k]+=count[k-1];
	//implicit barrier
	//cout<<"hi"<<endl;
	
	#pragma omp parallel shared(a,lo,hi,d,count,subcount,lock) 
	{
	#pragma omp for schedule (dynamic,3 )
	for(int i=lo;i<hi;i++)//이게 걸리려면 d=1 pr pr pro pra 일때만이니까
	{
		if(a[i].length()==d+1) //pr,pr은 요기걸리게돼.그래서 먼저 index배정받음
		{
			//#pragma omp atomic it doesn't give atomicity to array elements
			int t;
			t=a[i].at(d);
			omp_set_lock(&(lock[t]));
			subcount[t-1]++;
			int las;
			//#pragma omp atomic //it doesn't work
			//#pragma omp critical(first) //we use fine grained locking
			las=count[t]++;
			omp_unset_lock(&(lock[t]));
		//	cout<<lo+las<<"las="<<a[i]<<endl;
			temp[lo+las]=a[i];			
		}
	}
	#pragma omp for schedule (dynamic,3)
	for(int i=lo; i<hi;i++)
	{
		if(a[i].length()!=d+1) 
		{
			int t;
			t=a[i].at(d);
			int las;
			//#pragma omp critical(first)
			omp_set_lock(&(lock[t]));
			las=count[t]++;
			omp_unset_lock(&(lock[t]));
			temp[lo+las]=a[i];	
		}
	}
	#pragma omp for
	for(int i=lo;i<hi;i++)
		a[i]=temp[i];
	//pr,pr은 새로 함수 안들어가야해.

	}//pragma omp parallel 
	
	for(int i=0;i<255;i++)
	{
		if(lo+count[i+1]<=lo+subcount[i]+1)
		{
			continue;
		}
		msdcu(a,lo+subcount[i]+count[i],lo+count[i+1],d+1); //각 구간별로 
	}

}
int main(int argc, char* argv[])
{
	struct timespec start, stop;
	ifstream inputfile;
	inputfile.open(argv[1]);
	string* arr;
	temp=new string[atoi(argv[2])];
	//count=new int[atoi(argv[2])];
	arr=new string[atoi(argv[2])];
	for(int i=0;i<atoi(argv[2]);i++){
		inputfile >> arr[i];
		
	}
	//omp_lock_t lock[256];
	inputfile.close();
	int i,j;
	
	N=atoi(argv[2]);

	//measure time
	clock_gettime(CLOCK_REALTIME, &start);
	for(int i=0; i<256;i++)
		omp_init_lock(&(lock[i]));

	//omp_set_num_threads(atoi(argv[5]));
	omp_set_num_threads(NTHREAD);
	msdcu(arr,0,N,0);


	
	clock_gettime(CLOCK_REALTIME,&stop);
	
	for(i=atoi(argv[3]);i<atoi(argv[3])+atoi(argv[4]);i++)
	{
		cout<<arr[i]<<endl;
	}
cout << "Elapsed time: " << (stop.tv_sec - start.tv_sec) +	((double) (stop.tv_nsec - start.tv_nsec))/BILLION << " sec" << endl;

	for(i=0;i<256;i++)
		omp_destroy_lock(&(lock[i]));
	delete[] arr;
	//delete[] count;
	delete[] temp;
}

