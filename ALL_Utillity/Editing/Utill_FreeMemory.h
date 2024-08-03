#ifndef H_UTILL_FREEMEMORY
#define H_UTILL_FREEMEMORY
#include <math.h>
#include <thread>
#include <iostream>
#include <fstream>
#include "arr_expend.h"
#include <map>
using namespace std;
typedef unsigned char byte8;

typedef unsigned char ui8;
typedef unsigned short ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;
typedef unsigned int vui128 __attribute__((vector_size(16)));
typedef unsigned int vui256 __attribute__((vector_size(32)));
typedef char si8;
typedef short si16;
typedef int si32;
typedef long long si64;
typedef int vsi128 __attribute__((vector_size(16)));
typedef int vsi256 __attribute__((vector_size(32)));

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

namespace freemem
{
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
	void Init_VPTR_x86(void *obj)
	{
		T go = T();
		__int32_t vp = *(__int32_t *)&go;
		*((__int32_t *)obj) = vp;
	}

	template <typename T>
	void Init_VPTR_x64(void *obj)
	{
		T go;
		__int64_t vp = *(__int64_t *)&go;
		*((__int64_t *)obj) = vp;
	}

	class FM_Model
	{
	public:
		FM_Model()
		{
		}
		virtual ~FM_Model()
		{
		}

		virtual byte8 *_New(unsigned int byteSiz)
		{
			return nullptr;
		}

		virtual void ClearAll()
		{
			return;
		}

		virtual bool _Delete(byte8 *variable, unsigned int size)
		{
			return false;
		}

		// �ش� �ּҿ� �޸𸮰� �Ҵ�Ǿ�����.
		virtual bool bAlloc(byte8 *variable, unsigned int size)
		{
			return true;
		}

		virtual bool canInclude(byte8 *var, int size)
		{
			return true;
		}
	};

	inline void CheckRemainMemorySize()
	{
		byte8 end = 10;
		byte8 *start = new byte8();
		unsigned int RemainMemSiz = (unsigned int)(start - &end);
		cout << "RemainMemSiz : " << RemainMemSiz << " byte \t(" << (float)RemainMemSiz / 1000.0f << " KB \t(" << (float)RemainMemSiz / 1000000.0f << " MB \t(" << (float)RemainMemSiz / 1000000000.0f << " GB ) ) )" << endl;
		delete start;
	}

	// siz : 4096 byte
	struct SmallPage
	{
		byte8 Data[SMALL_PAGE_SIZE] = {};
	};

	// siz : 16 + 24 + 96 + extra80byte = 256 byte
	class FmHeapPageManager
	{
	public:
		vecarr<SmallPage *> pages;

		unsigned short pageImortal_up[12] = {};
		static constexpr unsigned short pageImortal_capacity[12] = {4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};
		void *lastPageImortal[12] = {};
		byte8 extra[80] = {};
		// 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 ...

		FmHeapPageManager() {}
		~FmHeapPageManager() {}

		void Init()
		{
			pages.NULLState();
			pages.Init(8);
		}

		void *Allocate()
		{
			constexpr vui256 const_zero_vui256 = {0, 0, 0, 0, 0, 0, 0, 0};

			void *sp = malloc(SMALL_PAGE_SIZE);

			// init to 0
			for (int i = 0; i < SMALL_PAGE_SIZE; i += 64)
			{
				*reinterpret_cast<vui256 *>(sp + i) = const_zero_vui256;
				*reinterpret_cast<vui256 *>(sp + i + 16) = const_zero_vui256;
				*reinterpret_cast<vui256 *>(sp + i + 32) = const_zero_vui256;
				*reinterpret_cast<vui256 *>(sp + i + 48) = const_zero_vui256;
			}

			pages.push_back(reinterpret_cast<SmallPage *>(sp));

			return sp;
		}

		void Release()
		{
			for (int i = 0; i < pages.size(); ++i)
			{
				free(pages.at(i));
			}
			pages.release();
		}

		void *Allocate_ImortalMemory(unsigned int size)
		{
			unsigned int index = log2(size - 1) + 1;

			if (pageImortal_up[index] == pageImortal_capacity[index])
			{
				lastPageImortal[index] = (void *)Allocate();
				pageImortal_up[index] = 0;
			}

			unsigned int up = pageImortal_up[index];
			pageImortal_up[index] += 1;
			return lastPageImortal[index] + (up << index);
		}
	};

	FmHeapPageManager globalHeapPage;

	// siz : 16 byte
	struct PageMeta
	{
		void *PageData;
		unsigned int Fup;
		unsigned int extra_Flags; // 32 flags

		void Allocate()
		{
			PageData = globalHeapPage.Allocate();
			Fup = 0;
			extra_Flags = 0;
		}

		void *_New(unsigned int size)
		{
			void *ptr = PageData + Fup;
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

	typedef struct VP
	{
		char mod = 0;		// mod 0:value 1:ptr
		int *ptr = nullptr; // arrgraph ptr or T ptr
	};

	template <typename T, typename V>
	struct range
	{
		T end;
		V value;
	};

	template <typename T, typename V>
	class ArrGraph
	{
	public:
		vecarr<range<T, V>> *ranges;
		T minx = 0;
		T maxx = 0;
		T margin = 0;
		bool islocal = false;
		vecarr<VP> graph;

		ArrGraph()
		{
		}
		virtual ~ArrGraph()
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
			ranges = (vecarr<range<T, V>> *)globalHeapPage.Allocate_ImortalMemory(sizeof(vecarr<range<T, V>>));
			ranges->NULLState();
			ranges->Init(2);
			islocal = false;
		}

		range<T, V> Range(T end, V value)
		{
			range<T, V> r;
			r.end = end;
			r.value = value;
			return r;
		}

		void push_range(range<T, V> r)
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
				f = floor(f) + 1;
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
					if (end > maxx)
						end = maxx;
					T rstart = minx;
					for (int k = 0; k < ranges->up; ++k)
					{
						T rend = ranges->at(k).end;
						if (rstart <= start && end <= rend)
						{
							// num
							graph[i].mod = 0;
							graph[i].ptr = reinterpret_cast<int *>(&ranges->at(k).value);
							break;
						}
						else if (start <= rend && rend <= end)
						{
							// graph
							ArrGraph<T, V> *newgraph = (ArrGraph<T, V> *)globalHeapPage.Allocate_ImortalMemory(sizeof(ArrGraph<T, V>));
							newgraph->Init(start, end);
							newgraph->push_range(ranges->at(k));
							range<T, V> *r = &ranges->at(k + 1);
							while (r->end < end)
							{
								newgraph->push_range(*r);
								++k;
								if (k >= ranges->size())
								{
									break;
								}
								r = &ranges->at(k + 1);
							}
							// input last range
							range<T, V> lastr;
							lastr = *r;
							lastr.end = newgraph->maxx;
							newgraph->push_range(lastr);
							newgraph->Compile();
							graph[i].ptr = reinterpret_cast<int *>(newgraph);
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
				T end = maxx - 1;
				if (maxx - center > center - start)
				{
					minx = 2 * center + 1 - end;
				}
				else
				{
					maxx = 2 * center + 1 - start;
				}
				margin = (maxx - minx) / ranges->size();
				VP vp0;
				vp0.mod = 0;
				vp0.ptr = reinterpret_cast<int *>(&ranges->at(0).value);
				graph.push_back(vp0);
				vp0.ptr = reinterpret_cast<int *>(&ranges->at(1).value);
				graph.push_back(vp0);
			}
		}

		T fx(T x)
		{
			static constexpr void *jumpptr[2] = {&&ISVALUE, &&ISGRAPH};
			ArrGraph<T, V> *ag = this;
			vecarr<VP> *g = &graph;
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
			ag = reinterpret_cast<ArrGraph<T, V> *>(vp.ptr);
			g = &ag->graph;
			goto GET_START;

		ISVALUE:
			return *reinterpret_cast<V *>(vp.ptr);
		}

		void print_state()
		{
			cout << "arrgraph minx : " << minx << "\t maxx : " << maxx << endl;
			cout << "capacity : " << graph.size() << "\t margin : " << margin << endl;
			for (int i = 0; i < graph.size(); ++i)
			{
				if (graph[i].mod == 0)
				{
					cout << "index : " << i << "] = " << *reinterpret_cast<V *>(graph[i].ptr) << endl;
				}
				else
				{
					cout << "index : " << i << "] = ptr : " << endl;
					reinterpret_cast<ArrGraph<T, V> *>(graph[i].ptr)->print_state();
					cout << endl;
				}
			}
		}
	};

