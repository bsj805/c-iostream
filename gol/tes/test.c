#include <mpi.h>
#include <unistd.h>
//#include <iostream>
#include <stdio.h>
//using namespace std;

int main(int argc,char* argv[])
{

	int size;
	int rank;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	//cout<<"size="<<size<<"rank="<<rank<<endl;
	printf("size= %d rank= %d \n",size,rank);
	char **arr;
	char *leftarr;
	char *rightarr;
	char buf[100];
	gethostname(buf,100);
	printf("hostname: %s \n",buf);
	//cout<<"myname="<<buf<<endl;
	MPI_Finalize();
	return 0;
}
