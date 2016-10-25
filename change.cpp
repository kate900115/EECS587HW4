#include <iostream>
#include <cmath>
#include <omp.h>
#include <queue>
#include <stack>
#include <vector>

#define a 1.0
#define b 100.0
#define s 12
#define e 1.0e-6
#define thread_num 56

using namespace std;

double f (double x);
class interval
{
	public:
		double start;
		double end;
		double f_start;
		double f_end;
		interval (double S=0, double E=0, double FS=0, double FE=0){start = S; end = E; f_start = FS; f_end = FE;}
};
	
int main()
{
	queue <interval> works;
	vector <double> max_array;
	
	double fa = f(a);
	double fb = f(b);
	
	cout<<"f(a)="<<fa<<",f(b)="<<fb<<endl;
	
	interval setup(a, b, fa, fb);
	works.push(setup);
	
	double max = fa>fb? fa:fb;
	
	double interval_length = b - a;
	
	//manager to generate works 
	//breadth first search
	while (interval_length>1.0e-2)
	{
		interval old = works.front();
		works.pop();
		double fstart = old.f_start;
		double fend = old.f_end;
		double fmid = f((old.end+old.start)/2);

		
		if (fstart + fmid + s*(old.end-old.start)/4>(old.f_start>old.f_end?old.f_start:old.f_end))
		{
			interval new1(old.start, (old.end+old.start)/2, fstart, fmid);
			works.push(new1);
		}
		
		if (fmid + fend + s*(old.end-old.start)/4>(old.f_start>old.f_end?old.f_start:old.f_end))
		{
			interval new2((old.end+old.start)/2, old.end, fmid, fend);
			works.push(new2);
		}
		
		interval_length = (old.end - old.start)/2;
		
	}
	cout<<"manager done!!"<<endl;	
	//parallel part
	//send works to workers
	//each worker use a depth first search
	int num_works = works.size();
	
	cout<<"work size = "<<num_works<<endl;
	
	#pragma omp parallel num_threads(thread_num)
	{
		#pragma omp for schedule(dynamic, 1)
		for (int i=0; i<num_works; i++)
		{
			int threadID = omp_get_thread_num();
			#pragma omp critical(cout)
			cout<<"Thread ID = "<<threadID<<endl;

			stack <interval> work;
			interval thread_work;
			double max_num = max;

			//get the job from manager
			#pragma omp critical(thread_lock)
			{
				thread_work = works.front();
				works.pop();
			}
			work.push(thread_work);
			
			int number=0;//for test
			while (work.size())
			{
				number++;//for test
				#pragma omp critical(cout)
				cout<<"threadID = "<<threadID<<", loop times = "<<number<<endl;
				
				interval old = work.top();
				work.pop();
				if (old.end-old.start>=2.0e-6)
				{
					double fstart = old.f_start;
					double fend = old.f_end;
					double fmid = f((old.end+old.start)/2);

					//#pragma omp critical(cout)
					//cout<<"threadID = "<<threadID<<"aaaaaaaaaaaaaa"<<endl;
				
					if (fstart + fmid + s*(old.end-old.start)/4>(old.f_start>old.f_end?old.f_start:old.f_end))
					{
						interval new1(old.start, (old.end+old.start)/2, fstart, fmid);
						work.push(new1);
					}
			
					if (fmid + fend + s*(old.end-old.start)/4>(old.f_start>old.f_end?old.f_start:old.f_end))
					{
						interval new2((old.end+old.start)/2, old.end, fmid, fend);
						work.push(new2);
					}	
				
					if (work.size()==0)
					{
						break;
					}
					interval_length = (old.end - old.start)/2;
				}
				else
				{
					double max_temp = old.f_start > old.f_end ? old.f_start : old.f_end;
					if (max_num < max_temp)
					{
						max_num = max_temp;
					}					
				}		
			}
			
			#pragma omp critical(vector_lock)
			max_array.push_back(max_num);
			//works.push(thread_work);	
		}		
	}
	
	int size = max_array.size();
	double init_max = max_array.back();
	max_array.pop_back();
	for (int i=0; i<size-1; i++)
	{
		double temp = max_array.back();
		max_array.pop_back();
		if (temp>init_max)
		{
			init_max = temp;
		}
	}
	
	cout<<"the max number = "<<init_max<<endl;
	
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
