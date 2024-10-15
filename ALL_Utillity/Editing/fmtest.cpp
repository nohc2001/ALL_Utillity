#pragma optimize( "gs", off )

#include <iostream>
#include "Utill_FreeMemory.h"

struct AllocData {
	void* ptr;
	ui32 size;
};

#define HEAPDBG_MAXCAP 4096
AllocData heapDbg[HEAPDBG_MAXCAP];
ui32 up = 0;

//fmtest 0 : random allocate and free memory 4096 times
//fastnew :		1814934 clock (same work - rand() value is same)
//malloc :		3904527 clock (same work - rand() value is same)
void FMTest0 () {
	ui64 t0 = 0;
	ui64 t1 = 0;
	
	t0 = __rdtsc();
	for (int i = 0; i < HEAPDBG_MAXCAP; ++i) {
		ui32 sel = rand() % 2;
		if (sel == 0) {
			if (up < HEAPDBG_MAXCAP) {
				ui32 r = 1 + (rand() % (SMALL_PAGE_SIZE - 1));
				heapDbg[up].ptr = malloc(r);
				heapDbg[up].size = r;
				up += 1;
			}
		}
		else {
			if (up == 0) continue;
			ui32 r = rand() % up;
			free(heapDbg[r].ptr);
			for (int k = r; k < up; ++k)
			{
				heapDbg[k] = heapDbg[k + 1];
			}
			up -= 1;
		}
	}

	for (int i = up - 1; i >= 0; --i) {
		free(heapDbg[i].ptr);
	}
	up = 0;
	
	t1 = __rdtsc();
	cout << t1 - t0 << endl;

	t0 = __rdtsc();
	for (int i = 0; i < HEAPDBG_MAXCAP; ++i) {
		ui32 sel = rand() % 2;
		if (sel == 0) {
			if (up < HEAPDBG_MAXCAP) {
				ui32 r = 1 + (rand() % (SMALL_PAGE_SIZE - 1));
				heapDbg[up].ptr = fm->_fastnew(r);
				heapDbg[up].size = r;
				up += 1;
			}
		}
		else {
			if (up == 0) continue;
			ui32 r = rand() % up;
			fm->_Delete(heapDbg[r].ptr, heapDbg[r].size);
			for (int k = r; k < up; ++k)
			{
				heapDbg[k] = heapDbg[k + 1];
			}
			up -= 1;
		}
	}

	for (int i = up - 1; i >= 0; --i) {
		fm->_Delete(heapDbg[i].ptr, heapDbg[i].size);
	}
	up = 0;
	t1 = __rdtsc();
	cout << t1 - t0 << endl;
}

//fmtest 1 : single allocate speed measurement. 
void FMTest1(){
	ui64 t0 = 0;
	ui64 t1 = 0;
	void* ptr = nullptr;

	t0 = __rdtsc();
	ptr = fm->_inline_fastNew(1000);
	ptr = fm->_inline_fastNew(1000);
	t1 = __rdtsc();
	cout << t1 - t0 << endl;

	t0 = __rdtsc();
	ptr = malloc(1000);
	ptr = malloc(1000);
	t1 = __rdtsc();
	cout << t1 - t0 << endl;
}
int main() {
	fmhl.Init();
	fm = (FM_System0*)fmhl.Allocate_ImortalMemory(sizeof(FM_System0));
	fm->SetHeapData();

	FMTest0();

	fm->dbg_fm1_lifecheck();
	fm->RECORD_NonReleaseHeapData();
}