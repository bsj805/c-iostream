#include <iostream>
#include <atomic>

using namespace std;
int main()
{
	atomic<int> count[256];
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
	

}
