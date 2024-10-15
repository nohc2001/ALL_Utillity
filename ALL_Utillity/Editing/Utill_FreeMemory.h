#ifndef H_UTILL_FREEMEMORY
#define H_UTILL_FREEMEMORY

#define FM_GET_NONRELEASE_HEAPPTR
//#define FM_NONRELEASE_HEAPCHECK

#include <math.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;
typedef unsigned char byte8;
typedef unsigned char ui8;
typedef unsigned short ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;
typedef char si8;
typedef short si16;
typedef int si32;
typedef long long si64;

#ifdef __cplusplus
//C++ Compiler"
#else
//This is not C++ compiler."
#endif

#ifdef __STDC__
//Standard C compatible compiler"
#endif

#if defined (_MSC_VER)
//This is Microsoft C/C++ compiler Ver.
#include <xmmintrin.h> // SSE
#include <immintrin.h> // AVX, AVX2, FMA
#include <intrin.h>
typedef __m128i vui128;
typedef __m256i vui256;
typedef __m128i vsi128;
typedef __m256i vsi256;

union M128 {
	ui8 d8[32];
	__m128 simd;
	__m128i nums;
};

constexpr M128 zero128 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr M128 max128 = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

union M256 {
	ui8 d8[32];
	__m256 simd;
	__m256i nums;
};

constexpr M256 zero256 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr M256 max256 = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

union M512 {
	ui8 d8[64];
	__m512 simd;
	__m512i nums;
};

constexpr M512 zero512 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr M512 max512 = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

#elif defined (__TURBOC__)
//This is Turbo C/C++ compiler Ver.
#elif defined (__BORLANDC__)
//This is Borland C/C++ compiler Ver.
#elif defined (__WATCOMC__)
//This is Watcom C/C++ compiler Ver.
#elif defined (__IBMCPP__)
//This is IBM Visual Age C++ compiler Ver.
#elif defined (__GNUC__)
//This is GNU C compiler Ver.
#include <x86intrin.h>
typedef unsigned int vui128 __attribute__((vector_size(16)));
typedef unsigned int vui256 __attribute__((vector_size(32)));
typedef int vsi128 __attribute__((vector_size(16)));
typedef int vsi256 __attribute__((vector_size(32)));
#else
//This is knowwn compiler.
#endif

#include "arr_expend.h"

// todo :
/*
0. most mem allocate in fm is sized 4096 (1page)
1. fm_model1 - seperate Data and lifecheck
2. fm_model2 - find more efficient bigger memory allocate algorithm
	- header
3. isHeap -> not require. delete it.
4. any small temp memory - not fm0. just 4096 size mem and fup structure
5. debuging all memory in fmSystem0 any time.
6. no byte8*. use void*
7. use TLS to thread sperate temp memory.
8. there is no class only struct with functions.
9. vecarr to heap page mem fmvecarr..
10. only one function call.
11. delete islocal. -> temp memory replace this.
*/

#define SMALL_PAGE_SIZE 4096

#define Init_VPTR freemem::Init_VPTR_x64
#define ptr_size 8
#define ptr_max 0xFFFFFFFFFFFFFFFF
#define ptr_type uint64_t

#define _GetByte(dat, loc) (dat >> loc) % 2
#define _SetByte(dat, loc, is) dat = freemem::SetByte8(dat, loc, is);
#define vins_New(FM, T, VariablePtr)   \
	((T *)FM._New(sizeof(T)))->Init(); \
	Init_VPTR<T>(VariablePtr);
#define ins_New(FM, T, VariablePtr) ((T *)FM._New(sizeof(T)))->Init();

template <typename T>
void Init_VPTR_x86(void* obj)
{
	T go = T();
	ui32 vp = *(ui32*)&go;
	*((ui32*)obj) = vp;
}

template <typename T>
void Init_VPTR_x64(void* obj)
{
	T go;
	ui64 vp = *(ui64*)&go;
	*((ui64*)obj) = vp;
}

inline void CheckRemainMemorySize()
{
	byte8 end = 10;
	byte8* start = new byte8();
	unsigned int RemainMemSiz = (unsigned int)(start - &end);
	cout << "RemainMemSiz : " << RemainMemSiz << " byte \t(" << (float)RemainMemSiz / 1000.0f << " KB \t(" << (float)RemainMemSiz / 1000000.0f << " MB \t(" << (float)RemainMemSiz / 1000000000.0f << " GB ) ) )" << endl;
	delete start;
}

__forceinline ui32 isP(si32 n) { return !!(0 < n); }
__forceinline ui32 isP0(si32 n) { return !(0 > n); }
__forceinline ui32 isN(si32 n) { return !!(0 > n); }
__forceinline ui32 isN0(si32 n) { return !(0 < n); }

__forceinline ui32 sign_shiftL(ui32 d, si32 n) {
	return (d << n) * isP0(n) + (d >> n) * isN(n);
}
__forceinline ui32 sign_shiftR(ui32 d, si32 n) {
	return (d >> n) * isP0(n) + (d << n) * isN(n);
}

// siz : 4KB
struct SmallPage
{
	byte8 Data[SMALL_PAGE_SIZE] = {};
};

// siz : 2MB
struct Lump2MB {
	byte8 data[2097152];
};

struct ImmortalExpendArr {
	void* Arr; // 불사확장 배열의 첫번째 페이지 주소
	int up; // 현재 크기
	int capacity; // 최대 용량

	template<typename T> _forceinline T& at(ui32 index) {
		return ((T*)Arr)[index];
	}

	template<typename T> _forceinline void push_back(T value);

	template<typename T> _forceinline T& last() {
		return ((T*)Arr)[up - 1];
	}

	template<typename T> void erase(ui32 index) {
		for (int k = index; k < up; ++k)
		{
			((T*)Arr)[k] = ((T*)Arr)[k + 1];
		}
		up -= 1;
	}

	__forceinline ui32 size() {
		return up;
	}
}; // 16byte

//160byte
struct HeapLumps {
	ImmortalExpendArr* Lumps;
	ImmortalExpendArr* expendArrManager;
	ImmortalExpendArr* pageArr;
	ImmortalExpendArr* recycleArr;
	//32byte

	unsigned short pageImortal_up[12] = {};
	void* lastPageImortal[12] = {};
	ui64 extraFlag;
	//128byte

	static constexpr unsigned short pageImortal_capacity[12] = { 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2 };

	//todo
	// 1. recyclePageArr sorting (When?)

	void Init() {
		Lumps = nullptr;

		Lump2MB* newlump = (Lump2MB*)malloc(sizeof(Lump2MB));

		//add Lumps 2nd page data
		ImmortalExpendArr* lumpManagerPtr = (ImmortalExpendArr*)newlump;
		lumpManagerPtr->Arr = (void*)&(newlump->data[SMALL_PAGE_SIZE]);
		lumpManagerPtr->capacity = 512;
		lumpManagerPtr->up = 1;
		Lump2MB** lumparr = (Lump2MB**)lumpManagerPtr->Arr;
		lumparr[0] = newlump;
		Lumps = lumpManagerPtr;

		//add page_manager
		ImmortalExpendArr* pageManagerPtr = (ImmortalExpendArr*)&(newlump->data[16]);
		pageManagerPtr->Arr = (void*)Lumps; // page[n] -> lump[n/512][n%512];
		pageManagerPtr->capacity = 512;
		pageManagerPtr->up = 3;
		pageArr = pageManagerPtr;

		//add expend arr manager 1st page data
		ImmortalExpendArr* arrManagerPtr = (ImmortalExpendArr*)&(newlump->data[32]);
		arrManagerPtr->Arr = (void*)newlump;
		arrManagerPtr->capacity = 256;
		arrManagerPtr->up = 4;
		expendArrManager = arrManagerPtr;

		//add recycle page manager 3rd page data
		ImmortalExpendArr* recyclePageManagerPtr = (ImmortalExpendArr*)&(newlump->data[48]);
		recyclePageManagerPtr->Arr = (void*)&(newlump->data[2 * SMALL_PAGE_SIZE]);
		recyclePageManagerPtr->capacity = 512;
		recyclePageManagerPtr->up = 0;
		recycleArr = recyclePageManagerPtr;
	}

	void* AllocNewLump2MB() {
		Lump2MB* newlump = (Lump2MB*)malloc(sizeof(Lump2MB));
		if (Lumps->up + 1 > Lumps->capacity) {
			//expend lump arr
			ui32 psize = Lumps->capacity << 3;
			ui32 size = psize + SMALL_PAGE_SIZE;
			void* newpage = AllocNewPages(size);
			void* prevpage = Lumps->Arr;
			memcpy_s(newpage, psize, prevpage, psize);
			Lumps->Arr = newpage;
			Lumps->capacity = size >> 3;
			ui32 pcount = psize >> 12;
			for (int i = 0; i < pcount; ++i) {
				RecycleSinglePage(prevpage);
			}
		}
		((Lump2MB**)Lumps->Arr)[Lumps->up] = newlump;
		return newlump;
	}

	ImmortalExpendArr* AllocExpendArr() {
		if (expendArrManager->up + 1 > expendArrManager->capacity) {
			//expending expendArrManager
			ui32 psize = expendArrManager->capacity << 4;
			ui32 size = psize + SMALL_PAGE_SIZE;
			void* newpage = AllocNewPages(size);
			void* prevpage = expendArrManager->Arr;

			ui32 lumpdataOffset = (ui32)((byte8*)Lumps - (byte8*)prevpage);
			ui32 exarrdataOffset = (ui32)((byte8*)expendArrManager - (byte8*)prevpage);
			ui32 recycleOffset = (ui32)((byte8*)recycleArr - (byte8*)prevpage);

			expendArrManager->Arr = newpage;
			expendArrManager->capacity = size >> 4;
			memcpy_s(newpage, psize, prevpage, psize);

			Lumps = (ImmortalExpendArr*)((byte8*)newpage + lumpdataOffset);
			expendArrManager = (ImmortalExpendArr*)((byte8*)newpage + exarrdataOffset);
			recycleArr = (ImmortalExpendArr*)((byte8*)newpage + recycleOffset);

			ui32 pcount = psize >> 12;
			for (int i = 0; i < pcount; ++i) {
				RecycleSinglePage(prevpage);
			}
		}
		ImmortalExpendArr* r = &((ImmortalExpendArr*)expendArrManager->Arr)[expendArrManager->up];
		expendArrManager->up += 1;
		return r;
	}

	void* AllocNewPages(ui32 size) {
		size = size >> 12;
		if (pageArr->up + size > pageArr->capacity) {
			//new lump required
			ui32 prelumpindex = pageArr->up >> 9;
			ui32 prelump_subinedx = pageArr->up & 511;
			ui32 postlumpindex = (pageArr->up + size) >> 9;
			int k = prelump_subinedx;
			for (int i = prelumpindex; i < postlumpindex; ++i) {
				Lump2MB* lump = ((Lump2MB**)Lumps->Arr)[i];
				for (; k < 512; ++k) {
					SmallPage* page = (SmallPage*)&lump->data[SMALL_PAGE_SIZE * k];
					RecycleSinglePage(page);
				}
				k = 0;
			}
			void* newlump = AllocNewLump2MB();
			pageArr->capacity += 512;
			return AllocNewPages(size);
		}
		void* ptr = &((SmallPage*)((Lump2MB**)Lumps->Arr)[(pageArr->up >> 9)]->data)[pageArr->up & 511];
		pageArr->up += size;
		return ptr;
	}

	void* AllocNewSinglePage() {
		if (recycleArr->up > 0) {
			recycleArr->up -= 1;
			void* newpage = ((void**)recycleArr->Arr)[recycleArr->up];
			return newpage;
		}

		if (pageArr->up + 1 > pageArr->capacity) {
			//new lump required
			ui32 prelumpindex = pageArr->up >> 9;
			ui32 prelump_subinedx = pageArr->up & 511;
			ui32 postlumpindex = (pageArr->up + 1) >> 9;
			int k = prelump_subinedx;
			for (int i = prelumpindex; i < postlumpindex; ++i) {
				Lump2MB* lump = ((Lump2MB**)Lumps->Arr)[i];
				for (; k < 512; ++k) {
					SmallPage* page = (SmallPage*)&lump->data[SMALL_PAGE_SIZE * k];
					RecycleSinglePage(page);
				}
				k = 0;
			}
			void* newlump = AllocNewLump2MB();
			pageArr->capacity += 512;
			return AllocNewSinglePage();
		}
		void* ptr = &((SmallPage*)((Lump2MB**)Lumps->Arr)[(pageArr->up >> 9)]->data)[pageArr->up & 511];
		pageArr->up += 1;
		return ptr;
	}

	void RecycleSinglePage(void* page) {
		if (recycleArr->up >= recycleArr->capacity) {
			// expend recycle arr
			ui32 psize = recycleArr->capacity << 3;
			ui32 size = psize + SMALL_PAGE_SIZE;
			void* newpage = AllocNewPages(size);
			void* prevpage = recycleArr->Arr;
			memcpy_s(newpage, psize, prevpage, psize);
			recycleArr->Arr = newpage;
			recycleArr->capacity = size >> 3;
			ui32 pcount = psize >> 12;
			for (int i = 0; i < pcount; ++i) {
				RecycleSinglePage(prevpage);
			}
		}
		((void**)recycleArr->Arr)[recycleArr->up] = page;
		recycleArr->up += 1;
	}

	void RecyclePages(void* page, ui32 size) {
		size = size >> 12;
		if (recycleArr->up + size >= recycleArr->capacity) {
			// expend recycle arr
			ui32 psize = recycleArr->capacity << 3;
			ui32 size = psize + SMALL_PAGE_SIZE;
			void* newpage = AllocNewPages(size);
			void* prevpage = recycleArr->Arr;
			memcpy_s(newpage, psize, prevpage, psize);
			recycleArr->Arr = newpage;
			recycleArr->capacity = size >> 3;
			ui32 pcount = psize >> 12;
			for (int i = 0; i < pcount; ++i) {
				RecycleSinglePage(prevpage);
			}
		}
		for (int i = 0; i < size; ++i) {
			RecycleSinglePage(&((SmallPage*)page)[i]);
		}
	}

