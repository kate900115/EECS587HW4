#include <iostream>
#include <cmath>
#include <omp.h>
#include <queue>
#include <stack>
#include <vector>

#define a 1.0
#define b 100.0
#define s 12
#define E 1.0e-6
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
		interval (double S=0, double EE=0, double FS=0, double FE=0){start = S; end = EE; f_start = FS; f_end = FE;}
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
	while (interval_length>1.0e-1)
	{
		interval old = works.front();
		works.pop();
		double fstart = old.f_start;
		double fend = old.f_end;
		double fmid = f((old.end+old.start)/2);
		
		double max3 = fstart > fmid? fstart: fmid;
		max3 = max3 > fend ? max3 : fend;
		max = max3 > max? max3 : max;
		
		if ((fstart + fmid)/2 + s*(old.end-old.start)/4>=max+E)
		{
			interval new1(old.start, (old.end+old.start)/2, fstart, fmid);
			works.push(new1);
		}
		
		if ((fmid + fend)/2 + s*(old.end-old.start)/4>=max+E)
		{
			interval new2((old.end+old.start)/2, old.end, fmid, fend);
			works.push(new2);
		}
		
		interval_length = (old.end - old.start)/2;
	}
	cout<<"manager done!!"<<endl;	
	cout<<"max = "<<max<<endl;
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
			//#pragma omp critical(cout)
			//cout<<"Thread ID = "<<threadID<<endl;

			stack <interval> work;
			interval thread_work;

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
				double fstart = old.f_start;
				double fend = old.f_end;
				double fmid = f((old.end+old.start)/2);

				//#pragma omp critical(cout)
				//cout<<"threadID = "<<threadID<<"aaaaaaaaaaaaaa"<<endl;
				double max3 = fstart>fmid? fstart : fmid;
				       max3 = max3>fend? max3 : fend;
				#pragma omp critical(update_max)
				{
					if (max3>max)
					{
						max = max3;
					}
				}
					
				if ((fstart + fmid)/2 + s*(old.end-old.start)/4>= max+E)
				{
					interval new1(old.start, (old.end+old.start)/2, fstart, fmid);
					work.push(new1);
				}
			
				if ((fmid + fend)/2 + s*(old.end-old.start)/4>= max+E)
				{
					interval new2((old.end+old.start)/2, old.end, fmid, fend);
					work.push(new2);
				}	
				//interval_length = (old.end - old.start)/2;
			}
			
			#pragma omp critical (cout)
			cout<<"temp max = "<<max<<endl;	
			//works.push(thread_work);		
		}
		
		
	}		
	

	
	cout<<"the max number = "<<max<<endl;
	
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
