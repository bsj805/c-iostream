#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "skiplist.h"

using namespace std;

long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;


int main(int argc, char* argv[])
{
	skiplist<int, int> list(0,10000);
	int inp;
	while(true){

		cin>>inp;
		if(inp == -1)
		{
			break;
		}
		list.insert(inp,inp);
		cout<<list.printlevel() << endl;
	}

}
