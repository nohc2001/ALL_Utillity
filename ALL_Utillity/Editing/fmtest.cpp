#pragma optimize( "gs", off )

#include <iostream>
#include "Utill_FreeMemory.h"

struct AllocData {
	void* ptr;
	ui32 size;
};

#define HEAPDBG_MAXCAP 1024
AllocData heapDbg[HEAPDBG_MAXCAP];
ui32 up = 0;
void FMTest0 () {
	for (int i = 0; i < HEAPDBG_MAXCAP; ++i) {
		ui32 sel = rand() % 2;
		if (sel == 0) {
			if (up < HEAPDBG_MAXCAP) {
				ui32 r = 1 + (rand() % (SMALL_PAGE_SIZE-1));
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

	fm->dbg_fm1_lifecheck();
}

int main() {
	fmhl.Init();
	fm = (FM_System0*)fmhl.Allocate_ImortalMemory(sizeof(FM_System0));
	fm->SetHeapData();

	FMTest0();
}