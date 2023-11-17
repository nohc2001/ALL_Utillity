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

namespace freemem
{
#define Init_VPTR freemem::Init_VPTR_x64
#define ptr_size 8
#define ptr_max 0xFFFFFFFFFFFFFFFF
#define ptr_type int64_t

#define _GetByte(dat, loc) (dat >> loc) % 2
#define _SetByte(dat, loc, is) dat = freemem::SetByte8(dat, loc, is);
#define vins_New(FM, T, VariablePtr) ((T*)FM._New(sizeof(T)))->Init(); Init_VPTR<T>(VariablePtr);
#define ins_New(FM, T, VariablePtr) ((T*)FM._New(sizeof(T)))->Init();

	template < typename T > void Init_VPTR_x86(void *obj)
	{
		T go = T();
		__int32_t vp = *(__int32_t *) & go;
		 *((__int32_t *) obj) = vp;
	}

	template < typename T > void Init_VPTR_x64(void *obj)
	{
		T go;
		__int64_t vp = *(__int64_t *) & go;
		*((__int64_t *) obj) = vp;
	}

	class FM_Model
	{
	  public:
		FM_Model()
		{

		}
		virtual ~ FM_Model()
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

		virtual bool _Delete(byte8 * variable, unsigned int size)
		{
			return false;
		}

		// �ش� �ּҿ� �޸𸮰� �Ҵ�Ǿ�����.
		virtual bool bAlloc(byte8 * variable, unsigned int size)
		{
			return true;
		}

		virtual bool canInclude(byte8 * var, int size)
		{
			return true;
		}
	};

	inline void CheckRemainMemorySize()
	{
		byte8 end = 10;
		byte8 *start = new byte8();
		unsigned int RemainMemSiz = (unsigned int)(start - &end);
		cout << "RemainMemSiz : " << RemainMemSiz << " byte \t(" << (float)RemainMemSiz /
			1000.0f << " KB \t(" << (float)RemainMemSiz /
			1000000.0f << " MB \t(" << (float)RemainMemSiz / 1000000000.0f << " GB ) ) )" << endl;
		delete start;
	}