	void* Allocate_ImortalMemory(unsigned int size)
	{
		if (size == 0) return nullptr;
		unsigned int index = log2(size - 1) + 1;

		if (pageImortal_up[index] == pageImortal_capacity[index] || pageImortal_up[index] == 0)
		{
			lastPageImortal[index] = (void*)AllocNewSinglePage();
			pageImortal_up[index] = 0;
		}

		unsigned int up = pageImortal_up[index];
		pageImortal_up[index] += 1;
		ui32 x = (up << index);
		byte8* p = ((byte8**)lastPageImortal)[index];
		return (p + x);
	}

	void HeapDump() {
		ofstream out;
		out.open("HeapDump.txt");
		out << "Lump Up : " << Lumps->up << endl;
		for (int i = 0; i < Lumps->up; ++i) {
			out << "Lump[" << i << "] : " << (int*)(((Lump2MB**)Lumps->Arr)[0]) << "\t PageUp : ";
			if (pageArr->up > 512 * (i + 1)) {
				out << "512 / 512" << endl;
			}
			else {
				out << (pageArr->up & 511) << "/ 512" << endl;
			}
		}

		out << endl;

		for (ui32 i = 0; i < pageArr->up; ++i) {
			SmallPage* page = &((SmallPage*)((Lump2MB**)Lumps->Arr)[(i >> 9)]->data)[i & 511];

			if (Lumps->Arr == page) {
				int loopN = Lumps->capacity >> 9;
				for (int k = 0; k < loopN; ++k) {
					out << "[page " << i << "] : " << "Lump" << endl;
					++i;
				}
				--i;
				continue;
			}
			else if (expendArrManager->Arr == page) {
				int loopN = expendArrManager->capacity >> 8;
				for (int k = 0; k < loopN; ++k) {
					out << "[page " << i << "] : " << "ExpendArrManage" << endl;
					++i;
				}
				--i;
				continue;
			}
			else if (recycleArr->Arr == page) {
				int loopN = recycleArr->capacity >> 9;
				for (int k = 0; k < loopN; ++k) {
					out << "[page " << i << "] : " << "Recycle" << endl;
					++i;
				}
				--i;
				continue;
			}
			else {
				for (int k = 0; k < expendArrManager->up; ++k) {
					ImmortalExpendArr* expendArr = &expendArrManager->at<ImmortalExpendArr>(k);
					if (expendArr->Arr == page) {
						int loopN = expendArr->capacity >> 8;
						for (int u = 0; u < loopN; ++u) {
							out << "[page " << i << "] : " << "ExpendArr[" << k << "]" << endl;
							++i;
						}
						--i;

						goto HEAPLUMP_HEAPDUMP_CONTINUE;
					}
				}

				for (int k = 0; k < 12; ++k) {
					if (page == lastPageImortal[k]) {
						out << "[page " << i << "] : " << "last ImmortalPage " << (1 << k) << " byte inteval" << endl;
						goto HEAPLUMP_HEAPDUMP_CONTINUE;
					}
				}

				out << "[page " << i << "] : " << "invisible page" << endl;

			HEAPLUMP_HEAPDUMP_CONTINUE:
				continue;
			}
		}

		out.close();
	}
};

HeapLumps fmhl;

template<typename T> void ImmortalExpendArr::push_back(T value) {
	//fmhl.HeapDump();

	if (up + 1 > capacity) {
		// newalloc
		ui32 cpa = capacity * sizeof(T);
		ui32 psiz = cpa / SMALL_PAGE_SIZE;
		ui32 siz = psiz + SMALL_PAGE_SIZE;
		void* newpages = fmhl.AllocNewPages(siz);
		memcpy_s(newpages, psiz, Arr, psiz);
		fmhl.RecyclePages(Arr, cpa);
		Arr = newpages;
		capacity += SMALL_PAGE_SIZE / sizeof(T);
	}
	((T*)Arr)[up] = value;
	up += 1;
}

// siz : 16 byte
struct PageMeta
{
	void* PageData;
	unsigned int Fup;
	unsigned int extra_Flags; // 32 flags

	void Allocate()
	{
		PageData = fmhl.AllocNewSinglePage();
		Fup = 0;
		extra_Flags = 0;
	}

	void* _New(unsigned int size)
	{
		void* ptr = (byte8*)PageData + Fup;
		Fup += size;
		if (Fup >= SMALL_PAGE_SIZE)
		{
			return nullptr;
		}
		return ptr;
	}

	void ClearAll()
	{
		Fup = 0;
	}
};
// use this in smalltempmem and fm0

struct large_alloc
{
	void* ptr;
	//ui32 size;
};

// siz : 16 byte
struct FmTempLayer {
	ImmortalExpendArr* large;
	ImmortalExpendArr* tempFM;

	void Init() {
		large = fmhl.AllocExpendArr();
		large->capacity = 512;
		large->up = 0;
		for (int i = 0; i < 512; ++i) {
			large->at<void*>(i) = nullptr;
		}
		tempFM = fmhl.AllocExpendArr();
		tempFM->capacity = 512;
		tempFM->up = 0;
		for (int i = 0; i < 512; ++i) {
			tempFM->at<void*>(i) = nullptr;
		}
	}

	void Release() {
		for (int i = 0; i < tempFM->up; ++i) {
			tempFM->at<PageMeta*>(i)->ClearAll();
		}

		for (int i = 0; i < large->up; ++i) {
			free(large->at<large_alloc*>(i)->ptr);
		}

		// HeapLump안에 있어서 헤제 x
	}

	void ClearAll() {
		for (int i = 0; i < tempFM->up; ++i) {
			tempFM->at<PageMeta*>(i)->Fup = 0;
		}

		for (int i = 0; i < large->up; ++i) {
			free(large->at<large_alloc*>(i)->ptr);
		}

		tempFM->up = 0;
		large->up = 0;
	}

	void* _New(unsigned int size) {
		if (size <= SMALL_PAGE_SIZE)
		{
			unsigned int tsize = tempFM->up;
			for (int i = 0; i < tsize; ++i)
			{
				//watch("i", i);
				int remain = SMALL_PAGE_SIZE - tempFM->at<PageMeta*>(i)->Fup;
				if (remain >= size)
				{
					return tempFM->at<PageMeta*>(i)->_New(size);
				}
			}

			PageMeta* pm = (PageMeta*)fmhl.Allocate_ImortalMemory(sizeof(PageMeta));
			pm->Allocate();
			tempFM->push_back<PageMeta*>(pm);
			return tempFM->last<PageMeta*>()->_New(size);
		}
		else
		{
			large_alloc* la = (large_alloc*)fmhl.Allocate_ImortalMemory(8);
			la->ptr = malloc(size);
			//la.size = size;
			large->push_back<large_alloc*>(la);
			return reinterpret_cast<void*>(la->ptr);
		}
	}
};

thread_local unsigned int threadID;

//siz : 512byte
struct FmTempStack
{
public:
	ui32 up;
	ui32 capacity;
	ui32 extra[2];
	FmTempLayer data512byte[31];
	//vecarr < vecarr < large_alloc > *>large;
	//vecarr < vecarr < FM_Model0 * >*>tempFM;

	void init()
	{
		up = 0;
		capacity = 31;
		for (int i = 0; i < 31; ++i) {
			data512byte[i].tempFM = nullptr;
			data512byte[i].large = nullptr;
		}
		//watch("tempFM init", 0);
	}

	void release()
	{
		for (int i = 0; i < 31; ++i) {
			data512byte[i].Release();
		}
	}

	void* _New(unsigned int size, int fmlayer = -1)
	{
		int sel_layer = fmlayer;
		if (sel_layer < 0) sel_layer = up - 1;
		return data512byte[sel_layer]._New(size);
	}

	void PushLayer()
	{
		if (data512byte[up].tempFM == nullptr) {
			data512byte[up].Init();
		}
		up += 1;
	}

	void PopLayer()
	{
		data512byte[up - 1].ClearAll();
		up -= 1;
	}
};

// The storage method is classified by the size of the data.
// 
//siz : 8 + 8 + 4 + 4 + 4 + 4 = 32byte
struct FmFlagPage {
	SmallPage* page;
	void* flagData;
	void* LCENData; // life check flag End 0 flag string Num(len) 2byte->4bit 4byte->1byte
	//ex> 0b0111010011000011010000 => 4

	static constexpr unsigned int DataCapacity = 4096;
	unsigned int extradata = 0;
	unsigned int Fup = 0;

	__forceinline ui32 GetBytePerLifeFlag_pow2() {
		return (extradata & 0x000000FF);
	}

	__forceinline ui32 GetBytePerLifeFlag() {
		return 1 << (extradata & 0x000000FF);
	}

	__forceinline ui32 GetFitSiz(ui32 size) {
		const int bplf = GetBytePerLifeFlag_pow2();
		ui32 lm = 1 << (bplf);
		ui32 p = (size & (lm - 1));
		return size - p + !!p * lm;
	}

	__forceinline void SetlcenData(unsigned int sindex, byte8 data) {
		if (LCENData != nullptr) {
			byte8* lcenarr = (byte8*)LCENData;
			byte8 c = sindex & 1;
			lcenarr[sindex >> 1] &= 0x0F << (c << 2);
			lcenarr[sindex >> 1] |= data << 4 * c;
		}
	}

	__forceinline bool GetFlag(unsigned int dataindex) {
		const int bplf = GetBytePerLifeFlag_pow2();
		ui32 temp = dataindex >> bplf;
		ui32 byteindex = temp >> 3;
		ui32 bitindex = temp & 7;
		byte8 flagMask = (byte8)(1 << bitindex);
		byte8 chbyte = ((byte8*)flagData)[byteindex];
		return chbyte & flagMask;
	}

	void Init(unsigned int _BytePerLifeFlag)
	{
		//save bit per life flag
		extradata &= 0xFFFFFF00;
		extradata |= (ui32)(log2(_BytePerLifeFlag));

		page = (SmallPage*)fmhl.AllocNewSinglePage();
		ui32 flagmem_cap = DataCapacity / GetBytePerLifeFlag();
		flagmem_cap = flagmem_cap >> 3;
		if (flagmem_cap == 0) {
			flagmem_cap = 1;
		}
		const int id = GetID();
		flagData = (void*)fmhl.Allocate_ImortalMemory(flagmem_cap);
		if (flagData != nullptr) {
			byte8* fdarr = (byte8*)flagData;
			for (int i = 0; i < flagmem_cap; ++i) {
				fdarr[i] = 0;
			}
		}
		
		LCENData = (void*)fmhl.Allocate_ImortalMemory(flagmem_cap >> 2);
		if (LCENData != nullptr) {
			byte8* lcenarr = (byte8*)LCENData;
			for (int i = 0; i < flagmem_cap >> 2; ++i) {
				lcenarr[i] = 15; //0 mean no rest space, 15 : 15 rest space or 16 rest space
			}
		}
		Fup = 0;
	}

	ui32 GetID() {
		ui32 A = 0;
		ui32 B = 0;
		for (int i = 0; i < fmhl.Lumps->size(); ++i) {
			byte8* ptr = fmhl.Lumps->at<byte8*>(i);
			si32 n = page->Data - ptr;
			n = n >> 12;
			if (0 <= n && n < 512) {
				A = i;
				B = n;
				break;
			}
		}

		ui32 ID = (A << 9) + B;
		return ID;
	}

