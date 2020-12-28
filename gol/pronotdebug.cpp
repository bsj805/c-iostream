#include <mpi.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <string.h>
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
	//Plane **totalarr;
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
	int dirx={-1,0,1,-1,1,-1,0,1};
	int diry={-1,-1,-1,0,0,1,1,1};
	
	int *xstk, *ystk;
	int stksize,top;
	
	int persize;
	persize=(x_limit/processors)+1; //
	int endOfBoard;
	stksize=x_limit*y_limit;
	if(persize==1)
	{
		endOfBoard=x_limit; //총 15줄인데 16개들어와봐. 15로설정해야지.//즉 실제 데이터갖는 프로세서가 이거임.
	}
	else
	{
		endOfBoard=processors;
	}
	
	xstk=new int[stksize];
	ystk=new int[stksize];
	top=-1;
	//totalarr=new Plane*[y_limit];
	parr=new Plane*[y_limit];//왼쪽오른쪽 추가로 둬야할까
	for(int q=0;q<y_limit;q++)
	{
		parr[q]=new Plane[persize];//class 생성자호출
		//memset(parr[q],0,sizeof(parr[q]));초기화되어있을거
//	totalarr[q]=new Plane[x_limit]; 
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
		//totalarr[tempy][tempx].isalive=true;
		xstk[++top]=tempx;
		ystk[top]=tempy;
		
	}
	inputfile.close();
	
	MPI_Status *statusarr, status;
	//MPI_Request request1, request2;
	MPI_Request *requestarr;
	statusarr=new MPI_Status [size];
	requestarr=new MPI_Request[size];
	//cout<<"size="<<size<<"rank="<<rank<<endl;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	printf("size=%d rank= %d \n",size,rank);
	char ** leftarr;
	char ** rightarr;
	leftarr=new char*[2];
	rightarr=new char*[2];
	for(int a=0;a<2;a++)
	{
		leftarr[a]=new char[y_limit];
		rightarr[a]=new char[y_limit];

	}
	memset(leftarr[0],0,sizeof(leftarr[0]));
	memset(leftarr[1],0,sizeof(leftarr[1]));
	memset(rightarr[1],0,sizeof(rightarr[1]));
	memset(rightarr[0],0,sizeof(rightarr[0]));
	//rightarr=new char(MAXY)[2];
	char buf[100];
	gethostname(buf,100);
	printf("hostname: %s \n",buf);

	int i,j,nowx,m,n,curx,cury,curcount;
	bool curstat;
	for(i=0;i<totaliter;i++)//반복하라는 횟수만큼반복하는데0번째빼고는
	{//다 다른 stack을 쓰게되는거지.
		if(i==0)
		{
			while(top>-1)
			{
				tempx=xstk[top];
				tempy=ystk[top--];
				nowx=tempx-(persize*rank);
				if(nowx <persize && nowx>0 )
				{
					parr[tempy][nowx].isalive=true;
					for(m=0;m<8;m++)
					{
						curx=nowx+dirx[m];
						cury=tempy+diry[m];
						if(cury>=y_limit || cury <0)
						{
							continue;
						}
						if(curx<0)
						{
							leftarr[cury]++;
						}
						else if(curx==persize)
						{
							rightarr[cury]++;
						}
						else
						{
							parr[cury][curx].count++;
						}
					}
					
					//이제count추가
				}// persize안에 있다면
			}
		}//if 0번째 실행할때엔
		else//다음시행부터는 그냥 
		{
			while(top>-1)
			{
				tempx=xstk[top];
				tempy=ystk[top--];
				nowx=tempx;
				parr[tempy][tempx].isalive=true;
				for(m=0;m<8;m++)
				{
					curx=nowx+dirx[m];
					cury=tempy+diry[m];
					if(cury>=y_limit || cury <0)
					{
						continue;
					}
					if(curx<0)
					{
						leftarr[cury]++;
					}
					else if(curx==persize) //persize-1 ==rank인 애여도 상관없지. 어차피 안써 이정보.
					{
						rightarr[cury]++;
					}
					else
					{
						parr[cury][curx].count++;
					}
				}
					
					//이제count완료
			}//while stack not empty
		}//if 0번째실행아니면 끝
		
		//이제 leftarr ,rightarr보내기

		for(j=0;j<endOfBoard;j++)
		{
			if(rank!=0)
			{
				MPI_Isend(leftarr[0],y_limit,MPI_CHAR,rank-1,i,MPI_COMM_WORLD,&requestarr[0]);
			}
			if(rank!=endOfBoard-1)//오른쪽으로보내
			{
				MPI_Isend(rightarr[0],y_limit,MPI_CHAR,rank+1,i+1,MPI_COMM_WORLD,&requestarr[1]);

			}
			if(rank!=0)//left로부터 받을애들
			{
				MPI_Isend(leftarr[1],y_limit,MPI_CHAR,rank-1,i+1,MPI_COMM_WORLD,&requestarr[2]);

			}
			if(rank!=endOfBoard-1)//right로부터받을애들
			{
				MPI_Isend(rightarr[1],y_limit,MPI_CHAR,rank+1,i,MPI_COMM_WORLD,&requestarr[3]);
			}
//아마 문제가 생긴다면 wait할때 rank==0이나 rank==endOfBoard-1 인애들은 
//requestarr[0]을 안가지고 있기 때문일거야. 해당애들은 제외시켜주자. wait확인시에
		}
		//여기서 memset하면 제대로된 데이터 안 전송된다.
		top=-1;
		if(persize>2)//경곗값 필요없는애들에 대한 연산진행
		{
			for(int oloo=0;oloo<y_limit;oloo++)//n o more variable name to use
			{
				for(int iloo=1;iloo<persize-1;iloo++)//livecell check하기 curstat curcount
				{

					curstat=parr[oloo][iloo].isalive;//alive==true
					curcount=parr[oloo][iloo].count;
					parr[oloo][iloo].count=0;
					if(curstat==false && curcount==3)
					{
						parr[oloo][iloo].isalive=true;
						ystk[++top]=oloo;
						xstk[top]=iloo;
					}
					else if(curstat==false) continue;
					else//alive cells
					{
						if(curcount>3)
						{
							parr[oloo][iloo].isalive=false;
						}
						else if(curcount>1)
						{
							ystk[++top]=oloo;
							xstk[top]=iloo;
							continue;//계속살아계시면됩니다.
						}
						else
						{
							parr[oloo][iloo].isalive=false;
						}
					}
				}
			}
		}
		//do something useful

		for(n=0;n<4;n++)
		{
			if(rank==0 && (n==0 || n==2))
			{
				continue;
			}
			if(rank==endOfBoard-1 && (n==1 || n==3))
			{
				continue;
			}
			MPI_Wait(&requestarr[n],&status);
		}
		
		for(int outloop=0;outloop<y_limit;outloop++)//leftarr,rightarr
		{
			parr[outloop][0].count+=leftarr[1][outloop];
			parr[outloop][persize-1].count+=rightarr[1][outloop];

			curstat=parr[outloop][0].isalive;
			curcount=parr[outloop][0].count;
			parr[outloop][0].count=0;
			if(curstat==false && curcount==3)
			{
				parr[outloop][0].isalive=true;
				ystk[++top]=outloop;
				xstk[top]=0;
			}
			else if(curstat==false) continue;
			else//alive cells
			{
				if(curcount>3)
				{
					parr[outloop][0].isalive=false;
				}
				else if(curcount>1)
				{
					
					ystk[++top]=outloop;
					xstk[top]=0;
					continue;//계속살아계시면됩니다.
				}
				else
				{
					parr[outloop][0].isalive=false;
				}
			}
			if(persize-1 !=0)
			{
				curstat=parr[outloop][persize-1].isalive;
				curcount=parr[outloop][persize-1].count;
				parr[outloop][persize-1].count=0;
				if(curstat==false && curcount==3)
				{
					parr[outloop][persize-1].isalive=true;
					ystk[++top]=outloop;
					xstk[top]=persize-1;
				}
				else if(curstat==false) continue;
				else//alive cells
				{
					if(curcount>3)
					{
						parr[outloop][persize-1].isalive=false;
					}
					else if(curcount>1)
					{
						
						ystk[++top]=outloop;
						xstk[top]=persize-1;
						continue;//계속살아계시면됩니다.
					}
					else
					{
					parr[outloop][persize-1].isalive=false;
					}
				}

			}
		}
		memset(leftarr[1],0,sizeof(leftarr[1]));
		memset(rightarr[1],0,sizeof(rightarr[1]));
		

	}//totaliter for문끝이제 모든실행완료한거.
	while(top>-1)
	{
		tempx=xstk[top];
		tempy=ystk[top--];
		cout<<tempx<<" "<<tempy<<endl;
	}
	/*if(rank==0)
	{
	for(i=0;i<10;i++)
	{
		leftarr[i]=i;
	}//datasize==10
		MPI_Isend(leftarr,10,MPI_CHAR,1,1,MPI_COMM_WORLD,&requestarr[0]);
		//          data , size, type, to whom, tag, comm_channel, for check
		cout<<"why?"<<endl;
		MPI_Wait(&requestarr[0],&status);
	}
	else if(rank==1)
	{
		cout<<rank<<"recv"<<x_limit<<y_limit<<totaliter<<endl;
		MPI_Irecv(leftarr,10,MPI_CHAR,0,1,MPI_COMM_WORLD,&requestarr[1]);
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
			if(i==leftarr[i])
			{
				cout<<i<<" ";
			}
		}
		cout<<endl;
	}*/

//	leftarr[0][4]=3;	if(rank==13)	{		leftarr[0][4]=13;	}
//	if(rank==leftarr[0][4])		cout<<rank<<"helloworld"<<endl; //okay 독립적
	//cout<<"myname="<<buf<<endl;
	delete[] statusarr;
	delete[] requestarr;
	delete[] xstk;
	delete[] ystk;
	for(int q=0;q<y_limit;q++)
	{
		//delete[] totalarr[q];
		delete[] parr[q];
	}
	delete[] parr;
	//delete[] totalarr;
	delete[] leftarr[0];
	delete[] leftarr[1];
	delete[] rightarr[0];
	delete[] rightarr[1];
	delete[] leftarr;
	delete[] rightarr;
	MPI_Finalize();
	return 0;
}
