#include <math.h>
#include "arr_expend.h"

typedef struct VP
{
	char mod = 0;	// mod 0:value 1:ptr
	int *ptr = nullptr; // arrgraph ptr or T ptr
};

template <typename T, typename V> struct range
{
	T end;
	V value;
};

template <typename T, typename V> class ArrGraph
{
  public:
	vecarr < range<T, V> > *ranges;
	T minx = 0;
	T maxx = 0;
	T margin = 0;
	bool islocal = false;
	vecarr < VP > graph;

	ArrGraph()
	{
	}
	virtual ~ ArrGraph()
	{
		if (islocal)
		{
			graph.release();
			graph.NULLState();
		}
	}

	ArrGraph *Init(T min, T max)
	{
		minx = min;
		maxx = max;
		ranges = (vecarr < range< T, V > > *)malloc(sizeof(vecarr < range< T, V > >));
		ranges->NULLState();
		ranges->Init(2);
		islocal = false;
	}
	
	range< T, V > Range(T end, V value){
		range<T, V> r;
		r.end = end;
		r.value = value;
		return r;
	}

	void push_range(range< T, V > r)
	{
		if (minx <= r.end && r.end <= maxx)
		{
			ranges->push_back(r);
		}
	}

	void Compile()
	{
		if (ranges->size() > 2)
		{
			float d = (float)(maxx - minx);
			float div = (float)ranges->size();
			float f = d / div;
			f = floor(f)+1;
			T average_length = (T)(f);
			margin = average_length;
			graph.NULLState();
			graph.Init(ranges->size());
			graph.up = ranges->size();
			T start = minx;
			T end = start;
			for (int i = 0; i < graph.up; ++i)
			{
				end = start + average_length;
				if(end > maxx) end = maxx;
				T rstart = minx;
				for (int k = 0; k < ranges->up; ++k)
				{
					T rend = ranges->at(k).end;
					if (rstart <= start && end <= rend)
					{
						// num
						graph[i].mod = 0;
						graph[i].ptr = reinterpret_cast<int*>(&ranges->at(k).value);
						break;
					}
					else if (start <= rend && rend <= end)
					{
						// graph
						ArrGraph<T, V> *newgraph = (ArrGraph<T, V> *) malloc(sizeof(ArrGraph<T,V>));
						newgraph->Init(start, end);
						newgraph->push_range(ranges->at(k));
						range< T, V > *r = &ranges->at(k + 1);
						while (r->end < end)
						{
							newgraph->push_range(*r);
							++k;
							if (k >= ranges->size())
							{
								break;
							}
							r = &ranges->at(k+1);
						}
						//input last range
						range<T, V> lastr;
						lastr = *r;
						lastr.end = newgraph->maxx;
						newgraph->push_range(lastr);
						newgraph->Compile();
						graph[i].ptr = reinterpret_cast < int *>(newgraph);
						graph[i].mod = 1;
						break;
					}
				}
				start = end;
			}
		}
		else if (ranges->size() == 2)
		{
			graph.NULLState();
			graph.Init(2);
			T center = ranges->at(0).end;
			T start = minx;
			T end = maxx-1;
			if(maxx - center > center - start){
				minx = 2*center+1 - end;
			}
			else{
				maxx = 2*center+1-start;
			}
			margin = (maxx - minx) / ranges->size();
			VP vp0;
			vp0.mod = 0;
			vp0.ptr = reinterpret_cast<int*>(&ranges->at(0).value);
			graph.push_back(vp0);
			vp0.ptr = reinterpret_cast<int*>(&ranges->at(1).value);
			graph.push_back(vp0);
		}
	}

	T fx(T x)
	{
		static constexpr void* jumpptr[2] = {&&ISVALUE, &&ISGRAPH};
		ArrGraph<T, V>* ag = this;
		vecarr < VP >* g = &graph;
		VP vp;
		float f = 0;
		int index = 0;
		
		GET_START:
		f = (float)x - (float)ag->minx;
		f = f / (float)ag->margin;
		index = (int)f;
		
		vp = (*g)[index];
		goto *jumpptr[vp.mod];
		
		ISGRAPH:
		ag = reinterpret_cast<ArrGraph<T, V>*>(vp.ptr);
		g = &ag->graph;
		goto GET_START;
		
		ISVALUE:
		return *reinterpret_cast<V*>(vp.ptr);
	}
	
	void print_state(){
		cout << "arrgraph minx : " << minx << "\t maxx : " << maxx << endl;
		cout << "capacity : " << graph.size() << "\t margin : " << margin << endl;
		for(int i=0;i<graph.size();++i){
			if(graph[i].mod == 0){
				cout << "index : " << i << "] = " << *reinterpret_cast<V*>(graph[i].ptr) << endl;
			}
			else{
				cout << "index : " << i << "] = ptr : " << endl;
				reinterpret_cast<ArrGraph<T, V>*>(graph[i].ptr)->print_state(); 
				cout << endl;
			}
		}
	}
};

int getcost(int n, int size){
	int k = (n%size == 0) ? 0 : 1;
	return (8*size+1) * (n/size + k);
}

int minarr(int siz, int* arr, int* indexout){
	int min = arr[0];
	for(int i=0;i<siz;++i){
		if(min > arr[i]){
			min = arr[i];
			*indexout = i;
		}
	}
	return min;
}

int main(){
    ArrGraph<unsigned int, unsigned int> sizeGraph;
    sizeGraph.Init(1, 4096);
	int sizearr[13] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    range<unsigned int, unsigned int> currange;
	for(int i=1;i<4096;++i){
		int res[13] = {};
		for(int k=0;k<13;++k){
			res[k] = getcost(i, sizearr[k]);
		}
		int mini = 0;
		int min = minarr(13, res, &mini);
		cout << i << "\t" << sizearr[mini] << " : \t" << (float)res[mini] / 8.0f << "(" << res[mini] << ")\t" << "additional bit : " << res[mini] - i*8 << endl;

        if(currange.value == sizearr[mini]){
            continue;
        }
        else{
            currange.end = i-1;
            sizeGraph.push_range(currange);
            currange.end = 0;
            currange.value = sizearr[mini];
        }
	}

    currange.end = 4096;
    sizeGraph.push_range(currange);
    sizeGraph.Compile();
    sizeGraph.print_state();
	return 0;
}