	void SetFlag1(unsigned int DataStart, unsigned int size) {
		//update flag
		const ui32 id = GetID();
		const int bplf = GetBytePerLifeFlag_pow2();
		ui32 temp = DataStart >> bplf;
		ui32 temp2 = (DataStart + size-1) >> bplf;
		ui32 startflagindex = temp >> 3;
		ui32 startflag_bitindex = temp & 7;
		ui32 endflagindex = temp2 >> 3;
		ui32 endflag_bitindex = temp2 & 7;

		ui32 deltaflagindex = endflagindex - startflagindex;
		byte8* fdata = ((byte8*)flagData);
		byte8 startFlagMask = -(byte8)(1 << startflag_bitindex);
		byte8 endFlagMask = (byte8)(2 << endflag_bitindex) - 1;
		ui16* u16_fdata = (ui16*)fdata;
		ui32 startLCENindex = startflagindex >> 2;
		ui32 endLCENindex = endflagindex >> 2;
		switch (deltaflagindex) {
		case 0:
			fdata[startflagindex] |= (startFlagMask & endFlagMask);
			SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
			return;
		case 1:
			fdata[startflagindex] |= startFlagMask;
			fdata[endflagindex] |= endFlagMask;
			SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
			SetlcenData(endLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
			return;
		}

		fdata[startflagindex] |= startFlagMask;
		fdata[endflagindex] |= endFlagMask;

		/*ui32 simdState = _tzcnt_u32(deltaflagindex-1);
		switch (simdState) {
		case 0:
			for (int i = startflagindex + 1; i < endflagindex; ++i) {
				fdata[i] = 255;
			}
			break;
		case 1:
			for (int i = startflagindex + 1; i < endflagindex; i += 2) {
				*((ui16*)&fdata[i]) = 65535;
			}
			break;
		case 2:
			for (int i = startflagindex + 1; i < endflagindex; i += 4) {
				*((ui32*)&fdata[i]) = 4294967295;
			}
			break;
		case 3:
			for (int i = startflagindex + 1; i < endflagindex; i += 8) {
				*((ui64*)&fdata[i]) = ~0;
			}
			break;
		case 4:
			for (int i = startflagindex + 1; i < endflagindex; i += 16) {
				*((__m128*) & fdata[i]) = max128.simd;
			}
			break;
		case 5:
			for (int i = startflagindex + 1; i < endflagindex; i += 32) {
				*((__m256*) & fdata[i]) = max256.simd;
			}
			break;
		case 6:
			for (int i = startflagindex + 1; i < endflagindex; i += 64) {
				*((__m512*)&fdata[i]) = max512.simd;
			}
			break;
		}*/
		ui32 deltaF = deltaflagindex - 1;
		ui32 biggest1_Loc = 0;
		ui32 i = startflagindex + 1;
	FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT:
		biggest1_Loc = 32 - __lzcnt(deltaF);
		switch (biggest1_Loc) {
		case 1:
			fdata[i] = 255;
			deltaF -= 1;
			i += 1;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 2:
			*((ui16*)&fdata[i]) = 65535;
			deltaF -= 2;
			i += 2;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 3:
			*((ui32*)&fdata[i]) = 4294967295;
			deltaF -= 4;
			i += 4;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 4:
			*((ui64*)&fdata[i]) = ~0;
			deltaF -= 8;
			i += 8;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 5:
			*((__m128*) & fdata[i]) = max128.simd;
			deltaF -= 16;
			i += 16;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 6:
			*((__m256*) & fdata[i]) = max256.simd;
			deltaF -= 32;
			i += 32;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 7:
			*((__m512*)&fdata[i]) = max512.simd;
			deltaF -= 64;
			i += 64;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			for (int k = 0; k < deltaF; k += 64) {
				*((__m512*)&fdata[i]) = max512.simd;
				i += 64;
			}
			deltaF = deltaF % 64;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		}

		//LCEN Data Update
		SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
		for (int i = startLCENindex + 1; i < endLCENindex; ++i) {
			SetlcenData(i, _tzcnt_u16(u16_fdata[startLCENindex]));
		}
		SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
	}

	void SetFlag0(unsigned int DataStart, unsigned int size) {
		//update flag
		const int bplf = GetBytePerLifeFlag_pow2();
		ui32 temp = (DataStart >> bplf);
		ui32 temp2 = ((DataStart + size - 1) >> bplf);
		ui32 startflagindex = temp >> 3;
		ui32 startflag_bitindex = temp & 7; // 왜 15임?
		ui32 endflagindex = temp2 >> 3;
		ui32 endflag_bitindex = temp2 & 7;

		ui32 deltaflagindex = endflagindex - startflagindex;
		byte8* fdata = ((byte8*)flagData);
		byte8 startFlagMask = ~(-(byte8)(1 << startflag_bitindex));
		byte8 endFlagMask = ~((byte8)((2 << endflag_bitindex) - 1));
		ui16* u16_fdata = (ui16*)fdata;
		ui32 startLCENindex = startflagindex >> 2;
		ui32 endLCENindex = endflagindex >> 2;

		switch (deltaflagindex) {
		case 0:
			fdata[startflagindex] &= (startFlagMask | endFlagMask);
			SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
			return;
		case 1:
			fdata[startflagindex] &= startFlagMask;
			fdata[endflagindex] &= endFlagMask;
			SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
			SetlcenData(endLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
			return;
		}

		fdata[startflagindex] &= startFlagMask;
		fdata[endflagindex] &= endFlagMask;

		/*
		* ui32 simdState = _tzcnt_u32(deltaflagindex);
		switch (simdState) {
		case 0:
			for (int i = startflagindex + 1; i < endflagindex; ++i) {
				fdata[i] = 0;
			}
			break;
		case 1:
			for (int i = startflagindex + 1; i < endflagindex; i += 2) {
				*((ui16*)&fdata[i]) = 0;
			}
			break;
		case 2:
			for (int i = startflagindex + 1; i < endflagindex; i += 4) {
				*((ui32*)&fdata[i]) = 0;
			}
			break;
		case 3:
			for (int i = startflagindex + 1; i < endflagindex; i += 8) {
				*((ui64*)&fdata[i]) = 0;
			}
			break;
		case 4:
			for (int i = startflagindex + 1; i < endflagindex; i += 16) {
				*((__m128*) & fdata[i]) = zero128.simd;
			}
			break;
		case 5:
			for (int i = startflagindex + 1; i < endflagindex; i += 32) {
				*((__m256*) & fdata[i]) = zero256.simd;
			}
			break;
		case 6:
			for (int i = startflagindex + 1; i < endflagindex; i += 64) {
				*((__m512*)&fdata[i]) = zero512.simd;
			}
			break;
		}
		*/
		ui32 deltaF = deltaflagindex - 1;
		ui32 biggest1_Loc = 0;
		ui32 i = startflagindex + 1;
	FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT:
		biggest1_Loc = 32 - __lzcnt(deltaF);
		switch (biggest1_Loc) {
		case 1:
			fdata[i] = 0;
			deltaF -= 1;
			i += 1;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 2:
			*((ui16*)&fdata[i]) = 0;
			deltaF -= 2;
			i += 2;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 3:
			*((ui32*)&fdata[i]) = 0;
			deltaF -= 4;
			i += 4;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 4:
			*((ui64*)&fdata[i]) = 0;
			deltaF -= 8;
			i += 8;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 5:
			*((__m128*) & fdata[i]) = zero128.simd;
			deltaF -= 16;
			i += 16;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 6:
			*((__m256*) & fdata[i]) = zero256.simd;
			deltaF -= 32;
			i += 32;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 7:
			*((__m512*)&fdata[i]) = zero512.simd;
			deltaF -= 64;
			i += 64;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			for (int k = 0; k < deltaF; k += 64) {
				*((__m512*)&fdata[i]) = zero512.simd;
				i += 64;
			}
			deltaF = deltaF % 64;
			goto FMFLAGPAGE_SETFLAG0_SIMDFILL_LZCNT;
		}

		//LCEN Data Update
		SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
		for (int i = startLCENindex + 1; i < endLCENindex; ++i) {
			SetlcenData(i, _tzcnt_u16(u16_fdata[startLCENindex]));
		}
		SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
	}

	void* _saveNew(int size)
	{
		void* ptr = (byte8*)page + Fup;
		ui32 fitsiz = GetFitSiz(size);
		unsigned int preFup = Fup;
		Fup += fitsiz;
		if (Fup > SMALL_PAGE_SIZE)
		{
			Fup -= fitsiz;

			//Save Mode1 ()
			if (fitsiz < (16 << GetBytePerLifeFlag_pow2()))
			{
				byte8* lcenarr = (byte8*)LCENData;
				ui32 sd8 = fitsiz >> 3;
				ui32 flagmem_cap = DataCapacity / GetBytePerLifeFlag();
				ui32 index = 0;
				ui32 rest = 0;
				for (int i = 0; i < flagmem_cap >> 2; ++i) {
					rest = lcenarr[i] & 0x0F;
					if (rest >= sd8) {
						index = i << 1;
						index = GetBytePerLifeFlag() * (index << 1) - rest;
						goto FMFLAGPAGE_SAVENEW_SAVEMODE1_ALLOC_SUCESS;
					}
					rest = (lcenarr[i] & 0xF0) >> 4;
					if (rest >= sd8) {
						index = i << 1 + 1;
						index = GetBytePerLifeFlag() * (index << 1) - rest;
						goto FMFLAGPAGE_SAVENEW_SAVEMODE1_ALLOC_SUCESS;
					}
				}

				//SaveMode2
				return nullptr;

			FMFLAGPAGE_SAVENEW_SAVEMODE1_ALLOC_SUCESS:
				byte8* ptr = (byte8*)page + rest;
				ui8* tipptr = (ui8*)ptr;

				if (fitsiz >= 4) {
				FMFLAGPAGE_SAVENEW_SAVEMODE1_ALLOC_SUCESS_FILLHOLE:
					tipptr += *(ui16*)tipptr;
					si64 delta = (si64)tipptr - (si64)ptr;
					if (delta > 0) {
						switch (fitsiz) {
						case 1:
						case 2:
						case 3:
							for (int i = 0; i < fitsiz; ++i) {
								((byte8*)ptr)[i] = 255;
							}
							goto FMFLAGPAGE_SAVENEW_SETFLAG;
						}

						*(ui16*)ptr = (ui16)delta;
						*(ui16*)((byte8*)ptr + delta - 1) = (ui16)delta;
					}
					else {
						goto FMFLAGPAGE_SAVENEW_SAVEMODE1_ALLOC_SUCESS_FILLHOLE;
					}
				}

			FMFLAGPAGE_SAVENEW_SETFLAG:
				SetFlag1(index, fitsiz);
				return ptr;
			}
			else {
				//Too high size.. (not fit in LC)
				return nullptr;
			}
		}
		//Fast Mode
		//change flag
		SetFlag1(preFup, fitsiz);
		return ptr;
	}

	void* _fastNew(int size)
	{
		const ui32 id = GetID();
		void* ptr = (byte8*)page + Fup;
		ui32 fitsiz = GetFitSiz(size);
		ui32 postFup = Fup + fitsiz;
		if (postFup > SMALL_PAGE_SIZE)
		{
			return nullptr;
		}
		//Fast Mode
		//change flag
		

		SetFlag1(Fup, fitsiz);
		
		/*if (id == 22) {
			cout << "New Start" << endl;
			dbg_lifecheck();
		}*/

		Fup = postFup;
		return ptr;
	}

	bool _Delete(void* ptr, ui32 size) {
		const ui32 id = GetID();
		ui32 fitsiz = GetFitSiz(size);
		si32 offset_s = (byte8*)ptr - (byte8*)page;
		si32 offset_e = offset_s + fitsiz - 1;
		if (offset_e >= Fup || offset_s < 0) return false;
		int index = (byte8*)ptr - (byte8*)page;
		
		SetFlag0(index, fitsiz);

		/*if (id == 22) {
			cout << "Delete Start" << endl;
			dbg_lifecheck();
		}*/
		if (index + fitsiz >= Fup) {
			Fup = index;
			if (index - 1 < 0) return true;
			si32 pivot = index - 1;
		
			if ((pivot >= 0) && (GetFlag(pivot) == false && page->Data[pivot] == 255)) {
				Fup -= 1;
				pivot -= 1;
			FLAGPAGE_DELETE_FUP_UPDATE_1b:
				if ((pivot >= 0) && (GetFlag(pivot) == false && page->Data[pivot] == 255) ) {
					Fup -= 1;
					pivot -= 1;
					goto FLAGPAGE_DELETE_FUP_UPDATE_1b;
				}
				else {
					return true;
				}
			}

			pivot -= 1;
		FLAGPAGE_DELETE_FUP_UPDATE:
			ui16 delta = *(ui16*)&page->Data[pivot];

			//check index byte is allocated
			bool allocated = GetFlag(pivot);

			if (allocated == false) {
				ui16 delta = *(ui16*)&page->Data[pivot];
				
				if (delta == 0) {
					//error! hole meta data is not exist.
					for (int i = 0; i < SMALL_PAGE_SIZE; i += 16) {
						for (int k = 0; k < 16; k+=2) {
							if (GetFlag(i + k) == true) {
								cout << "X, ";
								continue;
							}
							else if (i + k == pivot) {
								cout << "[" << *(short*)&page->Data[i + k] << "]";
								continue;
							}
							cout << *(short*)&page->Data[i + k] << ", ";
						}
						cout << endl;
					}
					_CrtDbgBreak();
					return false;
				}
				Fup -= delta;
				pivot = Fup - 2;
				if (pivot >= 0) {
					goto FLAGPAGE_DELETE_FUP_UPDATE;
				}
				else Fup = 0;
			}
		}
		else {
			if (fitsiz <= 3) {
				for (int i = 0; i < fitsiz; ++i) {
					((byte8*)ptr)[i] = 255;
				}
				return true;
			}
			else {
				*(ui16*)ptr = (ui16)fitsiz;
				*(ui16*)((byte8*)ptr + fitsiz - 2) = (ui16)fitsiz;
				return true;
			}
		}
	}

	bool isAllocated(void* ptr, ui32 size) {
		si32 offset = (byte8*)ptr - (byte8*)page + size;
		if (offset >= Fup || offset < 0) return false;

		si32 DataStart = (byte8*)ptr - (byte8*)page;
		const int bplf = GetBytePerLifeFlag_pow2();
		ui32 temp = DataStart >> bplf;
		ui32 temp2 = (DataStart + size-1) >> bplf;
		ui32 startflagindex = temp >> 3;
		ui32 startflag_bitindex = temp & 7;
		ui32 endflagindex = temp2 >> 3;
		ui32 endflag_bitindex = temp2 & 7;

		ui32 deltaflagindex = endflagindex - startflagindex;
		byte8* fdata = ((byte8*)flagData);
		byte8 startFlagMask = -(byte8)(1 << startflag_bitindex);
		byte8 endFlagMask = (byte8)(2 << endflag_bitindex) - 1;

		switch (deltaflagindex) {
		case 0:
		{
			byte8 mask = (startFlagMask & endFlagMask);
			if (_pdep_u32(fdata[startflagindex], mask) == mask) {
				return true;
			}
			return false;
		}
		case 1:
			if (_pdep_u32(fdata[startflagindex], startFlagMask) == startFlagMask && _pdep_u32(fdata[startflagindex], endFlagMask) == endFlagMask) {
				return true;
			}
			return false;
		}

		if (_pdep_u32(fdata[startflagindex], startFlagMask) == startFlagMask && _pdep_u32(fdata[startflagindex], endFlagMask) == endFlagMask) {
			ui32 simdState = _tzcnt_u32(deltaflagindex-1);
			switch (simdState) {
			case 0:
				for (int i = startflagindex + 1; i < endflagindex; ++i) {
					if (fdata[i] != 255) return false;
				}
				break;
			case 1:
				for (int i = startflagindex + 1; i < endflagindex; i += 2) {
					if (*((ui16*)&fdata[i]) != 65535) return false;
				}
				break;
			case 2:
				for (int i = startflagindex + 1; i < endflagindex; i += 4) {
					if (*((ui32*)&fdata[i]) != 4294967295) return false;
				}
				break;
			case 3:
				for (int i = startflagindex + 1; i < endflagindex; i += 8) {
					if (*((ui64*)&fdata[i]) != ~0) return false;
				}
				break;
			case 4:
				for (int i = startflagindex + 1; i < endflagindex; i += 16) {
					M128 v;
					v.simd = _mm_cmpneq_ps(*((__m128*) & fdata[i]), max128.simd);
					if (_mm_movemask_epi8(v.nums) != 0) return false;
				}
				break;
			case 5:
				for (int i = startflagindex + 1; i < endflagindex; i += 32) {
					M256 v1, v2;
					v1.simd = *((__m256*) & fdata[i]);
					if (_mm256_cmpneq_epu8_mask(v1.nums, max256.nums) != 0) return false;
				}
				break;
			case 6:
				for (int i = startflagindex + 1; i < endflagindex; i += 64) {
					M512 v1, v2;
					v1.simd = *((__m512*)&fdata[i]);
					if (_mm512_cmpneq_epi8_mask(v1.nums, max512.nums) != 0) return false;
				}
				break;
			}

			return true;
		}

		return false;
	}

	void dbg_lifecheck()
	{
		int count = 0;
		cout << "FlagPage_" << GetID() << endl;
		const int BytePerLifeFlag = GetBytePerLifeFlag();
		for (int i = 0; i < SMALL_PAGE_SIZE; i += BytePerLifeFlag)
		{
			if (count % 32 == 0) {
				cout << (int*)&((byte8*)page)[i] << " : \t";
			}
			switch (GetFlag(i))
			{
			case true:
				cout << '1';
				break;
			case false:
				cout << '_';
				break;
			}

			if (count % 32 == 31) {
				cout << endl;
			}

			++count;
		}
		cout << endl;
	}
};

//16
vecarr<vecarr<vecarr<uint64_t>>> checkarr;
struct FmFlagLayer {
	ImmortalExpendArr* flagPageArr;
	ui32 bytePerFlag = 1;
	ui32 extraFlag = 0;

	void Init(ui32 bpf) {
		bytePerFlag = bpf;
		flagPageArr = fmhl.AllocExpendArr();
		flagPageArr->Arr = fmhl.AllocNewSinglePage();
		flagPageArr->capacity = 512;
		flagPageArr->up = 0;
		for (int i = 0; i < 512; ++i) {
			((FmFlagPage**)flagPageArr->Arr)[i] = nullptr;
		}
	}

	void* _fastNew(ui32 size) {
		for (int i = 0; i < flagPageArr->up; ++i) {
			void* rptr = ((FmFlagPage**)flagPageArr->Arr)[i]->_fastNew(size);
			if (rptr != nullptr) {
#ifdef FM_NONRELEASE_HEAPCHECK
				vecarr<uint64_t>* chvec = &checkarr.at(log2(bytePerFlag)).at(fm1->at(i)->id);
				for (int k = 0; k < chvec->size(); ++k) {
					uint64_t rated = reinterpret_cast<uint64_t>(rptr) - reinterpret_cast<uint64_t>(fm1->at(i)->DataPtr);
					if (rated == chvec->at(k)) {
						//cout << "break;" << endl;
						_CrtDbgBreak();
					}
					break;
				}
#endif
				return rptr;
			}
		}

		// require new flagpage
		FmFlagPage* flagpage = (FmFlagPage*)fmhl.Allocate_ImortalMemory(sizeof(FmFlagPage));
		flagpage->Init(bytePerFlag);
		flagPageArr->push_back<FmFlagPage*>(flagpage);
		//flagpage->dbg_lifecheck();
		void* ptr = flagpage->_fastNew(size);
#ifdef FM_NONRELEASE_HEAPCHECK
		vecarr<uint64_t>* chvec = &checkarr.at(log2(bytePerFlag)).at(flagpage->GetID());
		for (int k = 0; k < chvec->size(); ++k) {
			uint64_t rated = reinterpret_cast<uint64_t>(ptr) - reinterpret_cast<uint64_t>(flagpage->page);
			if (rated == chvec->at(k)) {
				//cout << "break;" << endl;
				_CrtDbgBreak();
			}
			break;
		}
#endif
		return ptr;
	}

	void* _saveNew(ui32 size) {
		for (int i = 0; i < flagPageArr->up; ++i) {
			void* rptr = ((FmFlagPage**)flagPageArr->Arr)[i]->_saveNew(size);
			if (rptr != nullptr) return rptr;
		}

		// require new flagpage
		FmFlagPage* flagpage = (FmFlagPage*)fmhl.Allocate_ImortalMemory(sizeof(FmFlagPage));
		flagpage->Init(bytePerFlag);
		flagPageArr->push_back<FmFlagPage*>(flagpage);
		return flagpage->_saveNew(size);
	}

	bool Delete(void* ptr, ui32 size) {
		static int dbgc = 0;
		for (int i = 0; i < flagPageArr->up; ++i) {
			FmFlagPage* rptr = ((FmFlagPage**)flagPageArr->Arr)[i];
			if (i == 0) dbgc += 1;
			if ((byte8*)ptr - (byte8*)rptr->page < SMALL_PAGE_SIZE) {
#ifdef FM_NONRELEASE_HEAPCHECK
				vecarr<uint64_t>* chvec = &checkarr.at(log2(rptr->GetBitPerLifeFlag())).at(rptr->GetID());
				for (int k = 0; k < chvec->size(); ++k) {
					uint64_t rated = reinterpret_cast<uint64_t>(ptr) - reinterpret_cast<uint64_t>(rptr->page);
					if (rated == chvec->at(k)) {
						_CrtDbgBreak();
					}
					break;
				}
#endif
				return rptr->_Delete(ptr, size);
			}
		}
		return false;
	}

	bool isAllocated(void* ptr, ui32 size) {
		for (int i = 0; i < flagPageArr->up; ++i) {
			byte8* rptr = (byte8*)((FmFlagPage**)flagPageArr->Arr)[i]->page;
			if (rptr < ptr && ptr < rptr + SMALL_PAGE_SIZE) {
				return ((FmFlagPage*)rptr)->isAllocated(ptr, size);
			}
		}
		return false;
	}
};

//16byte
struct FmPageAlloc {
	void* page;
	ui32 size;
	ui32 capacity;
};

struct FmLargeAlloc {
	void* ptr;
};

template <typename IndexType, typename ValueType> struct range
{
	IndexType end;
	ValueType value;
};

int getcost(int n, int size)
{
	ui32 k = !!(n % size);
	ui32 dd = (n / size) + k;
	ui32 r = (8 * size) * dd + dd;
	return r;
}

int minarr(int siz, int* arr, int* indexout)
{
	int min = arr[0];
	for (int i = 0; i < siz; ++i)
	{
		if (min > arr[i])
		{
			min = arr[i];
			*indexout = i;
		}
	}
	return min;
}

template <typename IndexType, typename ValueType>
struct RangeArr_node {
	unsigned short divn;
	IndexType maxI;
	IndexType cmax;
	void* child;

	void print_state(ofstream& ofs, int stack = 0) {
		switch (divn) {
		case 1:
			ofs << "divn : " << divn << " | maxI : " << maxI << " | cmax : " << cmax << " | Value : " << *((ValueType*)child) << endl;
			return;
		case 2:
			ofs << "divn : " << divn << " | maxI : " << maxI << " | cmax : " << cmax << " | Value[0] : " << ((ValueType*)child)[0] << " | Value[1] : " << ((ValueType*)child)[1] << endl;
			return;
		}
		ofs << "divn : " << divn << " | maxI : " << maxI << " | cmax : " << cmax << endl;
		for (int i = 0; i < divn; ++i) {
			RangeArr_node ran = ((RangeArr_node*)child)[i];
			for (int i = 0; i < stack + 1; ++i) {
				ofs << "\t";
			}
			ofs << "\tchild[" << i << "] : ";
			ran.print_state(ofs, stack + 1);
		}
	}

	// -1 : no replace
	// 0~1 : replace with that index
	int clean(vecarr<RangeArr_node<IndexType, ValueType>*>& rangeBuff) {
		switch (divn) {
		case 1:
		case 2:
		{
			for (int i = 0; i < rangeBuff.size(); ++i) {
				RangeArr_node<IndexType, ValueType>* ri = rangeBuff.at(i);
				if (ri->divn == divn) {
					bool b = true;
					for (int k = 0; k < divn; ++k) {
						b = b && ((ValueType*)ri->child)[k] == ((ValueType*)child)[k];
					}
					b = b && ri->cmax == cmax;
					b = b && ri->maxI == maxI;
					if (b) {
						return i;
					}
				}
			}

			// no replace
			rangeBuff.push_back(this);
			return -1;
		}
		}

		for (int i = 0; i < divn; ++i) {
			RangeArr_node<IndexType, ValueType>& ran = ((RangeArr_node<IndexType, ValueType>*)child)[i];
			int index = ran.clean(rangeBuff);
			if (index >= 0) {
				if (ran.divn == 2) {
					free(ran.child); // 2siz valuetype
				}
				else if (ran.divn == 1) {
					free(ran.child); // 1siz valuetype
				}
				else if (ran.divn > 2) {
					free(ran.child); // divn siz value type
				}
				ran = *rangeBuff.at(index);
			}
			else {
				if (ran.divn > 2) {
					ui32 len = sizeof(RangeArr_node<IndexType, ValueType>) * ran.divn;
					void* ptr = fmhl.Allocate_ImortalMemory(len);
					memcpy_s(ptr, len, ran.child, len);
					ran.child = ptr;
				}
				else {
					ui32 len = sizeof(ValueType) * ran.divn;
					void* ptr = fmhl.Allocate_ImortalMemory(len);
					memcpy_s(ptr, len, ran.child, len);
					ran.child = ptr;
				}
			}
		}

		for (int i = 0; i < rangeBuff.size(); ++i) {
			RangeArr_node<IndexType, ValueType>* ri = rangeBuff.at(i);
			if (ri->divn == divn) {
				bool b = ri->maxI == maxI;
				b = b && ri->cmax == cmax;
				for (int k = 0; k < divn; ++k) {
					b = b && (((RangeArr_node<IndexType, ValueType>*)ri->child)[k] == ((RangeArr_node<IndexType, ValueType>*)child)[k]);
				}

				if (b) {
					return i;
				}
				else {
					rangeBuff.push_back(this);
					return -1;
				}
			}
		}

		return -1;
	}

	bool operator==(RangeArr_node<IndexType, ValueType>& ref) {
		bool b = (divn == ref.divn);
		b = b && (maxI == ref.maxI);
		b = b && (cmax == ref.cmax);
		b = b && (child == ref.child);
		return b;
	}
};

template <typename IndexType, typename ValueType> struct RangeArr {
	unsigned int divn;
	IndexType minI;
	ValueType maxI;
	IndexType cmax;
	void* child;

	ValueType& operator[](IndexType index) {
		ValueType v = 0;
		if (minI > index || index > maxI) return v;
		IndexType previndex = index - minI;
		IndexType movindex = previndex / cmax;
		RangeArr_node<IndexType, ValueType> ran = ((RangeArr_node<IndexType, ValueType>*)child)[movindex];
		previndex -= movindex * cmax;
	RANGEARR_ACCESSOPERATION_LOOP:
		switch (ran.divn) {
		case 1:
			return *(ValueType*)ran.child;
		case 2:
			previndex += ran.maxI;
			movindex = previndex / ran.cmax;
			return ((ValueType*)ran.child)[movindex];
			break;
		}
		movindex = previndex / ran.cmax;
		previndex -= movindex * ran.cmax;
		ran = ((RangeArr_node<IndexType, ValueType>*)ran.child)[movindex];
		goto RANGEARR_ACCESSOPERATION_LOOP;
	}
	void print_state(ofstream& ofs) {
		ofs << "divn : " << divn << " | minI : " << minI << " | maxI : " << maxI << " | cmax : " << cmax << endl;
		for (int i = 0; i < divn; ++i) {
			RangeArr_node<IndexType, ValueType> rad = ((RangeArr_node<IndexType, ValueType>*)child)[i];
			ofs << "\tchild[" << i << "] : ";
			rad.print_state(ofs);
		}
	}
	void clean() {
		vecarr<RangeArr_node<IndexType, ValueType>*> rangeBuff;
		rangeBuff.NULLState();
		rangeBuff.Init(8);

		void* ptr = fmhl.Allocate_ImortalMemory(sizeof(RangeArr_node<IndexType, ValueType>) * divn);
		memcpy_s(ptr, sizeof(RangeArr_node<IndexType, ValueType>) * divn, child, sizeof(RangeArr_node<IndexType, ValueType>) * divn);
		free(child);
		child = ptr;

		for (int i = 0; i < divn; ++i) {
			RangeArr_node<IndexType, ValueType>& ran = ((RangeArr_node<IndexType, ValueType>*)child)[i];
			int index = ran.clean(rangeBuff);
			if (index >= 0) {
				if (ran.divn == 2) {
					free(ran.child); // 2siz valuetype
				}
				else if (ran.divn == 1) {
					free(ran.child); // 1siz valuetype
				}
				ran.child = rangeBuff.at(index)->child;
			}
			else {
				if (ran.divn > 2) {
					ui32 len = sizeof(RangeArr_node<IndexType, ValueType>) * ran.divn;
					void* ptr = fmhl.Allocate_ImortalMemory(len);
					memcpy_s(ptr, len, ran.child, len);
					ran.child = ptr;
				}
				else {
					ui32 len = sizeof(ValueType) * ran.divn;
					void* ptr = fmhl.Allocate_ImortalMemory(len);
					memcpy_s(ptr, len, ran.child, len);
					ran.child = ptr;
				}
			}
		}
		rangeBuff.release();
	}
};

template <typename IndexType, typename ValueType> RangeArr_node<IndexType, ValueType> AddRangeArrNode(vecarr<range<IndexType, ValueType>> r) {
	RangeArr_node<IndexType, ValueType> darr;
	darr.divn = r.size();
	darr.maxI = r.last().end;

	// cmax set
	IndexType totalDomainLen = (darr.maxI + 1);
	IndexType temp = totalDomainLen % darr.divn;
	IndexType cmax = totalDomainLen / darr.divn;
	IndexType mov_mini = 0;
	if (temp != 0) {
		cmax += 1;
	}

	//divn set
	temp = totalDomainLen % cmax;
	IndexType dn = totalDomainLen / cmax;
	if (temp != 0) {
		dn += 1;
	}
	darr.divn = dn;
	darr.cmax = cmax;
	darr.child = nullptr;

	unsigned int range_counter = 0;
	RangeArr_node<IndexType, ValueType>* CArr = (RangeArr_node<IndexType, ValueType>*)malloc(sizeof(RangeArr_node<IndexType, ValueType>) * darr.divn);
	if (CArr == nullptr) return darr;
	for (int i = 0; i < darr.divn; ++i) {
		CArr[i].maxI = cmax - 1;
		unsigned int cr_count = 0;
		unsigned int r_start = range_counter;
		for (int k = range_counter; k < r.size(); ++k) {
			range<IndexType, ValueType> ran = r.at(k);
			if (ran.end <= cmax * (i + 1) - 1) {
				range_counter += 1;
				cr_count += 1;
				if (ran.end == cmax * (i + 1) - 1) {
					goto ADD_DYNAMICARR_ADD_CDARR;
				}
			}
			else {
				cr_count += 1;
				goto ADD_DYNAMICARR_ADD_CDARR;
			}
		}

	ADD_DYNAMICARR_ADD_CDARR:
		if (cr_count > 2) {
			vecarr<range<IndexType, ValueType>> r2;
			r2.NULLState();
			r2.Init(cr_count);
			/*CArr[i].divn = cr_count;*/
			for (int u = 0; u < cr_count; ++u) {
				range<IndexType, ValueType> tempR = r.at(r_start + u);
				tempR.end -= cmax * i;
				r2.push_back(r.at(r_start + u));
			}
			/*RangeArr_node<IndexType, ValueType>* ccarr = (RangeArr_node<IndexType, ValueType>*)malloc(sizeof(RangeArr_node<IndexType, ValueType>));
			*ccarr = AddRangeArrNode(r2);
			CArr[i].child = (void*)ccarr;*/
			CArr[i] = AddRangeArrNode(r2);
			r2.release();
		}
		else if (cr_count == 2) {
			vecarr<range<IndexType, ValueType>> r2;
			r2.NULLState();
			r2.Init(cr_count);
			CArr[i].divn = cr_count;
			for (int u = 0; u < cr_count; ++u) {
				range<IndexType, ValueType> tempR = r.at(r_start + u);
				tempR.end -= cmax * i;
				r2.push_back(tempR);
			}

			//RangeArr_node<IndexType, ValueType>* ccarr = (RangeArr_node<IndexType, ValueType>*)malloc(sizeof(RangeArr_node<IndexType, ValueType>));
			CArr[i].divn = 2;

			IndexType start = 0;
			IndexType end = darr.cmax - 1;
			IndexType margin_right = end - r2[0].end;
			IndexType margin_left = r2[0].end - start + 1;
			if (margin_left > margin_right) {
				CArr[i].cmax = margin_left;
				CArr[i].maxI = 0;
			}
			else {
				CArr[i].cmax = margin_right;
				CArr[i].maxI = r2[0].end - margin_right + 1;
				CArr[i].maxI = start - CArr[i].maxI;
			}

			ValueType* varr = (ValueType*)malloc(sizeof(ValueType) * 2);
			if (varr != nullptr) {
				varr[0] = r2[0].value;
				varr[1] = r2[1].value;
			}
			CArr[i].child = (void*)varr;
			r2.release();
		}
		else {
			CArr[i].divn = 1;
			CArr[i].cmax = 1;
			CArr[i].maxI = 0;
			ValueType* varr = (ValueType*)malloc(sizeof(ValueType));
			if (varr != nullptr) {
				*varr = r.at(r_start).value;
			}
			CArr[i].child = varr;
		}
	}

	darr.child = CArr;
	return darr;
}

template <typename IndexType, typename ValueType> 
RangeArr<IndexType, ValueType> AddRangeArr(unsigned int mini, vecarr<range<IndexType, ValueType>> r) {
	RangeArr<IndexType, ValueType> darr;
	darr.divn = r.size();
	darr.minI = mini;
	darr.maxI = r.last().end;

	//cmax set
	IndexType totalDomainLen = (darr.maxI - darr.minI + 1);
	IndexType temp = totalDomainLen % darr.divn;
	IndexType cmax = totalDomainLen / darr.divn;
	IndexType mov_mini = mini;
	if (temp != 0) {
		cmax += 1;
	}

	//divn set
	temp = totalDomainLen % cmax;
	IndexType dn = totalDomainLen / cmax;
	if (temp != 0) {
		dn += 1;
	}
	darr.divn = dn;
	darr.cmax = cmax;
	darr.child = nullptr;

	unsigned int range_counter = 0;
	RangeArr_node<IndexType, ValueType>* CArr = (RangeArr_node<IndexType, ValueType>*)malloc(sizeof(RangeArr_node<IndexType, ValueType>) * darr.divn);

	if (CArr == nullptr) return darr;
	for (int i = 0; i < darr.divn; ++i) {
		CArr[i].maxI = cmax - 1;
		unsigned int cr_count = 0;
		unsigned int r_start = range_counter;
		for (int k = range_counter; k < r.size(); ++k) {
			range<IndexType, ValueType> ran = r.at(k);
			if (ran.end <= darr.minI + cmax * (i + 1) - 1) {
				range_counter += 1;
				cr_count += 1;
				if (ran.end == darr.minI + cmax * (i + 1) - 1) {
					goto ADD_DYNAMICARR_ADD_CDARR;
				}
			}
			else {
				cr_count += 1;
				goto ADD_DYNAMICARR_ADD_CDARR;
			}
		}

	ADD_DYNAMICARR_ADD_CDARR:
		if (cr_count > 2) {
			vecarr<range<IndexType, ValueType>> r2;
			r2.NULLState();
			r2.Init(cr_count);
			//CArr[i].divn = cr_count;
			for (int u = 0; u < cr_count; ++u) {
				range<IndexType, ValueType> tempR = r.at(r_start + u);
				tempR.end -= darr.minI + cmax * i;
				r2.push_back(tempR);
			}
			//RangeArr_node<IndexType, ValueType>* ccarr = (RangeArr_node<IndexType, ValueType>*)malloc(sizeof(RangeArr_node<IndexType, ValueType>));
			//*ccarr = 
			//CArr[i].child = (void*)ccarr;
			CArr[i] = AddRangeArrNode(r2);
			r2.release();
		}
		else if (cr_count == 2) {
			vecarr<range<IndexType, ValueType>> r2;
			r2.NULLState();
			r2.Init(cr_count);
			CArr[i].divn = cr_count;
			for (int u = 0; u < cr_count; ++u) {
				range<IndexType, ValueType> tempR = r.at(r_start + u);
				tempR.end -= darr.minI + cmax * i;
				r2.push_back(tempR);
			}

			//RangeArr_node<IndexType, ValueType>* ccarr = (RangeArr_node<IndexType, ValueType>*)malloc(sizeof(RangeArr_node<IndexType, ValueType>));
			CArr[i].divn = 2;

			IndexType start = 0;
			IndexType end = darr.cmax - 1;
			IndexType margin_right = end - r2[0].end;
			IndexType margin_left = r2[0].end - start + 1;
			if (margin_left > margin_right) {
				CArr[i].cmax = margin_left;
				CArr[i].maxI = 0;
			}
			else {
				CArr[i].cmax = margin_right;
				CArr[i].maxI = r2[0].end - margin_right + 1;
				CArr[i].maxI = start - CArr[i].maxI;
			}

			ValueType* varr = (ValueType*)malloc(sizeof(ValueType) * 2);
			if (varr != nullptr) {
				varr[0] = r2[0].value;
				varr[1] = r2[1].value;
				CArr[i].child = (void*)varr;
			}
			r2.release();
		}
		else {
			CArr[i].divn = 1;
			CArr[i].cmax = 1;
			CArr[i].maxI = 0;
			ValueType* varr = (ValueType*)malloc(sizeof(ValueType));
			if (varr != nullptr) {
				*varr = r.at(r_start).value;
			}
			CArr[i].child = varr;
		}
	}

	darr.child = CArr;
	return darr;
}

#define MAX_THREAD_COUNT_DIV8 2
#define MAX_THREAD_COUNT 16
//max thread count is 32.
class FM_System0
{
public:
	byte8 threadID_allocater[MAX_THREAD_COUNT] = {};
	// 1 - unallocated | 0 - allocated

	//static constexpr int midminsize = 72;	// x64
	//int fm1_sizetable[128] = { };
	RangeArr<int, int> fm1_sizetable;

	//page1
	FmTempStack* tempStack; // vecarr < FmTempStack * >
	//page2
	ImmortalExpendArr* UnderLumpSiz_HeapDebugFM; // vecarr <page_alloc>
	ImmortalExpendArr* OverLumpSiz_HeapDebugFM; // vecarr<void*>
	//other
	FmFlagLayer UnderPageSiz_HeapDebugFM[12]; // vecarr < FmFlagLayer*>

	FM_System0()
	{

	}

	virtual ~FM_System0()
	{

	}

	void RECORD_NonReleaseHeapData() {
#ifdef FM_GET_NONRELEASE_HEAPPTR
		ofstream thout;
		thout.open("Editing/fm_NonReleaseHeapData.txt");
		for (int i = 0; i < 13; ++i) {
			thout << "mulindex_: " << i << endl;
			FmFlagLayer fm1 = UnderPageSiz_HeapDebugFM[i];
			thout << "fmsiz: " << fm1.flagPageArr->size() << endl;
			for (int k = 0; k < fm1.flagPageArr->size(); ++k) {
				FmFlagPage* fm11 = fm1.flagPageArr->at<FmFlagPage*>(k);
				thout << "ID : " << (ui64)fm11->GetID() << endl;

				vecarr<int> addrs;
				addrs.NULLState();
				addrs.Init(8);
				uint64_t pivot = reinterpret_cast<uint64_t>(fm11->page);
				for (int i0 = 0; i0 < SMALL_PAGE_SIZE; i0 += fm11->GetBytePerLifeFlag())
				{
					if (fm11->isAllocated(fm11->page + i0, 1) == true) {
						addrs.push_back(i0);
						while (fm11->isAllocated(&(fm11->page->Data[0]) + i0, 1) == true) {
							i0 += fm11->GetBytePerLifeFlag();
						}
					}
				}

				thout << "NonReleaseHeapNum_: " << addrs.size() << endl;
				for (int i0 = 0; i0 < addrs.size(); ++i0) {
					thout << addrs.at(i0) << " ";
				}
				thout << endl;
				addrs.release();
			}
		}
#endif
	}

	RangeArr<int, int> CreateFm1SizeTable()
	{
		RangeArr<int, int> sizeGraph;
		int sizearr[13] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
		vecarr<range<int, int>> rangeArr;
		rangeArr.NULLState();
		rangeArr.Init(340);
		range<int, int> currange;
		currange.value = 0;
		currange.end = 1;
		for (int i = 1; i < 4096; ++i)
		{
			int res[13] = {};
			for (int k = 0; k < 13; ++k)
			{
				res[k] = getcost(i, sizearr[k]);
			}
			int mini = 0;
			int min = minarr(13, res, &mini);
			// cout << i << "\t" << sizearr[mini] << " : \t" << (float)res[mini] / 8.0f << "(" << res[mini] << ")\t" << "additional bit : " << res[mini] - i * 8 << endl;

			if (currange.value == mini)
			{
				continue;
			}
			else
			{
				currange.end = i - 1;
				rangeArr.push_back(currange);
				currange.end = 0;
				currange.value = mini;
			}
		}

		currange.end = 4096;
		rangeArr.push_back(currange);
		sizeGraph = AddRangeArr<int, int>(1, rangeArr);
		//sizeGraph.clean();
		// sizeGraph->print_state();
		return sizeGraph;
	}

	void SetHeapData()
	{
#ifdef FM_NONRELEASE_HEAPCHECK
		checkarr.NULLState();
		checkarr.Init(9);
		checkarr.up = 8;
		ifstream thin("Editing/fm_NonReleaseHeapData.txt");
		string tempstr;
		for (int i = 0; i < 8; ++i) {
			thin >> tempstr; // mulindex_:
			thin >> tempstr; // i
			thin >> tempstr; // fmsiz:
			int fmsiz = 0;
			thin >> fmsiz;
			checkarr.at(i).NULLState();
			checkarr.at(i).Init(fmsiz + 1);
			for (size_t k = 0; k < fmsiz; k++)
			{
				checkarr.at(i).at(k).NULLState();
			}

			for (int k = 0; k < fmsiz; ++k) {
				thin >> tempstr; // id_:
				int id = 0;
				thin >> id;
				vecarr<uint64_t>* chvec = &checkarr.at(i).at(id);
				thin >> tempstr; //NonReleaseHeapNum_:
				int intsiz = 0;
				thin >> intsiz;
				chvec->NULLState();
				chvec->Init(intsiz + 1);
				for (int u = 0; u < intsiz; ++u) {
					uint64_t value;
					thin >> value;
					chvec->push_back(value);
				}
			}
		}
		thin.close();
#endif

		for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
			threadID_allocater[i] = 255;
		}

		tempStack = (FmTempStack*)fmhl.AllocNewPages(2 * SMALL_PAGE_SIZE);
		for (int i = 0; i < MAX_THREAD_COUNT; ++i)
		{
			tempStack[i].init();
		}

		fm1_sizetable = CreateFm1SizeTable();

		for (int i = 0; i < 13; ++i) {
			UnderPageSiz_HeapDebugFM[i].Init(1 << i);
		}

		UnderLumpSiz_HeapDebugFM = (ImmortalExpendArr*)fmhl.AllocExpendArr();
		UnderLumpSiz_HeapDebugFM->Arr = fmhl.AllocNewSinglePage();
		UnderLumpSiz_HeapDebugFM->capacity = 256;
		UnderLumpSiz_HeapDebugFM->up = 0;

		OverLumpSiz_HeapDebugFM = (ImmortalExpendArr*)fmhl.AllocExpendArr();
		OverLumpSiz_HeapDebugFM->Arr = fmhl.AllocNewSinglePage();
		OverLumpSiz_HeapDebugFM->capacity = 512;
		OverLumpSiz_HeapDebugFM->up = 0;
	}

	void allocate_thread_fmTempMem()
	{
		for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
			if (threadID_allocater[i] != 0) {
				for (int k = 0; k < 8; ++k) {
					if (threadID_allocater[i] & (1 << k)) {
						threadID_allocater[i] |= (1 << k);
						threadID = i * 8 + k;
						return;
					}
				}
			}
		}
	}

