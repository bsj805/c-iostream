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
#include <atomic>
# define _Atomic(X) std::atomic< X >
//#include <atomic>
using namespace std;
//#define _Atomic(x) std::atomic< X > //https://stackoverflow.com/questions/45342776/how-to-include-c11-headers-when-compiling-c-with-gcc
#define BILLION  1000000000L
#define NTHREAD 2
int N;
//int *count;
string *temp;

atomic<int> count[256];

atomic<int> subcount[256];
//omp_lock_t lock[256];
void msdcu(string* a, int lo, int hi, int d);
void msdcu(string* a, int lo, int hi, int d)
{

	//cout<<lo<<"arr="<<hi<<"d="<<d<<endl;for(int i=0;i<N;i++)	{		cout<<"["<<i<<"]"<<a[i]<<" ";	}	cout<<endl;
	if(hi<=lo+1)return;
	//int sharedcount[256]={0};

	//atomic<int> count[256];

	//atomic<int> subcount[256];
	for(int i=0;i<256;i++)
	{
		count[i]=0;
		subcount[i]=0;
	}
	//int count[256]={0}; 
	//int subcount[256]={0};
	//int count[256]={0}; int subcount[256]={0};
	#pragma omp for 
	for(int i=lo;i<hi;i++)
	{
		int las;
		int t;
		t=a[i].at(d);
		//#pragma omp atmoic it doesn't give atomicity to array element. 
		//we use fine grained locking here
		/*while(true)
		{
			las=count[t];
			if(compare_and_swap(count[t],las,las+1))
			{
				break;
			}
		}*/
		count[t+1].fetch_add(1,memory_order_relaxed);
		//omp_set_lock(&(lock[t]));
		//count[t]+=1;
		//omp_unset_lock(&(lock[t]));
		//cout<<"t"<<t<<" ";
	}
	
	
	//cout<<"count="<<endl;for(int i=48;i<59;i++)	{		cout<<"["<<i<<"]"<<count[i]<<" ";	}	cout<<endl;
	#pragma omp single
	{
	for(int k=1;k<256;k++)//prefix sum
		count[k]+=count[k-1];
	//implicit barrier
	//cout<<"hi"<<endl;
	}
	int tempcount[256],tempsubcount[256];

	//#pragma omp parallel firstprivate(a,lo,hi,d)
	#pragma omp parallel shared(a,lo,hi,d,count,subcount,temp,tempcount) 
	{
	#pragma omp for schedule (dynamic,3 )
	for(int i=lo;i<hi;i++)//이게 걸리려면 d=1 pr pr pro pra 일때만이니까
	{
		if(a[i].length()==d+1) //pr,pr은 요기걸리게돼.그래서 먼저 index배정받음
		{
			//#pragma omp atomic it doesn't give atomicity to array elements
			int t;
			t=a[i].at(d);
			//omp_set_lock(&(lock[t]));
			subcount[t-1].fetch_add(1,memory_order_relaxed);
			//subcount[t-1]++;
			int las;
			//#pragma omp atomic //it doesn't work
			//#pragma omp critical(first) //we use fine grained locking
			while(true)
			{
				las=count[t].load();
				//if(atomic_compare_exchange_weak(count[t],las,las+1))
				if(count[t].compare_exchange_weak(las,las+1))
					break;
			}
			//las=count[t].load(memory_order_release);
			//count[t].fetch_add(1,memory_order_acquire);//prev값이남는다.

			//las=count[t]++;
			//omp_unset_lock(&(lock[t]));
			//cout<<lo+las<<"las="<<a[i]<<endl;
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
			//omp_set_lock(&(lock[t]));
			while(true)
			{
				las=count[t].load();
				//if(atomic_compare_exchange_weak(count[t],las,las+1))
				if(count[t].compare_exchange_weak(las,las+1))
					break;
			}
			//las=count[t].load(memory_order_release);
			//count[t].fetch_add(1,memory_order_acquire);
			//las=count[t]++;
			//omp_unset_lock(&(lock[t]));
			temp[lo+las]=a[i];	
		}
	}
	#pragma omp for
	for(int i=lo;i<hi;i++)
		a[i]=temp[i];
	//pr,pr은 새로 함수 안들어가야해.

	#pragma omp for
	for(int i=0;i<256;i++)
	{
		tempcount[i]=count[i];
		tempsubcount[i]=subcount[i];
	}
	}//pragma omp parallel 
	
			//cout<<"lowercount="<<endl;for(int i=49;i<59;i++)	{		cout<<"["<<i<<"]"<<count[i]<<" ";	}	cout<<endl;

	for(int i=0;i<255;i++)
	{
	//	if(i==49)//공통arr count쓰니까 생기는문제임. 어디 옮겨놓자. {			//cout<<"49dddd"<<lo+count[i+1]<<"ddd"<<lo+subcount[i]+count[i]+1<<endl;}
		if(lo+tempcount[i+1]<=lo+tempsubcount[i]+tempcount[i]+1)
		{
			continue;
		}

		//cout<<lo<<"lowerarr="<<hi<<"d="<<d<<endl;for(int j=0;j<N;j++)	{		cout<<"["<<j<<"]"<<a[j]<<" ";	}	cout<<endl;
		msdcu(a,lo+tempsubcount[i]+tempcount[i],lo+tempcount[i+1],d+1); //각 구간별로 
	}

}
int func_compare(const void* first,const void *second)
{
	int res;
	if(*(string*)first<*(string*)second)
	{
		return 1;
	}
	else if(*(string*)first > *(string*)second)
		return -1;
	else
		return 0;
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
	//omp_lock_t lock[256];
	inputfile.close();
	int i,j;
	
	N=atoi(argv[2]);

	//measure time
	clock_gettime(CLOCK_REALTIME, &start);
	//for(int i=0; i<256;i++)omp_init_lock(&(lock[i]));

	omp_set_num_threads(atoi(argv[5]));
	//omp_set_num_threads(NTHREAD);
	#pragma omp parallel shared(arr,N)
	{
	msdcu(arr,0,N,0);
	//qsort((string*)arr,N,sizeof(string),func_compare);
	}
	
	clock_gettime(CLOCK_REALTIME,&stop);
	
	for(i=atoi(argv[3]);i<atoi(argv[3])+atoi(argv[4]);i++)
	{
		cout<<arr[i]<<endl;
	}
cout << "Elapsed time: " << (stop.tv_sec - start.tv_sec) +	((double) (stop.tv_nsec - start.tv_nsec))/BILLION << " sec" << endl;

	//for(i=0;i<256;i++)	omp_destroy_lock(&(lock[i]));
	delete[] arr;
	//delete[] count;
	delete[] temp;
}

