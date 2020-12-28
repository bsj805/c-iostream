#include <iostream>
#include <atomic>
#include <fstream>
#include <omp.h>
#include <memory.h>
using namespace std;
int sa[10];
atomic<int> count[256];
//atomic<int> (*newarr)[100][256];
atomic<int> ***newarr;
int incr(string* arr,int lo,int hi,int d)
{
	atomic<int>* pcur;
	pcur=newarr[lo][d%256];

	int small[256]={0};
	int absmall[256]={0};
	int las,las2;
	las=1;las2=2;
	sa[0]=1;
	cout<<"thread:"<<omp_get_thread_num()<<endl;
	#pragma omp for 
	for(int i=0;i<256;i++)
	{
		if(i==0)
		{
			cout<<"몇번반복될까?"<<omp_get_thread_num()<<endl;
		}

		small[i]+=1;
		absmall[i]+=1;
		pcur[i].fetch_add(1);

		__atomic_compare_exchange(&small[i],&las,&las2,0,0,0);

	}

	cout<<"this is middle"<<sa[0]<<endl;
	while(true)
	{
		las=sa[0];
	 las2=las+1;

		cout<<"las="<<las<<"las2"<<las2<<endl;
	if(__atomic_compare_exchange(&sa[0],&las,&las2,true,__ATOMIC_RELAXED,__ATOMIC_RELAXED))
		break;
	}
	
	cout<<omp_get_thread_num()<<"sa:"<<sa[0]<<endl;
	int t;
	t=omp_get_num_threads();

	cout<<"end"<<endl;
	//cout<<t<<"pcur:"<<pcur[0]<<endl<<sa[0]<<endl;
	//이걸로 알 수 있는건 
	//각자가 small을 가지고 있다는건데, 그걸 나중에 공통의 var에 합쳐줄수만있다면된다.

}
int main(int argc, char* argv[])
{

	int a=100;
	newarr=new atomic<int>** [a];
	for(int i=0;i<a;i++)
	{
		newarr[i]=new atomic<int>* [100];
		for(int j=0;j<100;j++)
		{
			newarr[i][j]=new atomic<int> [256];
		}
	}

	for(int i=0;i<100;i++)
	{
		for(int j=0;j<100;j++)
		{
			for(int k=0;k<256;k++)
			{
				//newarr[i][j][k]=0;
			}
			memset(newarr[i][j],0,sizeof(atomic<int>)*256);
		}
	}
	for(int i=0;i<256;i++)
	{
		count[i]=0;
	}
	
	cout<<count[1]<<endl;
	cout<<count[1].fetch_add(1)<<endl;
	cout<<count[1]<<endl;
	int las=2;
	if(count[1].compare_exchange_weak(las,las+1))
	{
		cout<<"las=:"<<las<<endl;
		cout<<"count:"<<count[1]<<endl;
	}
	else
	{
		cout<<"falselas=:"<<las<<endl;
		cout<<"count:"<<count[1]<<endl;

	}
	omp_set_num_threads(2);
	ifstream inputfile;
	inputfile.open(argv[1]);
	string arr[256];

	for(int i=0;i<3;i++)
	{
		//inputfile>>arr[i];
		getline(inputfile,arr[i]);
		if(arr[i].length()==0)
			arr[i]=' ';
	}
	cout<<"dd"<<arr[1].c_str()[0]<<"ee"<<endl;
	if(arr[1].c_str()[0]==0x20)
	{
		cout<<"small"<<endl;
	}
	inputfile.close();

		cout<<"hi????"<<endl;
	#pragma omp parallel
		{
	incr(arr,0,3,0);
		}
		cout<<"why??"<<endl;
	for(int i=0;i<100;i++)
	{
		for(int j=0;j<100;j++)
		{
			
				delete[] newarr[i][j];
			
		}

		delete[] newarr[i];
	}
	delete[] newarr;
}