	void free_thread_fmTempMem() {
		unsigned int i = threadID / 8;
		unsigned int k = threadID % 8;
		threadID_allocater[i] &= ~(1 << k);
	}

	void dbg_fm1_lifecheck()
	{
		cout << "----------------fmsystem-----------------" << endl;
		for (int k = 0; k < 12; ++k)
		{
			cout << "bytePerFlag : " << UnderPageSiz_HeapDebugFM[k].bytePerFlag << endl;
			for (int i = 0; i < UnderPageSiz_HeapDebugFM[k].flagPageArr->size(); ++i)
			{
				FmFlagPage* fm = UnderPageSiz_HeapDebugFM[k].flagPageArr->at<FmFlagPage*>(i);
				cout << "\nFM1_" << i << "=" << fm << endl;
				cout << "FUP : " << fm->Fup << "/" << SMALL_PAGE_SIZE << endl;
				fm->dbg_lifecheck();
			}
		}
	}

	void* _inline_fastNew(ui32 byteSiz) {
		if (byteSiz == 0) return nullptr;
		ui32 cs = (!(byteSiz >> 12) << 1) + !(byteSiz >> 21);
		/*
		* cs == 11 -> UnderPageSiz_HeapDebugFM
		 (cs == 10 is not exist..)
		* cs == 01 -> UnderLumpSiz_HeapDebugFM
		* cs == 00 -> OverLumpSiz_HeapDebugFM
		*/

		switch (cs) {
		case 3:
		case 2:
		{
			ui32 index = fm1_sizetable[byteSiz];

			ui32 lm = 1 << index;
			ui32 p = (byteSiz & (lm - 1));
			ui32 fitsiz = byteSiz - p + !!p * lm;
			
			FmFlagLayer fml = UnderPageSiz_HeapDebugFM[index];
			
			for (int i = 0; i < fml.flagPageArr->up; ++i) {
				FmFlagPage* fp = ((FmFlagPage**)fml.flagPageArr->Arr)[i];
				void* lptr = (byte8*)fp->page + fp->Fup;

				ui32 postFup = fp->Fup + fitsiz;
				if (postFup >= SMALL_PAGE_SIZE)
				{
					return nullptr;
				}
				//Fast Mode
				 
				//fp->SetFlag1(fp->Fup, fitsiz);
				ui32 DataStart = fp->Fup;
				ui32 temp = DataStart >> index;
				ui32 temp2 = (DataStart + fitsiz - 1) >> index;
				ui32 startflagindex = temp >> 3;
				ui32 endflagindex = temp2 >> 3;
				ui32 startflag_bitindex = temp & 7;
				ui32 endflag_bitindex = temp2 & 7;

				ui32 deltaflagindex = endflagindex - startflagindex;
				ui32 simdState = _tzcnt_u32(deltaflagindex);
				byte8* fdata = ((byte8*)fp->flagData);
				byte8 startFlagMask = -(byte8)(1 << startflag_bitindex);
				byte8 endFlagMask = (byte8)(2 << endflag_bitindex) - 1;
				ui16* u16_fdata = (ui16*)fdata;
				//ui32 startLCENindex = startflagindex >> 2;
				//ui32 endLCENindex = endflagindex >> 2;
				switch (deltaflagindex) {
				case 0:
					fdata[startflagindex] |= (startFlagMask & endFlagMask);
					//fp->SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
					goto FM_FASTNEW_FINISH_FLAGCHAGE;
				case 1:
					fdata[startflagindex] |= startFlagMask;
					fdata[endflagindex] |= endFlagMask;
					//fp->SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
					//fp->SetlcenData(endLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
					goto FM_FASTNEW_FINISH_FLAGCHAGE;
				}

				fdata[startflagindex] |= startFlagMask;
				fdata[endflagindex] |= ~endFlagMask;

				
				switch (simdState) {
				case 0:
					for (int i = startflagindex + 1; i < endflagindex; ++i) {
						fdata[i] = 255;
					}
					break;
				case 1:
					for (int i = startflagindex + 1; i < endflagindex; i += 2) {
						*((ui16*)&fdata[i]) = 65535;
					}
					break;
				case 2:
					for (int i = startflagindex + 1; i < endflagindex; i += 4) {
						*((ui32*)&fdata[i]) = 4294967295;
					}
					break;
				case 3:
					for (int i = startflagindex + 1; i < endflagindex; i += 8) {
						*((ui64*)&fdata[i]) = ~0;
					}
					break;
				case 4:
					for (int i = startflagindex + 1; i < endflagindex; i += 16) {
						*((__m128*) & fdata[i]) = max128.simd;
					}
					break;
				case 5:
					for (int i = startflagindex + 1; i < endflagindex; i += 32) {
						*((__m256*) & fdata[i]) = max256.simd;
					}
					break;
				case 6:
					for (int i = startflagindex + 1; i < endflagindex; i += 64) {
						*((__m512*)&fdata[i]) = max512.simd;
					}
					break;
				}

				//LCEN Data Update
				/*fp->SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));
				for (int i = startLCENindex + 1; i < endLCENindex; ++i) {
					fp->SetlcenData(i, _tzcnt_u16(u16_fdata[startLCENindex]));
				}
				fp->SetlcenData(startLCENindex, _tzcnt_u16(u16_fdata[startLCENindex]));*/

				//fp->SetFlag1(fp->Fup, fitsiz);

				FM_FASTNEW_FINISH_FLAGCHAGE:
				fp->Fup = postFup;
				return lptr;
			}

			// require new flagpage
			
			FmFlagPage* flagpage = (FmFlagPage*)fmhl.Allocate_ImortalMemory(sizeof(FmFlagPage));
			flagpage->Init(fml.bytePerFlag);
			fml.flagPageArr->push_back<FmFlagPage*>(flagpage);
			//flagpage->dbg_lifecheck();
			void* ptr = flagpage->_fastNew(byteSiz);
			return ptr;
		}
		case 1:
		{
			FmPageAlloc page_alloc;
			page_alloc.capacity = (((byteSiz - 1) >> 12) + 1) << 12;
			page_alloc.page = fmhl.AllocNewPages(page_alloc.capacity);
			page_alloc.size = byteSiz;
			UnderLumpSiz_HeapDebugFM->push_back<FmPageAlloc>(page_alloc);
			return page_alloc.page;
		}
		case 0:
		{
			void* ptr = malloc(byteSiz);
			OverLumpSiz_HeapDebugFM->push_back<void*>(ptr);
			return ptr;
		}
		}

		return nullptr;
	}

