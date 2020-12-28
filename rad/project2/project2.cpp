#include <iostream>
#include <fstream>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>
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
void msdcu(string* a, int lo, int hi, int d);
void msdc(string* a,int len){
	msdcu(a,0,len,0);
}
void msdcu(string* a, int lo, int hi, int d)
{

	if(hi<=lo+1)return;
	int count[256]={0};
	int subcount[256]={0};
	//cout<<omp_get_thread_num()<<"번"<<lo<<"=lo d="<<d<<endl;
//	for(int i=0;i<256;i++)	{		count[i]=0;		subcount[i]=0;	}
	for(int i=lo;i<hi;i++)
	{
		count[a[i].at(d)+1]++;
	}
	for(int k=1;k<256;k++)//prefix sum
		count[k]+=count[k-1];
	for(int i=lo;i<hi;i++)//이게 걸리려면 d=1 pr pr pro pra 일때만이니까
	{
		if(a[i].length()==d+1) //pr,pr은 요기걸리게돼.그래서 먼저 index배정받음
		{
			subcount[a[i].at(d)-1]++;
			int las;
			las=lo+count[a[i].at(d)]++;
			temp[las]=a[i];			
		}
	}
	for(int i=lo; i<hi;i++)
	{
		if(a[i].length()!=d+1) 
		{
			int las;
			las=lo+count[a[i].at(d)]++;
			temp[las]=a[i];	
		}
	}
	for(int i=lo;i<hi;i++)
		a[i]=temp[i];
	//pr,pr은 새로 함수 안들어가야해.

	for(int i=0;i<255;i++)
	{
		if(lo+count[i+1]<=lo+subcount[i]+count[i]+1) continue;
		#pragma omp task
		msdcu(a,lo+subcount[i]+count[i],lo+count[i+1],d+1); //각 구간별로 
	}
	//#pragma omp taskwait


}
void firstmsdcu(string* a, int lo, int hi, int d)
{

	if(hi<=lo+1)return;
	int count[256]={0};
	int subcount[256]={0};
//	for(int i=0;i<256;i++)	{		count[i]=0;		subcount[i]=0;	}
	#pragma omp parallel shared(a,d,lo,hi,count,temp)
	{
	#pragma omp for
	for(int i=lo;i<hi;i++)
	{
		#pragma omp atomic
		count[a[i].at(d)+1]++;
	}
	#pragma omp single
	{
	for(int k=1;k<256;k++)//prefix sum
		count[k]+=count[k-1];
	
	}//pragma signle

	#pragma omp for schedule(dynamic, 1)
	for(int i=lo;i<hi;i++)//이게 걸리려면 d=1 pr pr pro pra 일때만이니까
	{
		if(a[i].length()==d+1) //pr,pr은 요기걸리게돼.그래서 먼저 index배정받음
		{
			#pragma omp atomic
			subcount[a[i].at(d)-1]++;
			int las,las2;
			while(true)
			{
				las=count[a[i].at(d)];
				las2=las+1;
				if(__atomic_compare_exchange(&count[a[i].at(d)],&las,&las2,true,__ATOMIC_RELAXED,__ATOMIC_RELAXED))
					break;
			}
			//las=lo+count[a[i].at(d)]++;
			temp[lo+las]=a[i];			
		}
	}
	#pragma omp for schedule(dynamic,1)
	for(int i=lo; i<hi;i++)
	{
		if(a[i].length()!=d+1) 
		{
			int las,las2;
			while(true)
			{
				las=count[a[i].at(d)];
				las2=las+1;
				if(__atomic_compare_exchange(&count[a[i].at(d)],&las,&las2,true,__ATOMIC_RELAXED,__ATOMIC_RELAXED))
					break;

			}
			//las=lo+count[a[i].at(d)]++;
			temp[lo+las]=a[i];	
		}
	}

	#pragma omp for
	for(int i=lo;i<hi;i++)
		a[i]=temp[i];
	//pr,pr은 새로 함수 안들어가야해.
	#pragma omp for
	for(int i=0;i<255;i++)
	{
		if(lo+count[i+1]<=lo+subcount[i]+count[i]+1) continue;
		#pragma omp task
		msdcu(a,lo+subcount[i]+count[i],lo+count[i+1],d+1); //각 구간별로 
	}

	}//pragma parallel 
	//#pragma omp taskwait


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
		//inputfile >> arr[i];
		getline(inputfile,arr[i]);
		if(arr[i].length()==0)
			arr[i]=' ';
		
	}
	inputfile.close();
	int i,j;
	
	N=atoi(argv[2]);

	//measure time
	clock_gettime(CLOCK_REALTIME, &start);
	omp_set_num_threads(atoi(argv[5]));
	//omp_set_num_threads(NTHREAD);
		firstmsdcu(arr,0,N,0);
	#pragma omp taskwait

	

	clock_gettime(CLOCK_REALTIME,&stop);
	
	for(i=atoi(argv[3]);i<atoi(argv[3])+atoi(argv[4]);i++)
	{
		cout<<arr[i]<<endl;
	}
cout << "Elapsed time: " << (stop.tv_sec - start.tv_sec) +	((double) (stop.tv_nsec - start.tv_nsec))/BILLION << " sec" << endl;
	delete[] arr;
	//delete[] count;
	delete[] temp;
}

