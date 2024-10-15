#ifndef H_ARR_EXPEND
#include "arr_expend.h"
#endif

#ifndef H_UTILL_FREEMEMORY
#include "Utill_FreeMemory.h"
#endif

#define H_MULTIFOR

using namespace freemem;

int get_total_loop(fmvecarr<int>* sizes){
	int total = 1;
	for(int i=0;i<sizes->size();++i){
		total *= sizes->at(i);
	}
	return total;
}

int get_index(fmvecarr<int>* sizes, int n, int realindex){
	int mul = 1;
	for(int i=0;i<n-1;++i){
		mul *= sizes->at(i);
	}
	return (realindex / mul) % sizes->at(n);
}