	class FM_Model0:FM_Model
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
				ptr_type start = (ptr_type) DataPtr;
				ptr_type end = (ptr_type) DataPtr;
				for (int i = 0; i < (int)AAsize; ++i)
				{
					start = end;
					start = start + ((byte8) (*(byte8 *) (start - 1)));
					end = (ptr_type) AllocArr[i];
					if (end - start > (int)byteSiz + 1)
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

	template < typename T > class InfiniteArray
	{
	  public:
		FM_Model * FM;
		T *Arr;
		size_t maxsize = 0;
		size_t up = 0;

	  InfiniteArray():
		FM(nullptr), Arr(nullptr), maxsize(0)
		{

		}

		virtual ~ InfiniteArray()
		{

		}

		void SetFM(FM_Model * fm)
		{
			FM = fm;
		}

		void NULLState()
		{
			FM = nullptr;
			Arr = nullptr;
			maxsize = 0;
			up = 0;
		}

		void Init(size_t siz)
		{
			Init_VPTR < InfiniteArray < T >> (this);

			T *newArr = (T *) FM->_New(sizeof(T) * siz);
			if (Arr != nullptr)
			{
				for (int i = 0; i < maxsize; ++i)
				{
					newArr[i] = Arr[i];
				}

				if (FM->bAlloc((byte8 *) Arr, sizeof(T) * maxsize))
				{
					FM->_Delete((byte8 *) Arr, sizeof(T) * maxsize);
				}
			}

			Arr = newArr;
			maxsize = siz;
		}

		T & at(size_t i)
		{
			return Arr[i];
		}

		T & operator[](size_t i)
		{
			return Arr[i];
		}

		void push_back(const T & value)
		{
			if (up < maxsize)
			{
				Arr[up] = value;
				up += 1;
			}
			else
			{
				Init(maxsize * 2 + 1);
				Arr[up] = value;
				up += 1;
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

		void insert(size_t i, const T & value)
		{
			push_back(value);
			for (int k = maxsize - 1; k > i; k--)
			{
				Arr[k] = Arr[k - 1];
			}
			Arr[i] = value;
		}

		size_t size()
		{
			return up;
		}

		void SetVPTR()
		{
			for (int i = 0; i < up; ++i)
			{
				Init_VPTR < T > (&Arr[i]);
			}
		}

		void clear()
		{
			if (FM->bAlloc((byte8 *) Arr, sizeof(T) * maxsize))
			{
				FM->_Delete((byte8 *) Arr, sizeof(T) * maxsize);
			}
			Arr = nullptr;
			up = 0;

			Init(2);
		}
	};

	struct large_alloc
	{
		int *ptr;
		int size = 0;
	};

	class TempStack
	{
	  public:
		int thread_id = 0;
		  vecarr < vecarr < large_alloc > *>large;
		  vecarr < vecarr < FM_Model0 * >*>tempFM;

		TempStack()
		{
		}
		virtual ~ TempStack()
		{
			release();
		}

		void init()
		{
			large.NULLState();
			large.Init(10, false);
			tempFM.NULLState();
			tempFM.Init(10, false);
			for (int i = 0; i < 10; ++i) {
				tempFM[i] = nullptr;
			}
			//watch("tempFM init", 0);
		}

		void release()
		{
			for (int i = 0; i < tempFM.size(); ++i)
			{
				for (int k = 0; k < tempFM[i]->size(); ++k)
				{
					tempFM[i]->at(k)->ClearAll();
					delete tempFM[i]->at(k);
				}
				tempFM[i]->release();
			}
			tempFM.release();

			for (int i = 0; i < large.size(); ++i)
			{
				for (int k = 0; k < large[i]->size(); ++k)
				{
					delete[](byte8 *) large[i]->at(k).ptr;
				}
				large[i]->release();
			}
			large.release();
		}

		byte8 *_New(unsigned int size, int fmlayer = -1)
		{
			vecarr<FM_Model0*>* tm;
			if(fmlayer < 0 || fmlayer >= tempFM.up) tm = tempFM.last();
			else tm = tempFM.at(fmlayer);
			unsigned int tsize = tm->size();

			for (int i = 0; i < tsize; ++i)
			{
				//watch("i", i);
				int remain = 4096 - tm->at(i)->Fup + 1;
				if (remain >= size)
				{
					return tm->at(i)->_New(size);
				}
			}

			if (size <= 4096)
			{
				FM_Model0 *fm0 = new FM_Model0();
				fm0->SetHeapData(new byte8[4096], 4096);
				tm->push_back(fm0);
				return tm->last()->_New(size);
			}
			else
			{
				large_alloc la;
				la.ptr = (int *)new byte8[size];
				la.size = size;
				large.last()->push_back(la);
				return reinterpret_cast < byte8 * >(la.ptr);
			}
		}

		void PushLayer()
		{
			if (tempFM[tempFM.up] == nullptr || tempFM.up == tempFM.maxsize)
			{
				vecarr < FM_Model0 * >* fmv = new vecarr < FM_Model0 * >();
				fmv->NULLState();
				fmv->Init(8, false);
				FM_Model0* fm0 = new FM_Model0();
				fm0->SetHeapData(new byte8[4096], 4096);
				fmv->push_back(fm0);
				tempFM.push_back(fmv);
				//watch("tempfm push", 0);

				large.push_back(new vecarr < large_alloc > ());
				large.last()->NULLState();
				large.last()->Init(8, false);
			}
			else
			{
				tempFM.up += 1;
			}
		}

		void PopLayer()
		{
			for (int i = 0; i < tempFM.last()->size(); ++i)
			{
				tempFM.last()->at(i)->Fup = 0;
			}
			tempFM.up -= 1;
			if (large.last()->size() > 0)
			{
				for (int i = 0; i < large.last()->size(); ++i)
				{
					delete[](byte8 *) large.last()->at(i).ptr;
				}
				large.last()->up = 0;
			}
			large.up -= 1;
		}

	};

	// The storage method is classified by the size of the data.
	class FM_System0
	{
	  public:
		// static constexpr int midminsize = 40; //x32
		static constexpr int midminsize = 72;	// x64
		unsigned int tempSize = 0;
		unsigned int sshd_Size = 0;
		unsigned int mshd_Size = 0;
		unsigned int bshd_Size = 0;
		int fm1_sizetable[128] = { };

		vecarr < std::thread::id > thread_idarr;
		vecarr < FM_Model0 * >TempFM;
		vecarr < TempStack * >tempStack;
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

		void SetHeapData(uint32_t temp, uint32_t sshd, uint32_t mshd, uint32_t bshd)
		{
			FILE *fp = fopen("fm1_sizetable.bin", "rb");
			for (int i = 1; i < 128; ++i)
			{
				int a = (int)fgetc(fp);
				int l = log2(a);
				fm1_sizetable[i] = l;
			}
			fclose(fp);

			tempSize = temp;
			sshd_Size = sshd;
			mshd_Size = mshd;
			bshd_Size = bshd;

			TempFM.NULLState();
			TempFM.Init(2, false);
			FM_Model0 *tempFM = new FM_Model0(new byte8[tempSize], tempSize);
			TempFM.push_back(tempFM);

			tempStack.NULLState();
			tempStack.Init(16, false);
			for (int i = 0; i < 8; ++i)
			{
				TempStack *ts = new TempStack();
				ts->init();
				tempStack.push_back(ts);
			}
			thread_idarr.NULLState();
			thread_idarr.Init(16, false);

			SmallSize_HeapDebugFM.NULLState();
			SmallSize_HeapDebugFM.Init(2, false);
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
			MidiumSize_HeapDebugFM.Init(2, false);
			FM_Model2 *mshdFM = new FM_Model2();
			mshdFM->SetHeapData(new byte8[mshd], mshd, 1);
			MidiumSize_HeapDebugFM.push_back(mshdFM);

			BigSize_HeapDebugFM.NULLState();
			BigSize_HeapDebugFM.Init(2, false);
			FM_Model2 *bshdFM = new FM_Model2();
			bshdFM->SetHeapData(new byte8[bshd], bshd, 2);
			BigSize_HeapDebugFM.push_back(bshdFM);
		}

		void thread_tm_push()
		{
			std::thread::id tid = std::this_thread::get_id();
			thread_idarr.push_back(tid);
		}

		void thread_tm_arrange()
		{
			for (int i = 0; i < thread_idarr.size(); ++i)
			{
				for (int k = i + 1; k < thread_idarr.size(); ++k)
				{
					if (thread_idarr[i] > thread_idarr[k])
					{
						std::thread::id a = thread_idarr[i];
						thread_idarr[i] = thread_idarr[k];
						thread_idarr[k] = a;
					}
				}
			}
		}

		void thread_tm_clear()
		{
			thread_idarr.up = 0;
		}

		int get_threadid(std::thread::id stdid)
		{
			int delta = thread_idarr.size() / 4;
			int piv = thread_idarr.size() / 2;
			while (delta > 0)
			{
				if (thread_idarr[piv] > stdid)
				{
					piv += delta;
					delta /= 2;
				}
				else if (thread_idarr[piv] < stdid)
				{
					piv -= delta;
					delta /= 2;
				}
				else
				{
					return piv;
				}
			}

			return 0;
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

		byte8 *_tempNew(unsigned int byteSiz, int fmlayer = -1)
		{
			return tempStack[get_threadid(std::this_thread::get_id())]->_New(byteSiz, fmlayer);
		}

		byte8 *_New(unsigned int byteSiz, bool isHeapDebug)
		{
			if (isHeapDebug == false)
			{
				for (int i = 0; i < (int)TempFM.size(); ++i)
				{
					byte8 *ptr = TempFM[i]->_New(byteSiz);
					if (ptr != nullptr)
					{
						return ptr;
					}
				}

				FM_Model0 *tempFM = new FM_Model0(new byte8[tempSize], tempSize);
				TempFM.push_back(tempFM);
				byte8 *ptr = TempFM[TempFM.size() - 1]->_New(byteSiz);
				return ptr;
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

	class BitArray
	{
	  public:
		FM_Model * FM = nullptr;
		int bit_arr_size = 0;	// saved bit count.
		int byte_arr_size = 0;	// saved byte count.
		byte8 *Arr = nullptr;
		int up = 0;

		  BitArray():FM(nullptr), bit_arr_size(0), byte_arr_size(0), Arr(nullptr), up(0)
		{

		}

		BitArray(FM_Model * fm, size_t bitsize):
			FM(fm), bit_arr_size(bitsize), byte_arr_size((bitsize / 8) + 1), up(0)
		{
			Arr = FM->_New(byte_arr_size);
		}

		virtual ~ BitArray()
		{
			FM->_Delete(Arr, byte_arr_size);
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
		}

		virtual ~ fmvecarr()
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
		}

		void Init(size_t siz, bool local, bool isdebug = false)
		{
			T *newArr;
			if (isdebug)
			{
				newArr = (T *) fm->_New(sizeof(T) * siz, isdebug);
			}
			else
			{
				newArr = (T *) fm->_tempNew(sizeof(T) * siz, fmlayer);
				if(fmlayer < 0){
					fmlayer = fm->tempStack[fm->get_threadid(std::this_thread::get_id())]->tempFM.size()-1;
				}
				else{
					//cout << "reuse" << endl;
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
				Init(maxsize * 2 + 1, islocal, isdebug);
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

			Init(2, islocal, isdebug);
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
		bool isdebug = false;
		bool islocal = false;
		size_t size = 0;
		fmlist_node < T > *first;

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

		void Init(T fv) {
			first = (fmlist_node<T>*)fm->_tempNew(sizeof(fmlist_node<T>), -1);
			first->value = fv;
			first->next = nullptr;
			first->prev = nullptr;
			++size;
		}

		void release()
		{
		}

		void push_front(T value)
		{
			fmlist_node < T > *sav = first;
			first = (fmlist_node < T > *)fm->_New(sizeof(fmlist_node < T >), isdebug);
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
}
#endif