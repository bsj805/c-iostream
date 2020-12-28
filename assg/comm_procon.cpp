#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <iostream>
#include <memory.h>
#include <unistd.h>
#include <limits.h>
#include <condition_variable>
#include <semaphore.h>
#include "skiplist.h"
#define THRNUM 2
using namespace std;
using std::thread;
using std::vector;
using std::mutex;
using std::condition_variable;

//char* acarr;
//long* numarr;
int crear,lrear;
int cfront,lfront;
int globalfin;
int opercount;
class cqueue{

	public:
	int rear =0;
	int front=0;
	int maxsize=0;
	char* acarr;
	long* numarr;
	
	cqueue(int size)
	{
		cout<<"size="<<size<<endl;
		maxsize=size;
		acarr=new char[size];
		numarr=new long[size];
	}
	~cqueue()
	{
		delete[] acarr;
		delete[] numarr;
	}
	bool isempty()
	{
		if(rear == front)
		{
			return true;
		}
		return false;
	}
	bool isfull()
	{
		if((rear+1)%maxsize == front)
		{
			return true;
		}
		return false;
	}
	int enqueue(char st,long num)
	{
		if(isfull()){
			//wait
			cout<<rear<<":rear " << front<<":front "<<endl;
			cout<<"queue full"<<endl;
			return -1;
		}
		else
		{
			rear=(rear+1)% maxsize;
			acarr[rear]=st;
			numarr[rear]=num;
			return 0;
		}
	}
	int dequeue(char* ps, long* ln)
	{
		if(isempty())
		{
			return -1;
		}
		else
		{
			front=(front+1)%maxsize;// 여기서 thread만들면될듯
			*ps=acarr[front];
			*ln=numarr[front];
			return 0;
		}
	}
};
/*
void get(int idx){
	m.lock();
	numarr[idx]=0;
	m.unlock();
}
void put(int idx,int val){
	
	m.lock();
	numarr[idx]=val;
	m.unlock();
}*/
vector<thread> threadarr;
mutex globalfinlock;
mutex skiplistlock;
mutex quelock;
mutex printlock;
mutex operlock;//opercount
int stopqueflag;
condition_variable cv;
condition_variable fullcv;
//std::unique_lock<std::mutex> lk(*m);

