#include <mpi.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
using namespace std;

#define MAXY 10
int main(int argc,char* argv[])
{

	int size;
	int rank;

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
		cout<<rank<<"recv"<<endl;
		MPI_Irecv(leftarr[0],10,MPI_CHAR,0,1,MPI_COMM_WORLD,&requestarr[1]);
		//for(j=0;j<2;j++)	
		cout<<"waiting"<<endl;
		MPI_Wait(&requestarr[1],&status);
		
		cout<<"finished"<<endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);
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
