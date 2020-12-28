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
#include <atomic>
using namespace std;

#define BILLION  1000000000L
#define NTHREAD 2
#define SECONDMAX 100
int N;
//int *count;
string *temp;
atomic<int> ***newcnt;
atomic<int> ***newsubcnt;
int **realcnt;
int **subrealcnt;
void msdcu(string* a, int lo, int hi, int d);
void msdc(string* a,int len){
	msdcu(a,0,len,0);
}
void msdcu(string* a, int lo, int hi, int d)
{

	int curcidx;
	atomic<int>* pcurc;
	atomic<int>* psubc;
	curcidx=d%256;
	pcurc=newcnt[lo][curcidx];
	psubc=newsubcnt[lo][curcidx];
	if(hi<=lo+1)return;
	int count[256]={0};
	int subcount[256]={0};
	//for(int i=0;i<256;i++)	{		count[i]=0;		subcount[i]=0;	}
	cout<<"hello world"<<endl;
	#pragma omp single
	{
	for(int i=lo;i<hi;i++)
	{
		//count[a[i].at(d)+1]++;
		//cout<<i<<endl;
		curcidx=a[i].at(d)+1;
		#pragma omp critical
		pcurc[curcidx]+=1;
		//pcurc[a[i].at(d)+1].fetch_add(1);
		//cout<<"pcurc"<<pcurc[a[i].at(d)+1]<<endl;
	}
	}
	cout<<"why?"<<endl;
	//for(int i=0;i<256;i++){		pcurc[i].fetch_add(count[i]);	}
	
	#pragma omp single
	{
	for(int k=1;k<256;k++)//prefix sum
		pcurc[k]+=pcurc[k-1];
	cout<<"Okay"<<endl;
	}
	cout<<"okay"<<endl;
	//#pragma omp barrier
	//이 밑에가 궁금해 count가 atomic하게안되니까 다 single로돌려
	#pragma omp single
	for(int i=lo;i<hi;i++)//이게 걸리려면 d=1 pr pr pro pra 일때만이니까
	{
		if(a[i].length()==d+1) //pr,pr은 요기걸리게돼.그래서 먼저 index배정받음
		{
			psubc[a[i].at(d)-1].fetch_add(1);
			//subcount[a[i].at(d)-1]++;
			int las,t;
			cout<<"upd"<<d<<endl;
			t=a[i].at(d);
			while(true)
			{
				las=pcurc[t].load();
				if(pcurc[t].compare_exchange_weak(las,las+1))
					break;
			}
			//las=lo+count[a[i].at(d)]++;
			temp[lo+las]=a[i];			
		}
	}
	#pragma omp single
	for(int i=lo; i<hi;i++)
	{
		if(a[i].length()!=d+1) 
		{
			cout<<"lod"<<d<<endl;
			int las;int t;
			t=a[i].at(d);
			while(true)
			{
				las=pcurc[t].load();
				if(pcurc[t].compare_exchange_weak(las,las+1))
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
		if(lo+pcurc[i+1]<=lo+psubc[i]+pcurc[i]+1) continue;
		#pragma omp task
		msdcu(a,lo+psubc[i]+pcurc[i],lo+pcurc[i+1],d+1); //각 구간별로 
	}

	#pragma omp taskwait


}
int main(int argc, char* argv[])
{
	struct timespec start, stop;
	ifstream inputfile;
	inputfile.open(argv[1]);
	string* arr;
	N=atoi(argv[2]);
	temp=new string[atoi(argv[2])];
	//count=new int[atoi(argv[2])];
	arr=new string[atoi(argv[2])];
	newcnt=new atomic<int>** [N];
	newsubcnt=new atomic<int>** [N];
	for(int i=0;i<N;i++)
	{

		newcnt[i]=new atomic<int>* [SECONDMAX];
		newsubcnt[i]=new atomic<int>* [SECONDMAX];
		for(int j=0;j<SECONDMAX;j++)
		{
			newcnt[i][j]=new atomic<int> [256];
			newsubcnt[i][j]=new atomic<int> [256];

			memset(newcnt[i][j],0,sizeof(atomic<int>)*256);
			memset(newsubcnt[i][j],0,sizeof(atomic<int>)*256);
		}
	}
	//realcnt=new int*[atoi(argv[2])];
	//subrealcnt=new int*[atoi(argv[2])];
	/*
	for(int i=0;i<atoi(argv[2]);i++)
	{
	
		realcnt[i]=new int[256];
		subrealcnt[i]=new int[256];
		memset(realcnt[i],0,sizeof(int)*256);
		memset(subrealcnt[i],0,sizeof(int)*256);
	}*/
	
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
	#pragma omp parallel shared(arr,N,temp,newcnt,newsubcnt)
	{
	msdc(arr,N);
	}

	
	clock_gettime(CLOCK_REALTIME,&stop);
	
	for(i=atoi(argv[3]);i<atoi(argv[3])+atoi(argv[4]);i++)
	{
		cout<<arr[i]<<endl;
	}
cout << "Elapsed time: " << (stop.tv_sec - start.tv_sec) +	((double) (stop.tv_nsec - start.tv_nsec))/BILLION << " sec" << endl;
	for(i=0;i<atoi(argv[2]);i++)
	{
		for(j=0;j<SECONDMAX;j++)
		{
			delete[] newcnt[i][j];
			delete[] newsubcnt[i][j];
		}

		delete[] newcnt[i];
		delete[] newsubcnt[i];
		delete[] realcnt[i];
		delete[] subrealcnt[i];
	}
	delete[] subrealcnt;
	delete[] realcnt;
	delete[] arr;
	delete[] newcnt;
	delete[] newsubcnt;
	//delete[] count;
	delete[] temp;
}

