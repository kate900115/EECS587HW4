#include <iostream>
#include <cmath>
#include <omp>

#define a 1
#define b 100
#define s 12

using namespace std;

double f (double x);

int main()
{
	return 0;
}

double f(double x)
{
	double sumj=0;
	double sumi=0;
	for (int i=1; i<101; i++)
	{
		for (int j=1; j<i+1; j++)
		{
			sumj = sumj + pow((x+0.5*j), -3.3);	
		}	
		sumi = sumi + sin(sumj)/(pow(1.3,i));
	}

	return sumi;
}