	void* _fastnew(ui32 byteSiz)
	{
		if (byteSiz == 0) return nullptr;
		ui32 cs = (!(byteSiz >> 12) << 1) + !(byteSiz >> 21);
		/*
		* cs == 11 -> UnderPageSiz_HeapDebugFM
		 (cs == 10 is not exist..)
		* cs == 01 -> UnderLumpSiz_HeapDebugFM
		* cs == 00 -> OverLumpSiz_HeapDebugFM
		*/

		switch (cs) {
		case 3:
		case 2:
		{
			ui32 index = fm1_sizetable[byteSiz];
			return UnderPageSiz_HeapDebugFM[index]._fastNew(byteSiz);
		}
		case 1:
		{
			FmPageAlloc page_alloc;
			page_alloc.capacity = (((byteSiz - 1) >> 12) + 1) << 12;
			page_alloc.page = fmhl.AllocNewPages(page_alloc.capacity);
			page_alloc.size = byteSiz;
			UnderLumpSiz_HeapDebugFM->push_back<FmPageAlloc>(page_alloc);
			return page_alloc.page;
		}
		case 0:
		{
			void* ptr = malloc(byteSiz);
			OverLumpSiz_HeapDebugFM->push_back<void*>(ptr);
			return ptr;
		}
		}
	}

