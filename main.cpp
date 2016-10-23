#include <iostream>
#include <cmath>
#include <omp.h>
#include <queue>
#include <stack>

#define a 1.0
#define b 100.0
#define s 12
#define e 1.0e(-6)
#define thread_num 56

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
	
	double interval_length = b - a;
	
	//manager to generate works 
	//breadth first search
	while (interval_length>1.0e-2)
	{
		interval old = works.pop();
		double fstart = f(old.start);
		double fmid = f((old.end+old.start)/2);
		double fend = f(old.end);
		
		if (fstart + fmid + s*(old.end-old.start)/4>old.max)
		{
			interval new1(old.start, (old.end+old.start)/2, fstart > fmid? fstart : fmid);
			works.push(new1);
		}
		
		if (fmid + fend + s*(old.end-old.start)/4>old.max)
		{
			interval new2((old.end+old.start)/2, old.end, fmid > fend? fmid : fend);
			works.push(new2);
		}
		
		interval_length = (old.end - old.start)/2;
		
	}
	
	//parallel part
	//send works to workers
	//each worker use a depth first search
	int num_works = works.size();
	
	#pragma omp parallel num threads(thread_num)
	{
		#pragma omp for schedule(dynamic, 1)
		for (int i=0; i<number_works; i++)
		{
			stack <interval> work;
			
			#pragma omp critical(thread_lock)
			interval thread_work = works.pop();
			
			work.push(thread_work);
			
			while (interval_length>=1.0e-6)
			{
				interval old = work.pop();
				double fstart = f(old.start);
				double fmid = f((old.end+old.start)/2);
				double fend = f(old.end);
				
				if (fstart + fmid + s*(old.end-old.start)/4>old.max)
				{
					interval new1(old.start, (old.end+old.start)/2, fstart > fmid? fstart : fmid);
					work.push(new1);
				}
		
				if (fmid + fend + s*(old.end-old.start)/4>old.max)
				{
					interval new2((old.end+old.start)/2, old.end, fmid > fend? fmid : fend);
					work.push(new2);
				}	
				
				interval_length = (old.end - old.start)/2;
			}	
			
			int count = work.size();
			
			double max_num = work.pop().max;
			for (int j=0; j<count; j++)
			{
				double temp = work.pop().max;
				if (temp > max_num)
				{
					max_num = temp;
				}
			}
			
			thread_work.max = max_num;
			
			#pragma omp critical(thread_lock)
			works.push(thread_work);	
		}
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