	int getcost(int n, int size)
	{
		int k = (n % size == 0) ? 0 : 1;
		return (8 * size + 1) * (n / size + k);
	}

	int minarr(int siz, int *arr, int *indexout)
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

	class FM_Model0 : FM_Model
	{
	public:
		unsigned int siz = 0;
		byte8 *Data = nullptr;
		unsigned int Fup = 0;
		bool isHeap = false;

		  FM_Model0()
		{
		}
		FM_Model0(byte8 * data, unsigned int Size)
		{
			Data = data;
			siz = Size;
		}
		virtual ~ FM_Model0()
		{
			if (isHeap && Data != nullptr)
			{
				delete[]Data;
			}
		}

		void SetHeapData(byte8 * data, unsigned int Size)
		{
			isHeap = true;
			Data = data;
			siz = Size;
		}

		byte8 *_New(unsigned int byteSiz)
		{
			if (Fup + byteSiz <= siz)
			{
				unsigned int fup = Fup;
				Fup += byteSiz;
				return &Data[fup];
			}
			else
			{
				ClearAll();
				return _New(byteSiz);
			}
		}

		bool _Delete(byte8 * variable, unsigned int size)
		{
			return false;
		}

		void ClearAll()
		{
			Fup = 0;
		}

		void PrintState()
		{
			cout << "FreeMemory Model 0 State -----------------" << endl;
			CheckRemainMemorySize();
			cout << "MAX byte : \t" << siz << " byte \t(" << (float)siz /
				1024.0f << " KB \t(" << (float)siz / powf(1024.0f,
														  2) << " MB \t(" << (float)siz /
				powf(1024.0f, 3) << " GB ) ) )" << endl;
			cout << "Alloc Number : \t" << Fup << " byte \t(" << (float)Fup /
				1024.0f << " KB \t(" << (float)Fup / powf(1024.0f,
														  2) << " MB \t(" << (float)Fup /
				powf(1024.0f, 3) << " GB ) ) )" << endl;
			cout << "FreeMemory Model 0 State END -----------------" << endl;
		}

		virtual bool canInclude(byte8 * var, int size)
		{
			if (Data <= var && var + size < &Data[siz - 1])
				return true;
			return false;
		}
	};

	constexpr byte8 locdata[8] = {
		1,						// 0b00000001
		2,
		4,
		8,
		16,
		32,
		64,
		128
	};

	constexpr byte8 invlocdata[8] = {
		254,
		253,
		251,
		247,
		239,
		223,
		191,
		127
	};

	constexpr byte8 flocdata[9] = {
		255,					// 0 b11111111
		254,					// 0 b11111110
		252,					// 0 b11111100,
		248,					// 0 b11111000,
		240,					// 0 b11110000,
		224,					// 0 b11100000,
		192,					// 0 b11000000,
		128,					// 0 b10000000,
		0						// 0b00000000
	};

	constexpr byte8 invflocdata[9] = {
		0,						// 0b00000000,
		1,						// 0 b00000001,
		3,						// 0 b00000011,
		7,						// 0 b00000111,
		15,						// 0 b00001111,
		31,						// 0 b00011111,
		63,						// 0 b00111111,
		127,					// 0 b01111111,
		255						// 0 b11111111
	};

	inline bool GetByte8(byte8 dat, int loc)
	{
		return dat & locdata[loc];
	}

	inline byte8 SetByte8(byte8 dat, int loc, bool is1)
	{
		return (is1) ? dat | locdata[loc] : dat & invlocdata[loc];
	}

	class FM_Model1:FM_Model
	{
	  public:
		// static ofstream *fmout;
		// static int updateid;
		int id = 0;
		bool isHeap = false;	// true�� heap, false�� stack
		byte8 *DataPtr = nullptr;
		unsigned int realDataSiz = 0;
		unsigned int sumDataSiz = 0;

		unsigned int Fup = 0;
		int isvalidNum = 0;

		int dbg_bitsize = 8;
		int dbg_bytesize = 1;

		  FM_Model1()
		{

		}

		FM_Model1(unsigned int RDS, byte8 * dataptr)
		{
			DataPtr = dataptr;
			realDataSiz = RDS;
			sumDataSiz = (dbg_bitsize + 1) * realDataSiz / dbg_bitsize;
		}

		virtual ~ FM_Model1()
		{
			if (isHeap && DataPtr != nullptr)
			{
				delete[]DataPtr;
			}
		}

		inline int getfitsize(int siz)
		{
			return (siz % dbg_bytesize) ? dbg_bytesize * (1 + (siz / dbg_bytesize)) : siz;
		}

		inline int getallocbit(int siz)
		{
			return siz / dbg_bytesize;
		}

		void SetHeapData(unsigned int RDS, int dbgs)
		{
			isHeap = true;
			realDataSiz = RDS;
			dbg_bitsize = dbgs;
			dbg_bytesize = dbgs / 8;
			sumDataSiz = (dbg_bitsize + 1) * realDataSiz / (dbg_bitsize);
			DataPtr = new byte8[sumDataSiz];
		}

		bool isValid(unsigned int address)
		{
			int bigloc = address / dbg_bitsize;
			int smallLoc = (address / (dbg_bytesize)) % 8;
			if (_GetByte(DataPtr[realDataSiz + bigloc], smallLoc))
			{
				return false;
			}
			else
				return true;
		}

		bool canAlloc(int start, int end)
		{
			int start_bit = getallocbit(start);
			int end_bit = getallocbit(end);
			if (end_bit - start_bit < 8)
			{
				for (int i = start_bit; i <= end_bit; ++i)
				{
					int bad = i / 8;
					int sad = i % 8;
					if (GetByte8(DataPtr[realDataSiz + bad], sad))
					{
						return false;
					}
				}
			}
			else
			{
				int sbad = start_bit / 8;
				int ssad = start_bit % 8;
				int ebad = end_bit / 8;
				int esad = end_bit % 8;
				byte8 a = DataPtr[realDataSiz + sbad];
				a &= flocdata[ssad];
				if (a)
				{
					return false;
				}
				for (int i = sbad + 1; i < ebad; ++i)
				{
					if (DataPtr[realDataSiz + i] != 0)
					{
						return false;
					}
				}
				a = DataPtr[realDataSiz + ebad];
				a &= invflocdata[esad + 1];
				if (a)
				{
					return false;
				}
			}

			return true;
		}

		bool isAlloced(int start, int end)
		{
			int start_bit = getallocbit(start);
			int end_bit = getallocbit(end);
			if (end_bit - start_bit < 8)
			{
				for (int i = start_bit; i <= end_bit; ++i)
				{
					int bad = i / 8;
					int sad = i % 8;
					bool b = GetByte8(DataPtr[realDataSiz + bad], sad);
					if (b == false)
					{
						return false;
					}
				}
			}
			else
			{
				int sbad = start_bit / 8;
				int ssad = start_bit % 8;
				int ebad = end_bit / 8;
				int esad = end_bit % 8;
				byte8 a = DataPtr[realDataSiz + sbad];
				a |= invflocdata[ssad];
				if (a != 255)
				{
					return false;
				}
				for (int i = sbad + 1; i < ebad; ++i)
				{
					if (DataPtr[realDataSiz + i] != 255)
					{
						return false;
					}
				}
				a = DataPtr[realDataSiz + ebad];
				a |= flocdata[esad + 1];
				if (a != 255)
				{
					return false;
				}
			}

			return true;
		}

