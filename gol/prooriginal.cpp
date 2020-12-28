#include <mpi.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <ctime>
using namespace std;

#define MAXY 10

class Plane
{
	public:
		Plane()
		{
			isalive=false;
			count=0;
		}
		bool isalive;
		char count;
};
int main(int argc,char* argv[])
{
	Plane **parr;

	int size;
	int rank;
	int processors;
	int totaliter;
	int x_limit,y_limit;
	ifstream inputfile;
	inputfile.open(argv[1]);
	processors=atoi(argv[2]);
	totaliter=atoi(argv[3]);
	x_limit=atoi(argv[4]);
	y_limit=atoi(argv[5]);
	int percpusize;
	persize=(x_limit/processors)+1; //
	int endOfBoard;
	if(persize==1)
	{
		endOfBoard=x_limit; //총 15줄인데 16개들어와봐. 15로설정해야지.
	}
	else
	{
		endOfBoard=processors;
	}
	parr=new Plane*[y_limit];//왼쪽오른쪽 추가로 둬야할까
	for(int q=0;q<y_limit;q++)
	{
		parr[q]=new Plane[persize];
	} 
	//나누기 안쓰고 곱하기로 쓰는게 더 낫겠다.
	string tempstr,sub;
	int tempx,tempy,current;
	while(1)
	{
		getline(inputfile,tempstr);
		if(inputfile.eof())
		{
			cout<<"finished"<<endl;
			break;
		}
		current=tempstr.find(' ');
		sub=inp.substr(0,current);
		tempx=stoi(sub);
		sub=inp.substr(current+1,tempstr.length()-current-1);
		tempy=stoi(sub);
		
	}
	inputfile.close();
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Status *statusarr, status;
	//MPI_Request request1, request2;
	MPI_Request *requestarr;
	statusarr=new MPI_Status [size];
	requestarr=new MPI_Request[size];
	//cout<<"size="<<size<<"rank="<<rank<<endl;

	printf("size=%d rank= %d \n",size,rank);
	char ** leftarr;
	char ** rightarr;
	leftarr=new char*[2];
	rightarr=new char*[2];
	for(int p=0;p<2;p++)
	{
		leftarr[p]=new char[MAXY];
		rightarr[p]=new char[MAXY];
	}
	//rightarr=new char(MAXY)[2];
	char buf[100];
	gethostname(buf,100);
	printf("hostname: %s \n",buf);

	int i,j;
	if(rank==0)
	{
	for(i=0;i<10;i++)
	{
		leftarr[0][i]=i;
	}//datasize==10
		MPI_Isend(leftarr[0],10,MPI_CHAR,1,1,MPI_COMM_WORLD,&requestarr[0]);
		//          data , size, type, to whom, tag, comm_channel, for check
		cout<<"why?"<<endl;
		MPI_Wait(&requestarr[0],&status);
	}
	else if(rank==1)
	{
		cout<<rank<<"recv"<<x_limit<<y_limit<<totaliter<<endl;
		MPI_Irecv(leftarr[0],10,MPI_CHAR,0,1,MPI_COMM_WORLD,&requestarr[1]);
		//for(j=0;j<2;j++)	
		cout<<"waiting"<<endl;
		MPI_Wait(&requestarr[1],&status);
		
		cout<<"finished"<<endl;

	}
	
	if(rank==1)
	{
		cout<<"rank==1"<<endl;
		for(i=0;i<10;i++)
		{
			if(i==leftarr[0][i])
			{
				cout<<i<<" ";
			}
		}
		cout<<endl;
	}
//	leftarr[0][4]=3;	if(rank==13)	{		leftarr[0][4]=13;	}
//	if(rank==leftarr[0][4])		cout<<rank<<"helloworld"<<endl; //okay 독립적
	//cout<<"myname="<<buf<<endl;
	delete[] statusarr;
	delete[] requestarr;
	for(int q=0;q<y_limit;q++)
	{
		delete[] parr[q];
	}
	delete[] parr;
	for(int p=0;p<2;p++)
	{
		delete[] leftarr[p];
		delete[] rightarr[p];
	}
	delete[] leftarr;
	delete[] rightarr;
	MPI_Finalize();
	return 0;
}