sem_t printsem;//insert가 1이면 풀어줄 것.
int workthread(cqueue* qu, skiplist<int,int>* skiparr,int num)
{
	char dec;
	long den;
	int i,j;
	int count=0;
	while(true)
	{
		cout<<num<<"count:"<<++count<<endl;
		if( globalfin==1)
		{
			cout<<num<<"globalfin"<<endl;
			quelock.lock();//dequeue하는동시에 이게 불리면 empty아닐지도
			if(qu->isempty())
			{
				quelock.unlock(); // insert 할때 check
				return 0; //thread exit
			}
			quelock.unlock();
		}
		/*
		if(stopqueflag==1)
		{
			quelock.lock(); // lock을 가져가니까 lock을가지려고해봐.
			quelock.unlock();
		}*/
		//cout<<"check que"<<endl;
		std::unique_lock<std::mutex> lk(quelock); //queue lock 
		cv.wait(lk,[&]{ return !qu->isempty() ||globalfin==1; }); //empty가 아니라면,또는 누가깨우면진행
		//cout<<"queue is not empty "<<count<<endl;
		if(globalfin==1)
		{

			cout<<num<<"globfin"<<endl;
			if(qu->isempty())
			{
				quelock.unlock();
				return 0;
			}
			
		}
		//else// dequeue
		
			if(qu->isempty())
			{
				cout<<num<<"I am empty but they tried to call me"<<endl;
				quelock.unlock();
				return 0;
			}
			qu->dequeue(&dec,&den);
			quelock.unlock();
///////////////////////////////////////semaphore
			operlock.lock();
			opercount++;
			if(opercount==1)
			{
				sem_wait(&printsem);
			}
			operlock.unlock();
/////////////////////////////////////////////
			cout<<num<<"inside thread "<<dec<<" "<<den<<endl;
			if(dec=='i')
			{
				cout<<num<<"I insert"<<den<<endl;
				skiplistlock.lock();
				skiparr->insert(den,den);
				skiplistlock.unlock();
			}
			else if(dec=='q')
			{
				cout<<num<<"I query"<<den<<endl;
				if(skiparr->find(den)!=den)
					cout<<num << "ERROR: Not Found: " << den << endl;
				else
				{
					cout<<num<<"FOUND"<<endl;
				}


			}
			else if(dec=='w')//w인경우. p는 queue에안넣어.
			{
				cout<<num<<"I sleep"<<den<<endl;
				usleep(den);
			}
			else
			{
				printf("ERROR: Unrecognized action: '%c'\n", dec);
				exit(EXIT_FAILURE);
			}
			///////////////////////semaphore
			operlock.lock();
			opercount--;
			if(opercount==0)
			{
				sem_post(&printsem);
			}
			operlock.unlock();
			fullcv.notify_one();
			///////////////////////semaphore
		
	
	}
}
//int opercount;//쓰는사람명수
int main(int argc, char* argv[])
{
	sem_init(&printsem,0,1);//
	struct timespec start, stop;
	skiplist<int, int> list(0,1000);
	globalfin=0; //이게 1되면 모든쓰레드종료
	int threadnum;
	char let;
	long lnu;
	//threadnum=THRNUM;
	//cqueue* one=new cqueue(threadnum+1);
	int i;
		/*
	one->enqueue('c',1);
	one->enqueue('d',2);
	cout<<one->acarr[one->rear]<<one->numarr[one->rear]<<endl;
	one->dequeue(&let,&lnu);
	one->dequeue(&let,&lnu);
	cout<<let<<lnu<<endl;
	one->enqueue('a',4);
	cout<<one->acarr[one->rear]<<one->numarr[one->rear]<<endl;
	one->dequeue(&let,&lnu);
	cout<<one->isempty()<<let<<lnu<<endl;
	*/
	clock_gettime( CLOCK_REALTIME, &start);
	/*if (argc !=2) {
		printf("Usage: %s <infile>\n",argv[0]);
		exit(EXIT_FAILURE);
	}*/
	
	if (argc !=3) {
		printf("Usage: %s <infile> <threadnum>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	threadnum=atoi(argv[2]);
	char *fn = argv[1];
	cout<<"thread="<<threadnum<<endl;
	cqueue* one=new cqueue(threadnum+1);
	for(i=0;i<threadnum;i++)
	{		
		threadarr.push_back(thread(workthread,one,&list,i));
	}

	//threadnum=THRNUM;
	opercount=0;//print해도되는지 check
	FILE* fin = fopen(fn, "r");
	char action;
	long num;
	while(fscanf(fin, "%c %ld\n", &action, &num) > 0) {
		cout<<"<<"<<action<<" "<<num<<">>" <<endl;
		//mutex ( full, empty)
		if(action=='p')
		{
			//여기서 try to acquire lock해야해. printmutex를 acquire하려고하고,
			//insertion할때는 printmutex를 가지고있는거지.(semaphore로하자)
			//그리고 마지막 insert==1 일때 printmutex를 unlock()해주면 얘가 동작
			//근데 printmutex가 들어온이상 다른 친구들이 새로 insert시작하면 
			//안되니까, 그거의 방지책으로, 여기서 stopqueflag=1을 걸어주고
			//insert,wait,query할 때 stopquelock==1을 기다리도록 해야겠어.
			//stopqueflag=1; 안필요한가봐
			quelock.lock();//아무도 새로 일을 못받아.
			//printlock.lock();//단순히 semaphore보호 이거하면 데드락. 아무도 세마포안풀어
			sem_wait(&printsem); //opercount==0이면 printsem풀어준다.
			
			cout << list.printList() <<endl;
			//stopqueflag=0;
			sem_post(&printsem); 
			quelock.unlock();
		}
		else
		{
			std::unique_lock<std::mutex> lk(quelock); 
			if(one->isempty())//최적화기법? inst하나줄이기?늘리는건가?
			{
				one->enqueue(action,num);
				//cv.notify_all();알아서 일어날걸? 조건정해져있어서.
			}
			else
			{
				fullcv.wait(lk,[&]{return !one->isfull();});
				one->enqueue(action,num);
			}
		
			quelock.unlock();
			cv.notify_one();
		}
	}
	fclose(fin);
	cout<<"@@@@@@@@@@@@"<<endl;
	globalfin=1;
	cv.notify_all();
	clock_gettime( CLOCK_REALTIME, &stop);
	for(i=0;i<threadnum;i++)
	{
		threadarr[i].join();
	}
	cout << "Elapsed time: " << (stop.tv_sec - start.tv_sec) + ((double) (stop.tv_nsec - start.tv_nsec))/BILLION << " sec" << endl;
	
	return (EXIT_SUCCESS);
	delete one;
}