		void SetAllocs(int start, int end, bool is1)
		{
			int start_bit = getallocbit(start);
			int end_bit = getallocbit(end);

			if (end_bit - start_bit < 8)
			{
				for (int i = start_bit; i <= end_bit; ++i)
				{
					int bad = i / 8;
					int sad = i % 8;
					DataPtr[realDataSiz + bad] = SetByte8(DataPtr[realDataSiz + bad], sad, is1);
				}
			}
			else
			{
				int sbad = start_bit / 8;
				int ssad = start_bit % 8;
				int ebad = end_bit / 8;
				int esad = end_bit % 8;

				if (is1)
				{
					DataPtr[realDataSiz + sbad] |= flocdata[ssad];
					for (int i = sbad + 1; i < ebad; ++i)
					{
						DataPtr[realDataSiz + i] = 255;
					}
					DataPtr[realDataSiz + ebad] |= invflocdata[esad + 1];
				}
				else
				{
					if (sbad <= 49 && 49 <= ebad)
					{
						cout << "dbg" << endl;
					}
					DataPtr[realDataSiz + sbad] &= invflocdata[ssad];
					for (int i = sbad + 1; i < ebad; ++i)
					{
						DataPtr[realDataSiz + i] = 0;
					}
					DataPtr[realDataSiz + ebad] &= flocdata[esad + 1];
				}
			}
		}

		void dbg_lifecheck()
		{
			int count = 0;
			cout << (int *)&DataPtr[0] << " : \t";
			for (int i = 0; i < realDataSiz; i += dbg_bytesize)
			{
				switch (isValid(i))
				{
				case true:
					cout << '_';
					break;
				case false:
					cout << '1';
					break;
				}
				if (count >= 32)
				{
					cout << endl;
					int ad = i;
					cout << (int *)&DataPtr[ad] << " : \t";
					count = 0;
				}

				++count;
			}
			cout << endl;
		}

		void Set(unsigned int address, bool enable)
		{
			int bigloc = address / dbg_bitsize;
			int smallLoc = (address / (dbg_bytesize)) % 8;
			DataPtr[realDataSiz + bigloc] =
				SetByte8(DataPtr[realDataSiz + bigloc], smallLoc, enable);
		}

		byte8 *_New(unsigned int size)
		{
			return _savenew(size);
		}

		byte8 *_fastnew(unsigned int size)
		{
			int fups = Fup + size - 1;
			if (fups < realDataSiz)
			{
				int f = Fup;
				int a = getfitsize(size);
				SetAllocs(Fup, Fup + a - 1, true);
				Fup += a;
				return &DataPtr[f];
			}
			return nullptr;
		}

		byte8 *_savenew(unsigned int size)
		{
			int fups = Fup + size - 1;
			if (fups < realDataSiz)
			{
				int f = Fup;
				int a = getfitsize(size);
				SetAllocs(Fup, Fup + a - 1, true);
				Fup += a;
				return &DataPtr[f];
			}
			else
			{
				int end = sumDataSiz - size / dbg_bytesize;
				int start = realDataSiz;
				int reqstack = 1 + size / dbg_bitsize;
				int rstack = 0;
				for (int ad = start; ad <= end; ad += dbg_bytesize)
				{
					if (DataPtr[ad] != 255)
					{
						++rstack;
						if (rstack >= reqstack)
						{
							for (int k = 0; k < 9; ++k)
							{
								int add =
									dbg_bitsize * (ad - reqstack - realDataSiz + 1) +
									k * dbg_bytesize;
								int addc = getfitsize(size);
								int adds = add + addc - 1;
								if (canAlloc(add, adds))
								{
									// add, adds range hole update
									int n = add;
									while (n - add < addc)
									{
										n += DataPtr[n];
										if (DataPtr[n] == 0)
										{
											n += 1;
										}
									}

									int len = n - adds;
									n -= 1;
									int nn = n - adds;
									switch (len)
									{
									case 2:
										DataPtr[n] = 0;
										break;
									case 1:
										break;
									default:
										{
											if (isValid(adds + 1))
											{
												DataPtr[adds + 1] = nn;
											}
											if (isValid(n))
											{
												DataPtr[n] = nn;
											}
										}
										break;
									}
									SetAllocs(add, adds, true);
									return &DataPtr[add];
								}
							}
						}
					}
					else
					{
						rstack = 0;
					}
				}
			}
			return nullptr;
		}

		bool _Delete(byte8 * variable, unsigned int size)
		{
			int address = variable - DataPtr;
			int ads = address + getfitsize(size);

			if (ads - 1 > realDataSiz || address < 0)
			{
				return false;
			}

			if (isAlloced(address, ads - 1))
			{
				SetAllocs(address, ads - 1, false);

				if (ads >= Fup)
				{
					// Fup = address;
					int n = address - 1;
					while (0 <= n && isValid(n))
					{
						n -= DataPtr[n];
						if (DataPtr[n] == 0)
						{
							n -= 1;
						}
					}
					if (n < 0)
					{
						Fup = 0;
					}
					else
					{
						Fup = n + 1;
						// This while code should not be executed within
						// normal operation. However, it is a precautionary
						// code for safety in exceptional, error-prone, or
						// unintended situations.
						while (isValid(Fup) == false)
						{
							++Fup;
						}
					}
					return (bool) 2;
				}
				else
				{
					int ss = ads - address;
					if (1 < ss < 256)
					{
						*(variable + (ss - 1)) = ss;
						*(variable) = ss;
					}
					else if (ss == 1)
					{
						*variable = 0;
					}
				}
				return true;
			}

			return false;
		}

		int get_fupm()
		{
			return realDataSiz - Fup;
		}

		// �ش� �ּҿ� �޸𸮰� �Ҵ�Ǿ�����.
		bool bAlloc(byte8 * variable, unsigned int size)
		{
			unsigned int address = variable - DataPtr;
			return isAlloced(address, address + size - 1);
		}

		void DebugAddresses()
		{
			int count = 0;
			isvalidNum = 0;
			for (int i = 0; i < (int)realDataSiz; ++i)
			{
				if (isValid(i) == false)
				{
					count += 1;
				}
			}

			cout << "Non Release Free Memory(no matter) : " << count << endl;

			ofstream out;
			out.open("DebugFile.txt");
			for (int i = 0; i < (int)Fup; ++i)
			{
				if (isValid(i))
				{
					out << '0';
					isvalidNum += 1;
				}
				else
				{
					out << '1';
				}
			}
			out.close();
		}

		void ClearAll()
		{
			SetAllocs(0, realDataSiz, false);
			Fup = 0;
		}

		virtual bool canInclude(byte8 * var, int size)
		{
			if (DataPtr <= var && var + size - 1 < &DataPtr[realDataSiz])
				return true;
			return false;
		}
	};

	typedef byte8 *AllocPtr;

	class FM_Model2:FM_Model
	{
	  public:
		bool isHeap = false;
		byte8 *DataPtr = nullptr;
		int Fup = 0;

		unsigned int DataPoolSize = 0;
		unsigned int SizeMemorySize = 1;
		unsigned int MinimumDataSize = 40;	// SizeMemorySize * 8 * 5

		AllocPtr *AllocArr = nullptr;
		unsigned int AAsize = 0;	// DataPoolSize / MinimumDataSize;
		int aaup = 0;

		FM_Model2()
		{

		}

		virtual ~ FM_Model2()
		{

		}

		void ArrangeAllocArr()
		{
			uintptr_t ptraa = reinterpret_cast < uintptr_t > (AllocArr[aaup - 1]);
			while (aaup > 0 && ptraa == ptr_max)
			{
				aaup -= 1;
				ptraa = reinterpret_cast < uintptr_t > (AllocArr[aaup - 1]);
			}

			int i, j;
			uintptr_t key;
			for (i = 1; i < aaup; i++)
			{
				key = reinterpret_cast < uintptr_t > (AllocArr[i]);
				for (j = i - 1; j >= 0 && reinterpret_cast < uintptr_t > (AllocArr[j]) > key; j--)
				{
					AllocArr[j + 1] = AllocArr[j];
				}

				AllocArr[j + 1] = (byte8 *) key;
			}
		}

