#include <iostream>
#include <fstream>
#include <string>
using namespace std;
class Plane{

	public:

	Plane()
	{
		isalive=false;
		count=0;
	}
		bool isalive;
		char count;
};
int main ()
{
	string s,inp;
	int t;
	s="life.data.1";
	ifstream inputfile;
	inputfile.open(s);
	int current;
	int temp_x;
	string sub;
	int temp_y;
	while(1)
	{
		
		getline(inputfile,inp);
		cout<<"last:"<<inp<<endl;//마지막에는 공백들어가네
		if(inputfile.eof())
		{
			cout<<"finished"<<endl;
			break;
		}
		current=inp.find(' ');
		sub=inp.substr(0,current);
		cout<<"substr"<<sub<<endl;
		temp_x=stoi(sub);
		cout<<temp_x;
		sub=inp.substr(current+1,inp.length()-current-1);
		cout<<"substr"<<sub;
		temp_y=stoi(sub);
		cout<<temp_y<<"d"<<endl;
		t++;
	}
	cout<<"total:"<<t<<endl;
	Plane *parr;
	int i,j;
	int k=20;
	parr=new Plane[k];
	char a=0;
	cout<<"10//3"<<10/3<<endl;
	if(a==0)
	{
		cout<<"true"<<endl;
		cout<<parr[1].isalive<<endl;
	}
	cout<<a++<<endl;
	cout<<a<<endl;
	if(a==1)
	{
		cout<<"yes"<<endl;
	}
	delete[] parr;
}