	void* _savenew(ui32 byteSiz)
	{
		ui32 cs = !(byteSiz >> 12) << 1 + !(byteSiz >> 21);
		/*
		* cs == 11 -> UnderPageSiz_HeapDebugFM
		 (cs == 10 is not exist..)
		* cs == 01 -> UnderLumpSiz_HeapDebugFM
		* cs == 00 -> OverLumpSiz_HeapDebugFM
		*/

		switch (cs) {
		case 3:
		case 2:
			return UnderPageSiz_HeapDebugFM[fm1_sizetable[byteSiz]]._saveNew(byteSiz);
		case 1:
		{
			FmPageAlloc page_alloc;
			page_alloc.capacity = (((byteSiz - 1) >> 12) + 1) << 12;
			page_alloc.page = fmhl.AllocNewPages(page_alloc.capacity);
			page_alloc.size = byteSiz;
			UnderLumpSiz_HeapDebugFM->push_back<FmPageAlloc>(page_alloc);
			return page_alloc.page;
		}
		case 0:
		{
			void* ptr = malloc(byteSiz);
			OverLumpSiz_HeapDebugFM->push_back<void*>(ptr);
			return ptr;
		}
		}
	}

	inline void _tempPushLayer()
	{
		tempStack[threadID].PushLayer();
	}

	inline void _tempPopLayer()
	{
		tempStack[threadID].PopLayer();
	}

	void* _tempNew(unsigned int byteSiz, int fmlayer = -1)
	{
		return tempStack[threadID]._New(byteSiz, fmlayer);
	}

	void* _New(unsigned int byteSiz, bool isHeapDebug, int fmlayer = -1)
	{
		if (isHeapDebug == false)
		{
			return tempStack[threadID]._New(byteSiz, fmlayer);
		}
		else
		{
			return _fastnew(byteSiz);
		}
	}

	bool _Delete(void* variable, unsigned int size)
	{
		if (size == 0) return false;
		ui32 cs = (!(size >> 12) << 1) + !(size >> 21);
		switch (cs) {
		case 3:
		case 2:
			return UnderPageSiz_HeapDebugFM[fm1_sizetable[size]].Delete(variable, size);
		case 1:
		{
			for (int i = 0; i < UnderLumpSiz_HeapDebugFM->up; ++i) {
				FmPageAlloc pa = ((FmPageAlloc*)UnderLumpSiz_HeapDebugFM->Arr)[i];
				if (pa.page == variable && pa.size == size) {
					fmhl.RecyclePages(pa.page, pa.capacity);
					UnderLumpSiz_HeapDebugFM->erase<FmPageAlloc>(i);
					return true;
				}
			}
			return false;
		}
		case 0:
		{
			for (int i = 0; i < OverLumpSiz_HeapDebugFM->up; ++i) {
				void* ptr = ((void**)OverLumpSiz_HeapDebugFM->Arr)[i];
				if (ptr == variable) {
					free(ptr);
					OverLumpSiz_HeapDebugFM->erase<FmPageAlloc>(i);
					return true;
				}
			}
			return false;
		}
		}
		return false;
	}

	bool isAllocated(byte8* variable, unsigned int size)
	{
		ui32 cs = !(size >> 12) << 1 + !(size >> 21);
		switch (cs) {
		case 3:
		case 2:
			return UnderPageSiz_HeapDebugFM[fm1_sizetable[size]].isAllocated(variable, size);
		case 1:
		{
			for (int i = 0; i < UnderLumpSiz_HeapDebugFM->up; ++i) {
				FmPageAlloc pa = ((FmPageAlloc*)UnderLumpSiz_HeapDebugFM->Arr)[i];
				if (pa.page == variable && pa.size == size) {
					return true;
				}
			}
			return false;
		}
		case 0:
		{
			for (int i = 0; i < OverLumpSiz_HeapDebugFM->up; ++i) {
				void* ptr = ((void**)OverLumpSiz_HeapDebugFM->Arr)[i];
				if (ptr == variable) {
					return true;
				}
			}
			return false;
		}
		}
		return false;
	}
};

FM_System0* fm;