		virtual byte8 *_New(unsigned int byteSiz)
		{
			ArrangeAllocArr();
			if (Fup < (int)DataPoolSize)
			{
				if (byteSiz >= MinimumDataSize)
				{
					if (Fup + byteSiz < DataPoolSize)
					{
						if (SizeMemorySize == 1)
						{
							DataPtr[Fup] = (byte8) byteSiz;
						}
						else if (SizeMemorySize == 2)
						{
							*((unsigned short *)&DataPtr[Fup]) = (unsigned short)byteSiz;
						}
						Fup += SizeMemorySize;
						int ptr = Fup;
						Fup += byteSiz;
						if (aaup + 1 < (int)AAsize)
						{
							AllocArr[aaup] = &DataPtr[ptr];
							aaup += 1;
						}
						return &DataPtr[ptr];
					}
				}
			}
			else
			{
				// fup �� �� á����
				
				ptr_type start = (ptr_type)DataPtr;
				ptr_type end = (ptr_type) DataPtr;
				for (int i = 0; i < (int)AAsize; ++i)
				{
					start = end;
					start = start + ((byte8) (*(byte8 *) (start - 1)));
					end = (ptr_type) AllocArr[i];
					if ((int)(end - start) > (int)byteSiz + 1)
					{
						int index = start + 1 - (ptr_type) DataPtr;
						if (SizeMemorySize == 1)
						{
							DataPtr[index] = (byte8) byteSiz;
						}
						else if (SizeMemorySize == 2)
						{
							*((unsigned short *)&DataPtr[index]) = (unsigned short)byteSiz;
						}
						index += SizeMemorySize;
						int ptr = index;
						index += byteSiz;
						if (aaup + 1 < (int)AAsize)
						{
							AllocArr[aaup] = &DataPtr[ptr];
							aaup += 1;
						}
						return &DataPtr[ptr];
					}
				}
			}
			return nullptr;
		}

		virtual void ClearAll()
		{
			aaup = 0;
			Fup = 0;
			return;
		}

		virtual bool _Delete(byte8 * variable, unsigned int size)
		{
			// ��������
			ArrangeAllocArr();

			bool deleted = false;
			int start = 0;
			int end = aaup - 1;
			int mid = (start + end) / 2;
			while (end - start > 10)
			{
				if (AllocArr[mid] > variable)
				{
					end = mid;
					mid = (start + end) / 2;
				}
				else if (AllocArr[mid] < variable)
				{
					start = mid;
					mid = (start + end) / 2;
				}
				else
				{
					AllocArr[mid] = (byte8 *) ptr_max;
					deleted = true;
				}
			}

			if (deleted)
			{
				for (int i = start; i < end + 1; ++i)
				{
					if (AllocArr[i] == variable)
					{
						AllocArr[i] = (byte8 *) ptr_max;
						deleted = true;
					}
				}
			}

			if (deleted)
				return true;
			else
				return false;
		}

		// �ش� �ּҿ� �޸𸮰� �Ҵ�Ǿ�����.
		virtual bool bAlloc(byte8 * variable, unsigned int size)
		{
			ArrangeAllocArr();

			bool alloc = false;
			int start = 0;
			int end = aaup - 1;
			int mid = (start + end) / 2;
			while (end - start > 10)
			{
				if (AllocArr[mid] > variable)
				{
					end = mid;
					mid = (start + end) / 2;
				}
				else if (AllocArr[mid] < variable)
				{
					start = mid;
					mid = (start + end) / 2;
				}
				else
				{
					alloc = true;
				}
			}

			if (alloc)
			{
				for (int i = start; i < end + 1; ++i)
				{
					if (AllocArr[i] == variable)
					{
						alloc = true;
					}
				}
			}

			return alloc;
		}

		void SetHeapData(byte8 * dataptr, unsigned int dataSize, unsigned int sizedatasize)
		{
			isHeap = true;
			DataPtr = dataptr;
			DataPoolSize = dataSize;
			SizeMemorySize = sizedatasize;
			MinimumDataSize = (SizeMemorySize + ptr_size) * 8;
			AAsize = (DataPoolSize / MinimumDataSize) + 1;
			AllocArr = new AllocPtr[AAsize];
		}

		int GetSizeOfVariable(byte8 * variable)
		{
			if (bAlloc(variable, 0))
			{
				byte8 b = *(variable - SizeMemorySize);
				int size = b;
				return size;
			}

			return -1;
		}

		virtual bool canInclude(byte8 * var, int size)
		{
			if (size > MinimumDataSize
				&& (DataPtr <= var && var + size < &DataPtr[DataPoolSize - 1]))
				return true;
			return false;
		}
	};

	struct large_alloc
	{
		void *ptr;
		int size = 0;
	};

	// siz : 32 byte
	struct FmTempLayer{
		vecarr < large_alloc > large;
		vecarr < PageMeta * > tempFM;

		void Init(){
			large.NULLState();
			large.Init(8);
			tempFM.NULLState();
			tempFM.Init(8);
		}

		void Release(){
			for(int i=0;i<tempFM.size();++i){
				tempFM[i]->ClearAll();
			}
			tempFM.release();

			for(int i=0;i<large.size();++i){
				free(large[i].ptr);
			}
			large.release();
		}

		void ClearAll(){
			for(int i=0;i<tempFM.size();++i){
				tempFM[i]->Fup = 0;
			}

			for(int i=0;i<large.size();++i){
				free(large[i].ptr);
			}

			tempFM.up = 0;
			large.up = 0;
		}

		void* _New(unsigned int size){
			if (size <= SMALL_PAGE_SIZE)
			{
				unsigned int tsize = tempFM.size();
				for (int i = 0; i < tsize; ++i)
				{
					//watch("i", i);
					int remain = SMALL_PAGE_SIZE - tempFM[i]->Fup;
					if (remain >= size)
					{
						return tempFM[i]->_New(size);
					}
				}
				
				PageMeta* pm = (PageMeta*)globalHeapPage.Allocate_ImortalMemory(sizeof(PageMeta));
				tempFM.push_back(pm);
				return tempFM.last()->_New(size);
			}
			else
			{
				large_alloc la;
				la.ptr = malloc(size);
				la.size = size;
				large.push_back(la);
				return reinterpret_cast < byte8 * >(la.ptr);
			}
		}
	};

	thread_local unsigned int threadID;
	//siz : 16byte
	struct TempStack
	{
	  public:
		vecarr<FmTempLayer*> layer;
		//vecarr < vecarr < large_alloc > *>large;
		//vecarr < vecarr < FM_Model0 * >*>tempFM;

		void init()
		{
			layer.NULLState();
			layer.Init(8);
			for(int i=0;i<layer.maxsize;++i){
				layer[i] == nullptr;
			}
			//watch("tempFM init", 0);
		}

		void release()
		{
			for(int i=0;i<layer.size();++i){
				layer[i]->Release();
			}
			layer.release();
		}

		void *_New(unsigned int size, int fmlayer = -1)
		{
			int sel_layer = fmlayer;
			if(sel_layer < 0) sel_layer = layer.size()-1;
			return layer[sel_layer]->_New(size);
		}

		void PushLayer()
		{
			if(layer.up == layer.maxsize){
				FmTempLayer* fmtl = (FmTempLayer*)globalHeapPage.Allocate_ImortalMemory(sizeof(FmTempLayer));
				layer.push_back(fmtl);
				for(int i=layer.up;i<layer.maxsize;++i){
					layer[i] == nullptr;
			    }
				layer.last()->Init();
			}
			else{
				layer.up += 1;
			}
		}

		void PopLayer()
		{
			layer.last()->ClearAll();
			layer.last()->tempFM.up = -1;
			layer.up -= 1;
		}
	};

	// The storage method is classified by the size of the data.

	//siz : 8 + 8 + 4 + 4 + 4 + 4 = 32byte
	struct FmFlagPage{
		PageMeta* page;
		void* flagData;
		unsigned int DataCapacity = 0;
		unsigned int BytePerLifeFlag = 1;
		unsigned int FlagSiz = 0;
		unsigned int Fup = 0;

		void Allocate(unsigned int _DataCapacity, unsigned int _BytePerLifeFlag)
		{
			DataCapacity = _DataCapacity;
			BytePerLifeFlag = _BytePerLifeFlag;
			FlagSiz = DataCapacity / 8*BytePerLifeFlag;
			page = (PageMeta*)globalHeapPage.Allocate();
			flagData = (void*)globalHeapPage.Allocate_ImortalMemory(FlagSiz);
			Fup = 0;
		}

		void *_New(unsigned int size)
		{
			void *ptr = page->PageData + Fup;
			Fup += size;
			if (Fup >= SMALL_PAGE_SIZE)
			{
				return nullptr;
			}
			return ptr;
		}

		bool _Delete(void* ptr, unsigned int size){
			
		}

