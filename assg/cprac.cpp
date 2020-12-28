#include <iostream>
#include <mutex>
using namespace std;
using std::mutex;

int main()
{
	cout<<"start"<<endl;

	mutex m;
	m.unlock();
	cout<<"end"<<endl;
	
}