template < typename T > class fmvecarr
{
public:
	T* Arr;
	size_t maxsize = 0;
	int up = 0;
	bool islocal = true;
	bool isdebug = false;
	int fmlayer = -1;

	fmvecarr()
	{
		Arr = nullptr;
		maxsize = 0;
		up = 0;
		islocal = true;
		fmlayer = -1;
		isdebug = false;
	}

	~fmvecarr()
	{
		if (islocal)
		{
			if (isdebug)
			{
				fm->_Delete(reinterpret_cast <byte8*>(Arr), sizeof(T) * maxsize);
			}
			Arr = nullptr;
		}
	}

	void NULLState()
	{
		Arr = nullptr;
		maxsize = 0;
		up = 0;
		fmlayer = -1;
		isdebug = false;
	}

	void Init(size_t siz, bool local, bool isdebug = false, int pfmlayer = -1)
	{
		T* newArr;
		if (isdebug)
		{
			newArr = (T*)fm->_New(sizeof(T) * siz, isdebug);
		}
		else
		{
			fmlayer = pfmlayer;
			newArr = (T*)fm->_tempNew(sizeof(T) * siz, fmlayer);
			if (fmlayer < 0) {
				fmlayer = fm->tempStack[threadID]->layer.size() - 1;
			}
		}
		if (Arr != nullptr)
		{
			for (int i = 0; i < maxsize; ++i)
			{
				newArr[i] = Arr[i];
			}

			if (isdebug)
			{
				fm->_Delete(reinterpret_cast <byte8*>(Arr), sizeof(T) * maxsize);
			}
			Arr = nullptr;
		}

		islocal = local;
		Arr = newArr;
		maxsize = siz;
		isdebug = isdebug;
	}

	T& at(size_t i)
	{
		return Arr[i];
	}

	T& operator[](size_t i) const
	{
		return Arr[i];
	}

	void push_back(T value)
	{
		if (up < maxsize)
		{
			Arr[up] = value;
			up += 1;
		}
		else
		{
			Init(maxsize * 2 + 1, islocal, isdebug, fmlayer);
			Arr[up] = value;
			up += 1;
		}
	}

	void pop_back()
	{
		if (up - 1 >= 0)
		{
			up -= 1;
			// Arr[up] = 0;
		}
	}

	void erase(size_t i)
	{
		for (int k = i; k < up; ++k)
		{
			Arr[k] = Arr[k + 1];
		}
		up -= 1;
	}

	void insert(size_t i, T value)
	{
		push_back(value);
		for (int k = maxsize - 1; k > i; k--)
		{
			Arr[k] = Arr[k - 1];
		}
		Arr[i] = value;
	}

	inline size_t size() const
	{
		return up;
	}

	void clear()
	{
		if (Arr != nullptr && isdebug)
			fm->_Delete(reinterpret_cast <byte8*>(Arr), sizeof(T) * maxsize);
		Arr = nullptr;
		up = 0;

		Init(2, islocal, isdebug, fmlayer);
	}

	T& last() const
	{
		if (up > 0)
		{
			return Arr[up - 1];
		}
		return Arr[0];
	}

	void release()
	{
		if (Arr != nullptr && isdebug)
			fm->_Delete(reinterpret_cast <byte8*>(Arr), sizeof(T) * maxsize);
		Arr = nullptr;
		up = 0;
		islocal = false;
	}
};

template < typename T > struct fmlist_node
{
	T value;
	fmlist_node < T >* next = nullptr;
	fmlist_node < T >* prev = nullptr;
};

template < typename T > class fmlist
{
public:
	fmlist_node < T >* first;
	size_t size = 0;
	short fmlayer = -1;
	bool isdebug = false;
	bool islocal = false;

	fmlist() {}
	~fmlist() {
		if (!islocal)
		{
			while (first->next != nullptr)
			{
				erase(first);
			}
			if (isdebug)
			{
				fm->_Delete(reinterpret_cast <byte8*>(first), sizeof(fmlist_node < T >));
			}
		}
	}

	void NULLState() {
		first = nullptr;
		size = 0;
		fmlayer = -1;
		isdebug = false;
		islocal = false;
	}

	void Init(bool isDbg, bool isLocal, short pfmlayer) {
		isdebug = isDbg;
		islocal = isLocal;
		fmlayer = pfmlayer;
		if (fmlayer < 0) {
			fmlayer = fm->tempStack[threadID]->layer.size() - 1;
		}
	}

	void release()
	{
	}

	void push_front(T value)
	{
		fmlist_node < T >* sav = first;
		first = (fmlist_node < T > *)fm->_New(sizeof(fmlist_node < T >), isdebug, (int)fmlayer);
		first->value = value;
		first->next = sav;
		first->prev = nullptr;
		sav->prev = first;
		++size;
	}

	inline fmlist_node < T >* getnext(fmlist_node < T >* node)
	{
		return node->next;
	}

	inline fmlist_node < T >* getprev(fmlist_node < T >* node)
	{
		return node->prev;
	}

	void erase(fmlist_node < T >* node)
	{
		if (node == first)
		{
			first = node->next;
		}
		if (node->prev != nullptr)
		{
			node->prev->next = node->next;
		}
		if (node->next != nullptr)
		{
			node->next->prev = node->prev;
		}
		if (isdebug)
		{
			fm->_Delete(reinterpret_cast <byte8*>(node), sizeof(fmlist_node < T >));
		}
		--size;
	}
};

//TODO : optimazation (need experiments)
//1. percent
// : ~([~0] << p)
// : 2 << p - 1
// : filter[p]
//16byte
template <typename T>
class fmCirculArr
{
public:
	T* arr = nullptr; // 8byte
	ui32 pivot = 0; // 4byte = 32bit - 12bit 20bit -> 100000capacity
	ui16 maxsiz_pow2 = 4; // array maxsiz = 1 << maxsiz_pow2; // pow(w, maxsiz_pow2);
	bool mbDbg = true;
	static constexpr unsigned int filter[33] = {
		0, 			// 0b 0000 0000 0000 0000 0000 0000 0000 0000
		1, 			// 0b 0000 0000 0000 0000 0000 0000 0000 0001
		3, 			// 0b 0000 0000 0000 0000 0000 0000 0000 0011
		7, 			// 0b 0000 0000 0000 0000 0000 0000 0000 0111
		15, 		// 0b 0000 0000 0000 0000 0000 0000 0000 1111
		31, 		// 0b 0000 0000 0000 0000 0000 0000 0001 1111
		63, 		// 0b 0000 0000 0000 0000 0000 0000 0011 1111
		127, 		// 0b 0000 0000 0000 0000 0000 0000 0111 1111
		255, 		// 0b 0000 0000 0000 0000 0000 0000 1111 1111
		511, 		// 0b 0000 0000 0000 0000 0000 0001 1111 1111
		1023, 		// 0b 0000 0000 0000 0000 0000 0011 1111 1111
		2047, 		// 0b 0000 0000 0000 0000 0000 0111 1111 1111
		4095, 		// 0b 0000 0000 0000 0000 0000 1111 1111 1111
		8191, 		// 0b 0000 0000 0000 0000 0001 1111 1111 1111
		16383, 		// 0b 0000 0000 0000 0000 0011 1111 1111 1111
		32767, 		// 0b 0000 0000 0000 0000 0111 1111 1111 1111
		65535, 		// 0b 0000 0000 0000 0000 1111 1111 1111 1111
		131071, 	// 0b 0000 0000 0000 0001 1111 1111 1111 1111
		262143, 	// 0b 0000 0000 0000 0011 1111 1111 1111 1111
		524287, 	// 0b 0000 0000 0000 0111 1111 1111 1111 1111
		1048575, 	// 0b 0000 0000 0000 1111 1111 1111 1111 1111
		2097151, 	// 0b 0000 0000 0001 1111 1111 1111 1111 1111
		4194303, 	// 0b 0000 0000 0011 1111 1111 1111 1111 1111
		8388607, 	// 0b 0000 0000 0111 1111 1111 1111 1111 1111
		16777215, 	// 0b 0000 0000 1111 1111 1111 1111 1111 1111
		33554431, 	// 0b 0000 0001 1111 1111 1111 1111 1111 1111
		67108863, 	// 0b 0000 0011 1111 1111 1111 1111 1111 1111
		134217727, 	// 0b 0000 0111 1111 1111 1111 1111 1111 1111
		268435455, 	// 0b 0000 1111 1111 1111 1111 1111 1111 1111
		536870911, 	// 0b 0001 1111 1111 1111 1111 1111 1111 1111
		1073741823, // 0b 0011 1111 1111 1111 1111 1111 1111 1111
		2147483647, // 0b 0111 1111 1111 1111 1111 1111 1111 1111
		4294967295, // 0b 1111 1111 1111 1111 1111 1111 1111 1111
	};
	static constexpr unsigned int uintMax = 4294967295;
	//freemem::FM_System0 *fm;

	fmCirculArr()
	{
	}

	~fmCirculArr()
	{
	}


	fmCirculArr(const fmCirculArr<T>& ref)
	{
		pivot = ref.pivot;
		maxsiz_pow2 = ref.maxsiz_pow2;
		arr = ref.arr;
		mbDbg = ref.mbDbg;
	}

	void Init(int maxsiz_pow, bool isdbg, int fmlayer = -1)
	{
		maxsiz_pow2 = maxsiz_pow;
		mbDbg = isdbg;
		arr = (T*)fm->_New(sizeof(T) * (1 << maxsiz_pow2), mbDbg, fmlayer);
		pivot = 0;
	}

	void Release()
	{
		if (mbDbg) {
			fm->_Delete((byte8*)arr, sizeof(T) * (1 << maxsiz_pow2));
		}
		arr = nullptr;
	}

	inline void move_pivot(int dist)
	{
		pivot = ((1 << maxsiz_pow2) + pivot + dist) & (~(uintMax << (maxsiz_pow2)));
	}

	inline T& operator[](int index)
	{
		int realindex = (index + pivot) & (~(uintMax << (maxsiz_pow2)));
		return arr[realindex];
	}

	void dbg()
	{
		ui32 max = (1 << maxsiz_pow2);
		for (int i = 0; i < max; ++i)
		{
			cout << this->operator[](i) << " ";
		}
		cout << endl;
	}
};

/*
TODO : list
1. match name of member var (ok)
2. use <<, >>, & instead *, /, % (ok)
3. normal casting > reinterpret_cast<> (ok)
4. data align (16 or 32byte) -> after finish all task, data align start.
5. current Data caching (when [] operator use, if last index +- value is in same fagment, do not excute logic and return add address value.) (ok)
*/
//32byte
template <typename T>
class fmDynamicArr
{
public:
	fmCirculArr<int*>* ptrArray = nullptr; // 8byte
	fmCirculArr<T>* lastCArr = nullptr; // 8byte cache
	ui32 last_outerIndex = 0; // 4byte
	ui32 array_siz = 0; // up 4byte
	ui16 fragPercent = 0; //2byte
	si16 fmlayer = -1; // 2byte
	ui32 array_capacity = 10; // 4byte
	ui8 fragment_siz_pow2 = 10; // 1byte
	ui8 array_depth = 1; // 1byte
	bool mbDbg = true; // 1byte

	fmDynamicArr()
	{
	}
	~fmDynamicArr()
	{
	}

	void Init(int fmgsiz_pow, bool isdbg, int up = 0, int pfmlayer = -1)
	{
		if (ptrArray != nullptr) {
			release();
		}
		fragment_siz_pow2 = fmgsiz_pow;
		array_depth = 1;
		mbDbg = isdbg;
		fmlayer = pfmlayer;
		if (ptrArray == nullptr) {
			ptrArray = (fmCirculArr<int*> *)fm->_New(sizeof(fmCirculArr<int*>), mbDbg, (int)fmlayer);
			ptrArray->Init(fmgsiz_pow, mbDbg);
			int fmgsiz = 1 << fmgsiz_pow;
			for (int i = 0; i < fmgsiz; ++i) {
				ptrArray->operator[](i) = nullptr;
			}
		}

		if (ptrArray->operator[](0) == nullptr) {
			fmCirculArr<T>* arr =
				(fmCirculArr<T> *)fm->_New(sizeof(fmCirculArr<T>), mbDbg, (int)fmlayer);
			arr->Init(fragment_siz_pow2, mbDbg);
			ptrArray->operator[](0) = (int*)arr;
			lastCArr = arr;
		}

		for (int i = 1; i < (1 << fragment_siz_pow2); ++i)
		{
			ptrArray->operator[](i) = nullptr;
		}

		array_capacity = 1 << fragment_siz_pow2;
		array_siz = 0;

		fragPercent = ((1 << (fragment_siz_pow2)) - 1);

		lastCArr = this->get_bottom_array(0);

		// up -> depth
		if (up > 0) {
			const unsigned int pushN = 1 + (up >> fragment_siz_pow2);
			const unsigned int delta = (1 << fragment_siz_pow2);
			for (int i = 0; i < pushN; ++i) {
				array_siz += delta;
				T v;
				push_back(v);
			}

			array_siz = up;
		}

		if (fmlayer < 0) {
			fmlayer = fm->tempStack[threadID]->layer.size() - 1;
		}
	}

	int get_max_capacity_inthisArr()
	{
		return 1 << (fragment_siz_pow2 * (array_depth + 1));
	}

	/*
			void set(int index, T value)
			{
				//this->operator[index] = value;
				T nullv = 0;
				int fragPercent = ((1 << (fragment_siz_pow2+1))-1);
				if (index >= (1 << array_capacity_pow2))
				{
					return;
				}
				circularArray<int *> *ptr = ptrArray;
				for (int i = 0; i < array_depth; ++i)
				{
					ptr = reinterpret_cast<circularArray<int *> *>(ptr->operator[]((int)((index >> (fragment_siz_pow2 * (array_depth - i)))) & fragPercent));
				}
				circularArray<T> *vptr = reinterpret_cast<circularArray<T> *>(ptr);
				// T *vptr = ptr;
				int inindex = (int)(index) & fragPercent;
				vptr->operator[](inindex) = value;
			}
	*/