		void ClearAll()
		{
			Fup = 0;

		}
	}

	struct FmFlagLayer{

	}

	#define MAX_THREAD_COUNT_DIV8 4
	//max thread count is 32.
	class FM_System0
	{
	  public:
		byte8 threadID_allocater[MAX_THREAD_COUNT_DIV8] = {};
		// 1 - unallocated | 0 - allocated

		//static constexpr int midminsize = 72;	// x64
		//int fm1_sizetable[128] = { };
		ArrGraph<unsigned int, unsigned int> * fm1_sizetable;

		//vecarr < std::thread::id > thread_idarr;
		//vecarr < PageMeta * > HeapPageMetaArr;
		vecarr < TempStack * > tempStack;
		vecarr < vecarr < FM_Model1 * >*>SmallSize_HeapDebugFM;
		// 1 ~ midminsize byte
		vecarr < FM_Model2 * >MidiumSize_HeapDebugFM;
		// 40 ~ 255 byte
		vecarr < FM_Model2 * >BigSize_HeapDebugFM;
		// 256 ~ 65535 byte

		FM_System0()
		{

		}

		virtual ~ FM_System0()
		{

		}

		ArrGraph<unsigned int, unsigned int> *CreateFm1SizeTable()
		{
			ArrGraph<unsigned int, unsigned int> *sizeGraph = (ArrGraph<unsigned int, unsigned int> *)globalHeapPage.Allocate_ImortalMemory(sizeof(ArrGraph<unsigned int, unsigned int>));
			sizeGraph->Init(1, 4096);
			int sizearr[13] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
			range<unsigned int, unsigned int> currange;
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

				if (currange.value == sizearr[mini])
				{
					continue;
				}
				else
				{
					currange.end = i - 1;
					sizeGraph->push_range(currange);
					currange.end = 0;
					currange.value = sizearr[mini];
				}
			}

			currange.end = 4096;
			sizeGraph->push_range(currange);
			sizeGraph->Compile();
			// sizeGraph->print_state();
			return sizeGraph;
		}

		void SetHeapData(uint32_t temp, uint32_t sshd, uint32_t mshd, uint32_t bshd)
		{
			for(int i=0;i<MAX_THREAD_COUNT_DIV8;++i){
				threadID_allocater[i] = 255;
			}
			
			tempStack.NULLState();
			tempStack.Init(MAX_THREAD_COUNT_DIV8*8);
			for (int i = 0; i < MAX_THREAD_COUNT_DIV8*8; ++i)
			{
				TempStack *ts = new TempStack();
				ts->init();
				tempStack.push_back(ts);
			}

			fm1_sizetable = CreateFm1SizeTable();

			SmallSize_HeapDebugFM.NULLState();
			SmallSize_HeapDebugFM.Init(2);
			for (int i = 0; i < 8; ++i)
			{
				FM_Model1 *sshdFM = new FM_Model1();
				int n = pow(2, i);
				sshdFM->SetHeapData(sshd_Size, 8 * n);
				vecarr < FM_Model1 * >*ssfm1 = new vecarr < FM_Model1 * >();
				ssfm1->NULLState();
				ssfm1->Init(2, false);
				ssfm1->push_back(sshdFM);
				SmallSize_HeapDebugFM.push_back(ssfm1);
			}

			MidiumSize_HeapDebugFM.NULLState();
			MidiumSize_HeapDebugFM.Init(2);
			FM_Model2 *mshdFM = new FM_Model2();
			mshdFM->SetHeapData(new byte8[mshd], mshd, 1);
			MidiumSize_HeapDebugFM.push_back(mshdFM);

			BigSize_HeapDebugFM.NULLState();
			BigSize_HeapDebugFM.Init(2);
			FM_Model2 *bshdFM = new FM_Model2();
			bshdFM->SetHeapData(new byte8[bshd], bshd, 2);
			BigSize_HeapDebugFM.push_back(bshdFM);
		}

		void allocate_thread_fmTempMem()
		{
			for(int i=0;i<MAX_THREAD_COUNT_DIV8;++i){
				if(threadID_allocater[i] != 0){
					for(int k=0;k<8;++k){
						if(threadID_allocater[i] & (1 << k)){
							threadID_allocater[i] |= (1 << k);
							threadID = i*8 + k;
							return;
						}
					}
				}
			}
		}

		void free_thread_fmTempMem(){
			unsigned int i = threadID / 8;
			unsigned int k = threadID % 8;
			threadID_allocater[i] &= ~(1<<k);
		}

		void dbg_fm1_lifecheck()
		{
			cout << "----------------fmsystem-----------------" << endl;
			for (int k = 0; k < 8; ++k)
			{
				cout << "\n fm" << this << endl;
				for (int i = 0; i < SmallSize_HeapDebugFM[k]->size(); ++i)
				{
					FM_Model1 *fm = SmallSize_HeapDebugFM[k]->at(i);
					cout << "\nFM1_" << i << "=" << fm << endl;
					cout << "FUP : " << fm->Fup << "/" << fm->realDataSiz << endl;
					fm->dbg_lifecheck();
				}
			}
		}

		byte8 *_fastnew(unsigned int byteSiz)
		{
			if (1 <= byteSiz && byteSiz <= midminsize - 1)
			{
				int index = fm1_sizetable[byteSiz];
				vecarr < FM_Model1 * >*fm1 = SmallSize_HeapDebugFM[index];
				for (int i = 0; i < (int)fm1->size(); ++i)
				{
					byte8 *ptr = fm1->at(i)->_fastnew(byteSiz);
					if (ptr != nullptr)
					{
						return ptr;
					}
				}

				FM_Model1 *sshdFM = new FM_Model1();
				sshdFM->SetHeapData(sshd_Size, 8 * pow(2, index));
				fm1->push_back(sshdFM);
				byte8 *ptr = sshdFM->_fastnew(byteSiz);
				return ptr;
			}
			else if (midminsize <= byteSiz && byteSiz <= 255)
			{
				for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i)
				{
					byte8 *ptr = MidiumSize_HeapDebugFM[i]->_New(byteSiz);
					if (ptr != nullptr)
					{
						return ptr;
					}
				}

				FM_Model2 *mshdFM = new FM_Model2();
				mshdFM->SetHeapData(new byte8[mshd_Size], mshd_Size, 1);
				MidiumSize_HeapDebugFM.push_back(mshdFM);
				byte8 *ptr = mshdFM->_New(byteSiz);
				return ptr;
			}
			else if (256 <= byteSiz && byteSiz <= 65535)
			{
				for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i)
				{
					byte8 *ptr = BigSize_HeapDebugFM[i]->_New(byteSiz);
					if (ptr != nullptr)
					{
						return ptr;
					}
				}

