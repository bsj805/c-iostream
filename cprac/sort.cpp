#include <iostream>
using namespace std;
int partition(int *arr,int left, int right)
	{
		int lindex;
		int rindex;
		int pivot;
		int temp;
		pivot=left;
		lindex=left+1;
		rindex=right;
		while(lindex<=rindex)
		{

			while(lindex<=rindex && arr[lindex]<=arr[pivot])
			{
				lindex++;
			}

			while(lindex<=rindex && arr[rindex]>=arr[pivot])
			{
				rindex--;
			}
			if(lindex <= rindex)
			{
				temp=arr[lindex];
				arr[lindex]=arr[rindex];
				arr[rindex]=temp;
			}
		}
		temp=arr[pivot];
		arr[pivot]=arr[rindex];//pivot
		arr[rindex]=temp;
		return rindex;
	}
	void quicksort(int *arr, int left, int right)
	{
		int mid;int i;
		if(left<right)
		{
			mid=partition(arr,left,right);
			quicksort(arr,left,mid-1);
			quicksort(arr,mid+1,right);
		}

	}
void insertsort(int* arr,int pos);
void insertionsort(int* arr,int pos);
void newinsort(int* arr,int len)
{
	for(int i=1;i<len;i++)
	{
		int temp=arr[i];
		int j;
		for(j=i;j>0 && arr[j-1] > temp ; j--)
		{
			arr[j]=arr[j-1];
		}
		arr[j]=temp;
	}
}
void insertionsort(int* arr, int len)
{
	for (int i=1; i<len;i++)
	{
		insertsort(arr,i);
	}

}
void insertsort(int* arr,int pos)
{
	int temp;
	if(arr[pos]>=arr[pos-1] && pos >0)
		return;
	for(int i= pos -1; i>=0 ; i--){
		if (arr[pos]<arr[i])
		{
			temp=arr[pos];
			arr[pos]=arr[i];
			arr[i]=temp;
			pos=i;
		}
	}
}
int main()
{
	int *arr;
	int i;
	int aar[5];
	aar[0]=7;
	aar[1]=3;
	aar[2]=1;
	aar[3]=6;
	aar[4]=2;
	for(i=0;i<5;i++)
	{
		cout<<aar[i]<<" ";
	}
	cout<<endl;
	newinsort(aar,5);
	//quicksort(aar,0,4);
	for(i=0;i<5;i++)
	{
		cout<<aar[i]<<" ";
	}
	cout<<endl;

	arr=new int(6);
	arr[0]=6;
	arr[1]=2;
	arr[2]=8;
	arr[3]=1;
	arr[4]=2;
	arr[5]=7;
	for(i=0;i<6;i++)
	{
		cout<<arr[i]<<" ";
	}
	cout<<endl;
	newinsort(arr,6);
	//quicksort(arr,0,5);
	
	for(i=0;i<6;i++)
	{
		cout<<arr[i]<<" ";
	}
	cout<<endl;
}
