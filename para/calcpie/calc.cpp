#include <iostream>
#include <cstdlib>
#include <ctime>
#define TEST 100000
int main()
{

double sum=0;
double x,y;
srand((unsigned int)time(NULL));
int i;
#pragma omp parallel \
 reduction (+ : sum) num_threads(8)
{
	sum = 0;
	#pragma omp for
	for (i = 0; i < TEST; i++) {
		x = ((double)rand() / RAND_MAX);
		y = ((double)rand() / RAND_MAX);
		//std::cout<<x;
		if ( x* x + y * y < 1.0)
			sum++;
	}

}

std::cout<<"ratio="<<((double)sum/8*TEST)*4.0<<std::endl;
}
