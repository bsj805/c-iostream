#include <iostream>
#include <string>
#include <omp.h>
using namespace std;
#define NTHREAD 2
int main()
{
	string one;
	one="hello";
	omp_set_num_threads(NTHREAD);
	int cnt[256]={0};
	cout<<cnt[12]<<endl;
	int i;
	#pragma omp parallel for shared(one) schedule(dynamic,1)
	for(i=0;i<6;i++)
	{
		int t;
		//t=omp_get_num_threads();
		t=omp_get_thread_num();
		cout<<"threadid:"<<t<<"="<<i<<endl;
	}
	int temp[10];
	i=0;
	temp[i++]=1;

}