	void push_back(T value)
	{
		if (array_siz + 1 <= array_capacity)
		{
			//set(array_siz, value);
			if (array_siz == 256) {
				cout << "break!" << endl;
			}
			this->operator[](array_siz) = value;
			//(*this)[array_siz] = value;
			array_siz += 1;
		}
		else
		{
			if (array_siz + 1 > get_max_capacity_inthisArr())
			{
				// create new parent ptr array
				int* chptr = (int*)ptrArray;
				ptrArray = reinterpret_cast<fmCirculArr<int*> *>(fm->_New(sizeof(fmCirculArr<int*>), mbDbg, (int)fmlayer));
				ptrArray->Init(fragment_siz_pow2, mbDbg, (int)fmlayer);

				ptrArray->operator[](0) = chptr;
				array_depth += 1;
				for (int i = 1; i < (1 << fragment_siz_pow2); ++i)
				{
					ptrArray->operator[](i) = nullptr;
				}
			}
			// create child ptr arrays
			int next = array_siz;
			fmCirculArr<int*>* ptr = ptrArray;
			int upcapacity = 0;
			for (int i = 0; i < array_depth; ++i)
			{
				int inindex = (int)(next >> fragment_siz_pow2 * (array_depth - i)) & (unsigned int)fragPercent;

				upcapacity += (inindex) << (fragment_siz_pow2 * (array_depth - i));

				fmCirculArr<int*>* tptr = ptr;
				ptr = reinterpret_cast<fmCirculArr<int*> *>(tptr->operator[](inindex));
				if (ptr == nullptr)
				{
					if (i == array_depth - 1)
					{
						fmCirculArr<T>* aptr =
							reinterpret_cast<fmCirculArr<T> *>(fm->_New(sizeof(fmCirculArr<T>), mbDbg, (int)fmlayer));
						aptr->Init(fragment_siz_pow2, mbDbg, (int)fmlayer);
						tptr->operator[](inindex) = (int*)aptr;
						ptr = reinterpret_cast<fmCirculArr<int*> *>(tptr->operator[](inindex));
					}
					else
					{
						fmCirculArr<int*>* insptr =
							reinterpret_cast<fmCirculArr<int*>*>(fm->_New(sizeof(fmCirculArr<int*>), mbDbg, (int)fmlayer));
						insptr->Init(fragment_siz_pow2, mbDbg, (int)fmlayer);
						tptr->operator[](inindex) = (int*)insptr;

						ptr = reinterpret_cast<fmCirculArr<int*> *>(tptr->operator[](inindex));
					}
				}
			}
			fmCirculArr<T>* vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);
			// T *vptr = ptr;
			int inindex = (int)(next) & (unsigned int)fragPercent;
			upcapacity += 1 << fragment_siz_pow2;
			vptr->operator[](inindex) = value;
			// capacity update
			array_capacity = upcapacity;
			array_siz += 1;
		}
	}

	void pop_back()
	{
		T nullt = 0;
		//set(array_siz - 1, nullt);
		(*this)[array_siz - 1] = nullt;
		array_siz -= 1;
	}

	inline unsigned int size() {
		return array_siz;
	}
	//use caching. (pre bake function.)
	T& operator[](size_t index)
	{
		if ((last_outerIndex >> fragment_siz_pow2) == index >> fragment_siz_pow2) {
			ui32 k = index & (unsigned int)fragPercent;
			return lastCArr->operator[](k);
		}

		if (index >= array_capacity)
		{
			cout << "error! array index bigger than capacity!" << endl;
			T nullv;
			byte8* carr = reinterpret_cast<byte8*>(&nullv);
			for (int i = 0; i < sizeof(T); ++i) {
				carr[i] = 0;
			}
			return nullv;
		}

		fmCirculArr<int*>* ptr = ptrArray;
		for (int i = 0; i < array_depth; ++i)
		{
			int depth_index = (index >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
			ptr = reinterpret_cast<fmCirculArr<int*> *>(ptr->operator[](depth_index));
		}
		fmCirculArr<T>* vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);

		lastCArr = vptr;
		last_outerIndex = index;

		// T *vptr = ptr;
		int inindex = ((int)(index)) & (unsigned int)fragPercent;
		return vptr->operator[](inindex);
	}

	fmCirculArr<T>* get_bottom_array(int index)
	{
		if ((last_outerIndex >> fragment_siz_pow2) == index >> fragment_siz_pow2) {
			return lastCArr;
		}

		if (index >= array_capacity)
		{
			return nullptr;
		}
		fmCirculArr<int*>* ptr = ptrArray;
		for (int i = 0; i < array_depth; ++i)
		{
			int depth_index = (index >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
			ptr = reinterpret_cast<fmCirculArr<int*> *>(ptr->operator[](depth_index));
			//ptr = reinterpret_cast<fmCirculArr<int *> *>(ptr->operator[]((int)((index >> (1 << fragment_siz_pow2) * (array_depth - i))) & fragPercent));
		}
		fmCirculArr<T>* vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);
		return vptr;
	}

	fmCirculArr<int*>* get_ptr_array(int index, int height)
	{
		if (index >= array_capacity)
		{
			return nullptr;
		}
		fmCirculArr<int*>* ptr = ptrArray;
		for (int i = 0; i < array_depth - height; ++i)
		{
			int depth_index = (index >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
			ptr = reinterpret_cast<fmCirculArr<int*> *>(ptr->operator[](depth_index));
		}
		return ptr;
	}

	// direction : -1 or 1
	void move(int index, int direction, bool expend)
	{
		ui32 fragSiz = (1 << fragment_siz_pow2);
		T save;
		fmCirculArr<T>* corearr = get_bottom_array(index);
		int inindex = index & (unsigned int)fragPercent;
		int last = 0;
		if (direction > 0)
		{
			last = (fragSiz)-direction;
		}
		save = corearr->operator[](last);

		if (direction > 0)
		{
			for (int i = last; i >= inindex; --i)
			{
				corearr->operator[](i) = corearr->operator[](i - 1);
			}
		}
		else
		{
			for (int i = inindex - 1; i < fragSiz; ++i)
			{
				corearr->operator[](i) = corearr->operator[](i + 1);
			}
		}

		if (direction > 0)
		{
			int next = index;
			while (true)
			{
				next = ((int)(next >> fragment_siz_pow2) + 1) << fragment_siz_pow2;
				fmCirculArr<T>* temparr = get_bottom_array(next);

				if (temparr == nullptr)
				{
					if (expend)
					{
						push(save);
					}
					break;
				}

				fmCirculArr<T>* nextarr = nullptr;

				nextarr = get_bottom_array(next + fragSiz);
				T ss;
				if (nextarr == nullptr)
				{
					int ind = (array_siz - 1) & (unsigned int)fragPercent;
					ss = temparr->operator[](ind);
				}
				else
				{
					ss = temparr->operator[](fragSiz - 1);
				}

				temparr->move_pivot(-direction);
				if (direction > 0)
				{
					temparr->operator[](0) = save;
				}
				else
				{
					temparr->operator[](fragSiz - 1) = save;
				}

				save = ss;
			}
		}
		else
		{
			int next = array_siz - 1 + fragSiz;
			while (true)
			{
				next = ((int)(next >> fragment_siz_pow2) - 1) << fragment_siz_pow2;
				if (next < 0)
					break;
				fmCirculArr<T>* temparr = get_bottom_array(next);

				if (temparr == nullptr)
				{
					continue;
				}

				T ss;
				if ((next - fragSiz) >> fragment_siz_pow2 == index >> fragment_siz_pow2)
				{
					int ind = (array_siz - 1) & (unsigned int)fragPercent;
					ss = temparr->operator[](0);

					if (next + fragSiz >= array_siz)
					{
						temparr->move_pivot(-direction);
						array_siz -= 1;
					}
					else
					{
						temparr->move_pivot(-direction);
						temparr->operator[](fragSiz - 1) = save;
					}

					save = ss;

					corearr->operator[](fragSiz - 1) = ss;
					break;
				}
				else
				{
					ss = temparr->operator[](0);
				}

				if (next + fragSiz >= array_siz)
				{
					temparr->move_pivot(-direction);
					array_siz -= 1;
				}
				else
				{
					temparr->move_pivot(-direction);
					temparr->operator[](fragSiz - 1) = save;
				}

				save = ss;
			}
		}
	}

	void printstate(char sig)
	{
		cout << sig << "_"
			<< "arr siz : " << array_siz << "[ ";
		for (int u = 0; u < array_siz; ++u)
		{
			if (u & (unsigned int)fragPercent == 0)
			{
				int uu = u;
				cout << ">";
				for (int k = 0; k < array_depth; ++k)
				{
					uu = uu >> fragment_siz_pow2;
					if (uu & (unsigned int)fragPercent == 0)
					{
						cout << ">";
					}
				}
			}
			cout << this->operator[](u) << ", ";
		}
		cout << "]" << endl;
	}

	void insert(int index, T value, bool expend)
	{
		move(index, 1, expend);
		//set(index, value);
		(*this)[index] = value;
	}

	void erase(int index)
	{
		if (index + 1 == array_siz)
		{
			T nullt = 0;
			//set(index, nullt);
			(*this)[index] = nullt;
			array_siz -= 1;
		}
		else
		{
			move(index + 1, -1, false);
		}
	}

	void NULLState()
	{
		ptrArray = nullptr;
		lastCArr = nullptr;
		last_outerIndex = 0;
		array_siz = 0;
		fragPercent = 0;
		array_capacity = 0;
		fragment_siz_pow2 = 0;
		array_depth = 1;
		mbDbg = false;
		fmlayer = -1;
	}

	T& at(size_t i)
	{
		ui32 ind = i;
		if ((last_outerIndex >> fragment_siz_pow2) == ind >> fragment_siz_pow2) {
			return lastCArr->operator[](ind& (unsigned int)fragPercent);
		}

		if (ind >= array_capacity)
		{
			T nullv;
			byte8* carr = reinterpret_cast<byte8*>(&nullv);
			for (int i = 0; i < sizeof(T); ++i) {
				carr[i] = 0;
			}
			cout << "error! array index bigger than capacity!" << endl;
			return nullv;
		}
		fmCirculArr<int*>* ptr = ptrArray;
		for (int i = 0; i < array_depth; ++i)
		{
			int depth_index = (ind >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
			ptr = reinterpret_cast<fmCirculArr<int*> *>(ptr->operator[](depth_index));
		}
		fmCirculArr<T>* vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);

		lastCArr = vptr;
		last_outerIndex = ind;

		// T *vptr = ptr;
		int inindex = ((int)(ind)) & (unsigned int)fragPercent;
		return vptr->operator[](inindex);
	}

	void clear()
	{
		array_siz = 0;
		last_outerIndex = 0;
		lastCArr = get_bottom_array(0);
		//NULLState();
	}

	T& last() const
	{
		return at(array_siz - 1);
	}

	//need test
	void release()
	{
		if (!mbDbg) return;
		for (ui32 k = 0; k < array_depth; ++k)
		{
			ui32 maxn = 1 << (fragment_siz_pow2 * array_depth - k);
			for (ui32 n = 0; n < maxn; ++n)
			{
				ui32 seek = n << (fragment_siz_pow2 * (k + 1));
				fmCirculArr<int*>* ptr = ptrArray;
				for (int i = 0; i < array_depth - k; ++i)
				{
					ptr = (fmCirculArr<int*> *)ptr->operator[](
						(int)((seek >> (fragment_siz_pow2* (array_depth - i))))& ((1 << (fragment_siz_pow2 + 1)) - 1));
				}

				if (ptr == nullptr)
				{
					break;
				}

				if (k == 0)
				{
					// most bottom real array
					fmCirculArr<T>* vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);
					vptr->Release();
					fm->_Delete(reinterpret_cast<byte8*>(vptr), sizeof(fmCirculArr<T>));
					// delete vptr;
				}
				else
				{
					// not bottom ptr array
					fmCirculArr<int*>* vptr = reinterpret_cast<fmCirculArr<int*> *>(ptr);
					vptr->Release();
					fm->_Delete(reinterpret_cast<byte8*>(vptr), sizeof(fmCirculArr<int*>));
				}
			}
		}

		ptrArray->Release();
		fm->_Delete((byte8*)ptrArray, sizeof(fmCirculArr<int*>));
		// delete[]ptrArray;
	}
};

class BitArray
{
public:
	byte8* Arr = nullptr;
	ui32 bit_arr_size = 0;	// saved bit count.
	ui32 byte_arr_size = 0;	// saved byte count.
	ui32 up = 0;
	short fmlayer = -1;
	bool isdebug = false;


	BitArray() : bit_arr_size(0), byte_arr_size(0), Arr(nullptr), up(0), fmlayer(-1), isdebug(false)
	{

	}

	/*
	BitArray(size_t bitsize):
		bit_arr_size(bitsize), byte_arr_size((bitsize / 8) + 1), up(0)
	{
		Arr = fm->_New(byte_arr_size);
	}
	*/

	virtual ~BitArray()
	{
		if (isdebug) {
			fm->_Delete(Arr, byte_arr_size);
			Arr = nullptr;
		}
	}

	void NULLState() {

	}

	void Init(ui32 siz, bool isdbg, int pfmlayer = -1) {
		bit_arr_size = siz;
		byte_arr_size = (bit_arr_size >> 3) + 1;
		isdebug = isdbg;
		fmlayer = pfmlayer;
		Arr = (byte8*)fm->_New(byte_arr_size, isdebug, fmlayer);
		if (fmlayer < 0) {
			fmlayer = fm->tempStack[threadID].up - 1;
		}
	}

	string get_bit_char()
	{
		string str;
		int byteup = (up / 8) + 1;
		for (int i = 0; i < byteup; ++i)
		{
			for (int lo = 0; lo < 8; ++lo)
			{
				if (up <= i * 8 + lo)
					break;
				int n = _GetByte(Arr[i], lo);
				if (n == 0)
				{
					str.push_back('0');
				}
				else if (n == 1)
				{
					str.push_back('1');
				}
			}
		}
		return str;
	}

	inline byte8 SetByte8(byte8 dat, int loc, bool is1)
	{
		return (is1) ? dat | (1 << (loc - 1)) : dat & ~(1 << (loc - 1));
	}

	void addbit(bool bit)
	{
		if (up + 1 <= bit_arr_size)
		{
			++up;
			int i = up / 8;
			int loc = up % 8;
			SetByte8(Arr[i], loc, bit);
		}
	}

	void SetUp(int n)
	{
		up = n;
		if (up <= bit_arr_size)
		{
			up = bit_arr_size;
		}
	}

	void setbit(int index, bool bit)
	{
		if (0 <= index && index < up)
		{
			int i = index / 8;
			int loc = index % 8;
			Arr[i] = SetByte8(Arr[i], loc, bit);
		}
	}

	bool getbit(int index)
	{
		if (0 <= index && index < up)
		{
			int i = index / 8;
			int loc = index % 8;
			return _GetByte(Arr[i], loc);
		}
	}
};

#endif