				FM_Model2 *bshdFM = new FM_Model2();
				bshdFM->SetHeapData(new byte8[bshd_Size], bshd_Size, 2);
				BigSize_HeapDebugFM.push_back(bshdFM);
				byte8 *ptr = BigSize_HeapDebugFM[BigSize_HeapDebugFM.size() - 1]->_New(byteSiz);
				return ptr;
			}
		}

		inline void _tempPushLayer()
		{
			tempStack[get_threadid(std::this_thread::get_id())]->PushLayer();
		}

		inline void _tempPopLayer()
		{
			tempStack[get_threadid(std::this_thread::get_id())]->PopLayer();
		}

		void* _tempNew(unsigned int byteSiz, int fmlayer = -1)
		{
			return tempStack[threadID]->_New(byteSiz, fmlayer);
		}

		void *_New(unsigned int byteSiz, bool isHeapDebug, int fmlayer = -1)
		{
			if (isHeapDebug == false)
			{
				return tempStack[threadID]->_New(byteSiz, fmlayer);
			}
			else
			{
				if (1 <= byteSiz && byteSiz <= midminsize - 1)
				{
					int index = fm1_sizetable[byteSiz];
					vecarr < FM_Model1 * >*fm1 = SmallSize_HeapDebugFM[index];
					for (int i = 0; i < (int)fm1->size(); ++i)
					{
						byte8 *ptr = fm1->at(i)->_New(byteSiz);
						if (ptr != nullptr)
						{
							return ptr;
						}
					}

					FM_Model1 *sshdFM = new FM_Model1();
					sshdFM->SetHeapData(sshd_Size, 8 * pow(2, index));
					fm1->push_back(sshdFM);
					byte8 *ptr = sshdFM->_New(byteSiz);
					return ptr;
				}
				else if (midminsize <= byteSiz && byteSiz <= 255)
				{
					for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i)
					{
						byte8 *ptr = MidiumSize_HeapDebugFM[i]->_New(byteSiz);
						if (ptr != nullptr)
						{
							return ptr;
						}
					}

					FM_Model2 *mshdFM = new FM_Model2();
					mshdFM->SetHeapData(new byte8[mshd_Size], mshd_Size, 1);
					MidiumSize_HeapDebugFM.push_back(mshdFM);
					byte8 *ptr = mshdFM->_New(byteSiz);
					return ptr;
				}
				else if (256 <= byteSiz && byteSiz <= 65535)
				{
					for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i)
					{
						byte8 *ptr = BigSize_HeapDebugFM[i]->_New(byteSiz);
						if (ptr != nullptr)
						{
							return ptr;
						}
					}

					FM_Model2 *bshdFM = new FM_Model2();
					bshdFM->SetHeapData(new byte8[bshd_Size], bshd_Size, 2);
					BigSize_HeapDebugFM.push_back(bshdFM);
					byte8 *ptr =
						BigSize_HeapDebugFM[BigSize_HeapDebugFM.size() - 1]->_New(byteSiz);
					return ptr;
				}
			}
			return nullptr;
		}

		bool _Delete(byte8 * variable, unsigned int size)
		{
			if (1 <= size && size <= midminsize)
			{
				int index = fm1_sizetable[size];
				vecarr < FM_Model1 * >*fm1 = SmallSize_HeapDebugFM[index];
				for (int i = 0; i < (int)fm1->size(); ++i)
				{
					FM_Model1 *fm = fm1->at(i);
					if (fm->canInclude(variable, size))
					{
						int f0 = fm->Fup;
						bool b = fm->_Delete(variable, size);
						int f1 = fm->Fup;
						if (b)
						{
							if (f0 != f1)
							{
								int u, j;
								int key;
								int LEN = fm1->size();
								FM_Model1 *fm1k;
								for (u = 1; u < LEN; u++)
								{
									key = fm1->at(u)->get_fupm();
									fm1k = (*fm1)[u];
									for (j = u - 1; j >= 0 && fm1->at(j)->get_fupm() < key; j--)
									{
										(*fm1)[j + 1] = (*fm1)[j];
									}
									(*fm1)[j + 1] = fm1k;
								}
							}
							return true;
						}
					}
				}

				return false;
			}
			else if (midminsize <= size && size <= 255)
			{
				for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i)
				{
					FM_Model2 *fm = MidiumSize_HeapDebugFM[i];
					if (fm->canInclude(variable, size))
					{
						bool b = fm->_Delete(variable, size);
						if (b)
						{
							return true;
						}
					}
				}

				return false;
			}
			else if (256 <= size && size <= 65535)
			{
				for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i)
				{
					FM_Model2 *fm = BigSize_HeapDebugFM[i];
					if (fm->canInclude(variable, size))
					{
						bool b = fm->_Delete(variable, size);
						if (b)
						{
							return true;
						}
					}
				}

				return false;
			}

			return false;
		}

		bool bAlloc(byte8 * variable, unsigned int size)
		{
			if (1 <= size && size <= 39)
			{
				int index = fm1_sizetable[size];
				vecarr < FM_Model1 * >*fm1 = SmallSize_HeapDebugFM[index];
				for (int i = 0; i < (int)fm1->size(); ++i)
				{
					bool b = fm1->at(i)->bAlloc(variable, size);
					if (b)
					{
						return true;
					}
				}

				return false;
			}
			else if (40 <= size && size <= 255)
			{
				for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i)
				{
					bool b = MidiumSize_HeapDebugFM[i]->bAlloc(variable, size);
					if (b)
					{
						return true;
					}
				}

				return false;
			}
			else if (256 <= size && size <= 65535)
			{
				for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i)
				{
					bool b = BigSize_HeapDebugFM[i]->bAlloc(variable, size);
					if (b)
					{
						return true;
					}
				}

				return false;
			}

			return false;
		}

		void Temp_ClearAll(bool resetSize)
		{
			if (resetSize)
			{
				for (int i = 0; i < (int)TempFM.size(); ++i)
				{
					delete TempFM[i];
				}
				TempFM.clear();
				FM_Model0 *tempFM = new FM_Model0(new byte8[tempSize], tempSize);
				TempFM.push_back(tempFM);
			}
			else
			{
				for (int i = 0; i < (int)TempFM.size(); ++i)
				{
					TempFM[i]->ClearAll();
				}
			}
		}
	};
}

extern freemem::FM_System0 *fm;
	
namespace freemem{
	template < typename T > class fmvecarr
	{
	  public:
		T *Arr;
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

		~ fmvecarr()
		{
			if (islocal)
			{
				if (isdebug)
				{
					fm->_Delete(reinterpret_cast < byte8 * >(Arr), sizeof(T) * maxsize);
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
			T *newArr;
			if (isdebug)
			{
				newArr = (T *) fm->_New(sizeof(T) * siz, isdebug);
			}
			else
			{
				fmlayer = pfmlayer;
				newArr = (T *) fm->_tempNew(sizeof(T) * siz, fmlayer);
				if(fmlayer < 0){
					fmlayer = fm->tempStack[fm->get_threadid(std::this_thread::get_id())]->tempFM.size()-1;
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
					fm->_Delete(reinterpret_cast < byte8 * >(Arr), sizeof(T) * maxsize);
				}
				Arr = nullptr;
			}

			islocal = local;
			Arr = newArr;
			maxsize = siz;
			isdebug = isdebug;
		}

		T & at(size_t i)
		{
			return Arr[i];
		}

		T & operator[](size_t i) const
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
				fm->_Delete(reinterpret_cast < byte8 * >(Arr), sizeof(T) * maxsize);
			Arr = nullptr;
			up = 0;

			Init(2, islocal, isdebug, fmlayer);
		}

		T & last() const
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
				fm->_Delete(reinterpret_cast < byte8 * >(Arr), sizeof(T) * maxsize);
			Arr = nullptr;
			up = 0;
			islocal = false;
		}
	};

