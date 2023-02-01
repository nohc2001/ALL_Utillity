#include <iostream>
#include <time.h>
#include "Utill_SeekUnknown.h"
#include "Utill_FreeMemory.h"

using namespace seek_unknown;

constexpr int pageSize = 4096;
constexpr int repeatNum = 3000;
constexpr int maxmemsiz = 20;

//지금 4~40 바이트 사이즈의 메모리 할당이 참 느린데 이건 FM_Model1에서 lifecheck가 체크하는 바이트를 늘려서 극복가능할 것 같음.
//어딘가에 비용함수가 있었음.

int main() {
	freemem::FM_System0 heap_system;
	heap_system.SetHeapData(pageSize, pageSize, 2* pageSize, 2* pageSize);

	int sum = 0;
	clock_t pt, st;
	st = clock();
	for (int i = 0; i < repeatNum; ++i) {
		byte8* b = heap_system._New(1+rand() % maxmemsiz, true);
		*b = i;
	}
	pt = clock();
	cout << "\n heapdebug : " << pt - st << endl;

	st = clock();
	for (int i = 0; i < repeatNum; ++i) {
		byte8* b = ((byte8*)malloc(1+rand() % maxmemsiz));
		*b = i;
	}
	pt = clock();
	cout << "\n malloc : " <<  pt - st << endl;

	st = clock();
	for (int i = 0; i < repeatNum; ++i) {
		byte8* b = heap_system._New(1+rand() % maxmemsiz, false);
		*b = i;
	}
	pt = clock();
	cout << "\n notdebug : " << pt - st << endl;

	return 0;
}