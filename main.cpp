#include <iostream>
#include <cmath>
#include <omp.h>
#include <queue>

#define a 1.0
#define b 100.0
#define s 12
#define e 1.0e(-10)

using namespace std;

double f (double x);



int main()
{

	struct interval
	{
		double start;
		double end;
		double max;
	};
	queue <interval> works;
	double fa = f(a);
	double fb = f(b);
	double max = fa>fb? fa:fb;
	interval setup(a, b, max);
	works.push(setup);
	
	double start1;
	double end1;
	double max1;
	double start2;
	double end2;
	double max2;
	
	double interval_length;
	
	while 
	{
		interval old = works.pop();
		double fstart = f(old.start);
		double fmid = f((old.end+old.start)/2);
		double fend = f(old.end);
		if (fstart + fmid + s*(end-start)/4>old.max)
		{
			interval new1(old.start, (old.end+old.start)/2, fstart > fmid? fstart : fmid);
			works.push(new1);
		}
		
		if (fmid + fend + s*(end-start)/4>old.max)
		{
			interval new2((old.end+old.start)/2, old.end, fmid > fend? fmid : fend);
			works.push(new2);
		}
		
		interval_max new_interval(start,end,max);
		works.push(new_interval);
		
	}
	return 0;
}

//should not change
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