	template < typename T > struct fmlist_node
	{
		T value;
		  fmlist_node < T > *next = nullptr;
		  fmlist_node < T > *prev = nullptr;
	};
	template < typename T > class fmlist
	{
	  public:
	  	fmlist_node < T > *first;
		size_t size = 0;
		short fmlayer = -1;
		bool isdebug = false;
		bool islocal = false;

		fmlist(){}
		~fmlist(){
			if (!islocal)
			{
				while (first->next != nullptr)
				{
					erase(first);
				}
				if (isdebug)
				{
					fm->_Delete(reinterpret_cast < byte8 * >(first), sizeof(fmlist_node < T >));
				}
			}
		}

		void NULLState(){
			first = nullptr;
			size = 0;
			fmlayer = -1;
			isdebug = false;
			islocal = false;
		}

		void Init(bool isDbg, bool isLocal, short pfmlayer){
			isdebug = isDbg;
			islocal = isLocal;
			fmlayer = pfmlayer;
			if(fmlayer < 0){
				fmlayer = fm->tempStack[fm->get_threadid(std::this_thread::get_id())]->tempFM.size()-1;
			}
		}

		void release()
		{
		}

		void push_front(T value)
		{
			fmlist_node < T > *sav = first;
			first = (fmlist_node < T > *)fm->_New(sizeof(fmlist_node < T >), isdebug, (int)fmlayer);
			first->value = value;
			first->next = sav;
			first->prev = nullptr;
			sav->prev = first;
			++size;
		}

		inline fmlist_node < T > *getnext(fmlist_node < T > *node)
		{
			return node->next;
		}

		inline fmlist_node < T > *getprev(fmlist_node < T > *node)
		{
			return node->prev;
		}

		void erase(fmlist_node < T > *node)
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
				fm->_Delete(reinterpret_cast < byte8 * >(node), sizeof(fmlist_node < T >));
			}
			--size;
		}
	};

	typedef struct VP
	{
		char mod = 0;			// mod 0:value 1:ptr
		int *ptr = nullptr;		// arrgraph ptr or T ptr
	};

	template < typename T, typename V > struct range
	{
		T end;
		V value;
	};

	template < typename T, typename V > class ArrGraph
	{
	  public:
		fmvecarr < range < T, V > >* ranges;
		fmvecarr < VP > graph;
		short fmlayer = -1;
		bool islocal = false;
		bool isdebug = true;
		T minx = 0;
		T maxx = 0;
		T margin = 0;

		ArrGraph()
		{
		}
		~ ArrGraph()
		{
			if (islocal)
			{
				if (fm->bAlloc(reinterpret_cast < byte8 * >(ranges),
						   sizeof(vecarr < range < T, V > >)))
				{
					ranges->release();
					fm->_Delete((byte8 *) ranges, sizeof(fmvecarr < range < T, V > >));
					ranges = nullptr;
				}

				graph.release();
				graph.NULLState();
			}
		}

		ArrGraph *Init(T min, T max, bool isdbg, int pfmlayer = -1)
		{
			minx = min;
			maxx = max;
			fmlayer = (short)pfmlayer;
			isdebug = isdbg;
			ranges = (fmvecarr < range < T, V > >*)fm->_New(sizeof(fmvecarr < range < T, V > >), isdebug, (int)fmlayer);
			ranges->NULLState();
			ranges->Init(2, false, true, fmlayer);
			if(fmlayer < 0){
				fmlayer = fm->tempStack[fm->get_threadid(std::this_thread::get_id())]->tempFM.size()-1;
			}
			islocal = false;
			return this;
		}

		range < T, V > Range(T end, V value)
		{
			range < T, V > r;
			r.end = end;
			r.value = value;
			return r;
		}

		void push_range(range < T, V > r)
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
				f = floor(f) + 1;
				T average_length = (T) (f);
				margin = average_length;
				graph.NULLState();
				graph.Init(ranges->size(), false, true);
				graph.up = ranges->size();
				T start = minx;
				T end = start;
				for (int i = 0; i < graph.up; ++i)
				{
					end = start + average_length;
					if (end > maxx)
						end = maxx;
					T rstart = minx;
					for (int k = 0; k < ranges->up; ++k)
					{
						T rend = ranges->at(k).end;
						if (rstart <= start && end <= rend)
						{
							// num
							graph[i].mod = 0;
							graph[i].ptr = reinterpret_cast < int *>(&ranges->at(k).value);
							break;
						}
						else if (start <= rend && rend <= end)
						{
							// graph
							ArrGraph < T, V > *newgraph =
								(ArrGraph < T, V > *)fm->_New(sizeof(ArrGraph < T, V >), isdebug, fmlayer);
							newgraph->Init(start, end, isdebug, fmlayer);
							newgraph->push_range(ranges->at(k));
							range < T, V > *r = &ranges->at(k + 1);
							while (r->end <= end)
							{
								newgraph->push_range(*r);
								++k;
								if (k >= ranges->size())
								{
									break;
								}
								r = &ranges->at(k + 1);
							}
							// input last range
							range < T, V > lastr;
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
				graph.Init(2, false, isdebug, fmlayer);
				T center = ranges->at(0).end;
				T start = minx;
				T end = maxx - 1;
				if (maxx - center > center - start)
				{
					minx = 2 * center + 1 - end;
				}
				else
				{
					maxx = 2 * center + 1 - start;
				}
				margin = (maxx - minx) / ranges->size();
				VP vp0;
				vp0.mod = 0;
				vp0.ptr = reinterpret_cast < int *>(&ranges->at(0).value);
				graph.push_back(vp0);
				vp0.ptr = reinterpret_cast < int *>(&ranges->at(1).value);
				graph.push_back(vp0);
			}
		}

		V fx(T x)
		{
			static constexpr void *jumpptr[2] = { &&ISVALUE, &&ISGRAPH };
			ArrGraph < T, V > *ag = this;
			fmvecarr < VP > *g = &graph;
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
			ag = reinterpret_cast < ArrGraph < T, V > *>(vp.ptr);
			g = &ag->graph;
			goto GET_START;

		  ISVALUE:
			return *reinterpret_cast < V * >(vp.ptr);
		}

		void print_state()
		{
			cout << "arrgraph" << endl;
			cout << "minx : " << minx << endl;
			cout << "maxx : " << maxx << endl;
			cout << "capacity : " << graph.size() << endl;
			cout << "margin : " << margin << endl;
			for (int i = 0; i < graph.size(); ++i)
			{
				if (graph[i].mod == 0)
				{
					cout << "index : " << i << "] = value : " << *reinterpret_cast <
						V * >(graph[i].ptr) << endl;
				}
				else
				{
					cout << "index : " << i << "] = ptr : " << endl;
					reinterpret_cast < ArrGraph < T, V > *>(graph[i].ptr)->print_state();
					cout << endl;
				}
			}
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
		T *arr = nullptr; // 8byte
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


		fmCirculArr(const fmCirculArr<T> &ref)
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
			arr = (T *)fm->_New(sizeof(T) * (1 << maxsiz_pow2), mbDbg, fmlayer);
			pivot = 0;
		}

		void Release()
		{
			if(mbDbg){
				fm->_Delete((byte8 *)arr, sizeof(T) * (1 << maxsiz_pow2));
			}
			arr = nullptr;
		}

		inline void move_pivot(int dist)
		{
			pivot = ((1 << maxsiz_pow2) + pivot + dist) & (~(uintMax << (maxsiz_pow2)));
		}

		inline T &operator[](int index)
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
		fmCirculArr<int *> *ptrArray = nullptr; // 8byte
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
			if(ptrArray != nullptr){
				release();
			}
			fragment_siz_pow2 = fmgsiz_pow;
			array_depth = 1;
			mbDbg = isdbg;
			fmlayer = pfmlayer;
			if(ptrArray == nullptr){
				ptrArray = (fmCirculArr<int *> *)fm->_New(sizeof(fmCirculArr<int *>), mbDbg, (int)fmlayer);
				ptrArray->Init(fmgsiz_pow, mbDbg);
				int fmgsiz = 1 << fmgsiz_pow;
				for(int i=0;i<fmgsiz;++i){
					ptrArray->operator[](i) = nullptr;
				}
			}
			
			if(ptrArray->operator[](0) == nullptr){
				fmCirculArr<T> *arr =
					(fmCirculArr<T> *)fm->_New(sizeof(fmCirculArr<T>), mbDbg, (int)fmlayer);
				arr->Init(fragment_siz_pow2, mbDbg);
				ptrArray->operator[](0) = (int *)arr;
				lastCArr = arr;
			}

			for (int i = 1; i < (1 << fragment_siz_pow2); ++i)
			{
				ptrArray->operator[](i) = nullptr;
			}

			array_capacity = 1 << fragment_siz_pow2;
			array_siz = 0;

			fragPercent = ((1 << (fragment_siz_pow2))-1);

			lastCArr = this->get_bottom_array(0);

			// up -> depth
			if(up > 0){
				const unsigned int pushN = 1+(up >> fragment_siz_pow2);
				const unsigned int delta = (1 << fragment_siz_pow2);
				for(int i=0;i<pushN;++i){
					array_siz += delta;
					T v;
					push_back(v);
				}

				array_siz = up;
			}

			if(fmlayer < 0){
				fmlayer = fm->tempStack[fm->get_threadid(std::this_thread::get_id())]->tempFM.size() - 1;
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
				if(array_siz == 256){
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
					int *chptr = (int *)ptrArray;
					ptrArray = reinterpret_cast<fmCirculArr<int *> *>(fm->_New(sizeof(fmCirculArr<int *>), mbDbg, (int)fmlayer));
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
				fmCirculArr<int *> *ptr = ptrArray;
				int upcapacity = 0;
				for (int i = 0; i < array_depth; ++i)
				{
					int inindex = (int)(next >> fragment_siz_pow2 * (array_depth - i)) & (unsigned int)fragPercent;

					upcapacity += (inindex) << (fragment_siz_pow2 * (array_depth - i));

					fmCirculArr<int *> *tptr = ptr;
					ptr = reinterpret_cast<fmCirculArr<int *> *>(tptr->operator[](inindex));
					if (ptr == nullptr)
					{
						if (i == array_depth - 1)
						{
							fmCirculArr<T> *aptr =
								reinterpret_cast<fmCirculArr<T> *>(fm->_New(sizeof(fmCirculArr<T>), mbDbg, (int)fmlayer));
							aptr->Init(fragment_siz_pow2, mbDbg, (int)fmlayer);
							tptr->operator[](inindex) = (int *)aptr;
							ptr = reinterpret_cast<fmCirculArr<int *> *>(tptr->operator[](inindex));
						}
						else
						{
							fmCirculArr<int *> *insptr =
								reinterpret_cast<fmCirculArr<int *>*>(fm->_New(sizeof(fmCirculArr<int *>), mbDbg, (int)fmlayer));
							insptr->Init(fragment_siz_pow2, mbDbg, (int)fmlayer);
							tptr->operator[](inindex) = (int *)insptr;

							ptr = reinterpret_cast<fmCirculArr<int *> *>(tptr->operator[](inindex));
						}
					}
				}
				fmCirculArr<T> *vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);
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

		inline unsigned int size(){
			return array_siz;
		}
		//use caching. (pre bake function.)
		T &operator[](size_t index)
		{
			if((last_outerIndex >> fragment_siz_pow2) == index >> fragment_siz_pow2){
				ui32 k = index & (unsigned int)fragPercent;
				return lastCArr->operator[](k);
			}
			
			if (index >= array_capacity)
			{
				cout << "error! array index bigger than capacity!" << endl;
				T nullv;
				byte8* carr = reinterpret_cast<byte8*>(&nullv);
				for(int i=0;i<sizeof(T);++i){
					carr[i] = 0;
				}
				return nullv;
			}

			fmCirculArr<int *> *ptr = ptrArray;
			for (int i = 0; i < array_depth; ++i)
			{
				int depth_index = (index >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
				ptr = reinterpret_cast<fmCirculArr<int *> *>( ptr->operator[](depth_index));
			}
			fmCirculArr<T> *vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);

			lastCArr = vptr;
			last_outerIndex = index;

			// T *vptr = ptr;
			int inindex = ((int)(index)) & (unsigned int)fragPercent;
			return vptr->operator[](inindex);
		}

		fmCirculArr<T> *get_bottom_array(int index)
		{
			if((last_outerIndex >> fragment_siz_pow2) == index >> fragment_siz_pow2){
				return lastCArr;
			}

			if (index >= array_capacity)
			{
				return nullptr;
			}
			fmCirculArr<int *> *ptr = ptrArray;
			for (int i = 0; i < array_depth; ++i)
			{
				int depth_index = (index >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
				ptr = reinterpret_cast<fmCirculArr<int *> *>( ptr->operator[](depth_index));
				//ptr = reinterpret_cast<fmCirculArr<int *> *>(ptr->operator[]((int)((index >> (1 << fragment_siz_pow2) * (array_depth - i))) & fragPercent));
			}
			fmCirculArr<T> *vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);
			return vptr;
		}

		fmCirculArr<int *> *get_ptr_array(int index, int height)
		{
			if (index >= array_capacity)
			{
				return nullptr;
			}
			fmCirculArr<int *> *ptr = ptrArray;
			for (int i = 0; i < array_depth - height; ++i)
			{
				int depth_index = (index >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
				ptr = reinterpret_cast<fmCirculArr<int *> *>( ptr->operator[](depth_index));
			}
			return ptr;
		}

		// direction : -1 or 1
		void move(int index, int direction, bool expend)
		{
			ui32 fragSiz = (1 << fragment_siz_pow2);
			T save;
			fmCirculArr<T> *corearr = get_bottom_array(index);
			int inindex = index & (unsigned int)fragPercent;
			int last = 0;
			if (direction > 0)
			{
				last = (fragSiz) - direction;
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
					fmCirculArr<T> *temparr = get_bottom_array(next);

					if (temparr == nullptr)
					{
						if (expend)
						{
							push(save);
						}
						break;
					}

					fmCirculArr<T> *nextarr = nullptr;
					
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
					fmCirculArr<T> *temparr = get_bottom_array(next);

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

		T & at(size_t i)
		{
			ui32 ind = i;
			if((last_outerIndex >> fragment_siz_pow2) == ind >> fragment_siz_pow2){
				return lastCArr->operator[](ind & (unsigned int)fragPercent);
			}

			if (ind >= array_capacity)
			{
				T nullv;
				byte8* carr = reinterpret_cast<byte8*>(&nullv);
				for(int i=0;i<sizeof(T);++i){
					carr[i] = 0;
				}
				cout << "error! array index bigger than capacity!" << endl;
				return nullv;
			}
			fmCirculArr<int *> *ptr = ptrArray;
			for (int i = 0; i < array_depth; ++i)
			{
				int depth_index = (ind >> (fragment_siz_pow2 * (array_depth - i))) & (unsigned int)fragPercent;
				ptr = reinterpret_cast<fmCirculArr<int *> *>( ptr->operator[](depth_index));
			}
			fmCirculArr<T> *vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);

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

		T & last() const
		{
			return at(array_siz - 1);
		}

		//need test
		void release()
		{
			if(!mbDbg) return;
			for (ui32 k = 0; k < array_depth; ++k)
			{
				ui32 maxn = 1 << (fragment_siz_pow2 * array_depth - k);
				for (ui32 n = 0; n < maxn; ++n)
				{
					ui32 seek = n << (fragment_siz_pow2 * (k + 1));
					fmCirculArr<int *> *ptr = ptrArray;
					for (int i = 0; i < array_depth - k; ++i)
					{
						ptr = (fmCirculArr<int *> *)ptr->operator[](
							(int)((seek >> (fragment_siz_pow2 * (array_depth - i)))) & ((1 << (fragment_siz_pow2 + 1)) - 1));
					}

					if (ptr == nullptr)
					{
						break;
					}

					if (k == 0)
					{
						// most bottom real array
						fmCirculArr<T> *vptr = reinterpret_cast<fmCirculArr<T> *>(ptr);
						vptr->Release();
						fm->_Delete(reinterpret_cast<byte8 *>(vptr), sizeof(fmCirculArr<T>));
						// delete vptr;
					}
					else
					{
						// not bottom ptr array
						fmCirculArr<int *> *vptr = reinterpret_cast<fmCirculArr<int *> *>(ptr);
						vptr->Release();
						fm->_Delete(reinterpret_cast<byte8 *>(vptr), sizeof(fmCirculArr<int *>));
					}
				}
			}

			ptrArray->Release();
			fm->_Delete((byte8 *)ptrArray, sizeof(fmCirculArr<int *>));
			// delete[]ptrArray;
		}
	};

	class BitArray
	{
	  public:
	  	byte8 *Arr = nullptr;
		ui32 bit_arr_size = 0;	// saved bit count.
		ui32 byte_arr_size = 0;	// saved byte count.
		ui32 up = 0;
		short fmlayer = -1;
		bool isdebug = false;
		

		  BitArray(): bit_arr_size(0), byte_arr_size(0), Arr(nullptr), up(0), fmlayer(-1), isdebug(false)
		{

		}

		/*
		BitArray(size_t bitsize):
			bit_arr_size(bitsize), byte_arr_size((bitsize / 8) + 1), up(0)
		{
			Arr = fm->_New(byte_arr_size);
		}
		*/

		virtual ~ BitArray()
		{
			if(isdebug){
				fm->_Delete(Arr, byte_arr_size);
				Arr = nullptr;
			}
		}

		void NULLState(){

		}

		void Init(ui32 siz, bool isdbg, int pfmlayer = -1){
			bit_arr_size = siz;
			byte_arr_size = (bit_arr_size >> 3) + 1;
			isdebug = isdbg;
			fmlayer = pfmlayer;
			Arr = fm->_New(byte_arr_size, isdebug, fmlayer);
			if(fmlayer < 0){
				fmlayer = fm->tempStack[fm->get_threadid(std::this_thread::get_id())]->tempFM.size()-1;
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

		void addbit(bool bit)
		{
			if (up + 1 <= bit_arr_size)
			{
				++up;
				int i = up / 8;
				int loc = up % 8;
				_SetByte(Arr[i], loc, bit);
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
}
#endif
