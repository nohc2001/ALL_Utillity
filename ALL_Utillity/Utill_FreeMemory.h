#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;
typedef unsigned char byte8;

namespace freemem {
#ifdef _WIN32
	#ifdef _WIN64
		#ifdef _M_X64
			#define Init_VPTR freemem::Init_VPTR_x64
			#define ptr_size 8
			#define ptr_max 0xFFFFFFFFFFFFFFFF
			#define ptr_type int64_t
		#elif defined(_M_IA64)
			#define Init_VPTR freemem::Init_VPTR_x64
			#define ptr_size 8
			#define ptr_max 0xFFFFFFFFFFFFFFFF
			#define ptr_type int64_t
		#endif
	#elif defined(_M_IX86)
		#define Init_VPTR freemem::Init_VPTR_x86
		#define ptr_size 4
		#define ptr_max 0xFFFFFFFF
		#define ptr_type int32_t
	#endif
#else
	#define Init_VPTR freemem::Init_VPTR_x86
#endif

#define GetByte(dat, loc) (dat >> loc) % 2
#define SetByte(dat, loc, is) if ((dat >> loc) % 2) { if (!is) { dat =  dat - (1 << loc); } } else if (is) { dat = dat + (1 << loc); }

	template<typename T> void Init_VPTR_x86(void* obj) {
		T go = T();
		__int32 vp = *(__int32*)&go;
		*((__int32*)obj) = vp;
	}

	template<typename T> void Init_VPTR_x64(void* obj) {
		T go;
		__int64 vp = *(__int64*)&go;
		*((__int64*)obj) = vp;
	}

	class FM_Model {
	public:
		FM_Model() {}
		virtual ~FM_Model() {}

		virtual byte8* _New(unsigned int byteSiz) {
			return nullptr;
		}

		virtual void ClearAll() {
			return;
		}

		virtual bool _Delete(byte8* variable, unsigned int size) {
			return false;
		}

		//해당 주소에 메모리가 할당되었는지.
		virtual bool bAlloc(byte8* variable, unsigned int size) {
			return true;
		}
	};

	inline void CheckRemainMemorySize() {
		byte8 end = 10;
		byte8* start = new byte8();
		unsigned int RemainMemSiz = (unsigned int)start - (unsigned int)&end;
		cout << "RemainMemSiz : " << RemainMemSiz << " byte \t(" << (float)RemainMemSiz / 1000.0f << " KB \t(" << (float)RemainMemSiz / 1000000.0f << " MB \t(" << (float)RemainMemSiz / 1000000000.0f << " GB ) ) )" << endl;
		delete start;
	}

	class FM_Model0 : FM_Model {
	public:
		unsigned int siz = 0;
		byte8* Data = nullptr;
		unsigned int Fup = 0;
		bool isHeap = false;

		FM_Model0() {}
		FM_Model0(byte8* data, unsigned int Size) {
			Data = data;
			siz = Size;
		}
		virtual ~FM_Model0() {
			if (isHeap && Data != nullptr) {
				delete[] Data;
			}
		}

		void SetHeapData(byte8* data, unsigned int Size) {
			isHeap = true;
			Data = data;
			siz = Size;
		}

		byte8* _New(unsigned int byteSiz) {
			if (Fup + byteSiz < siz) {
				unsigned int fup = Fup;
				Fup += byteSiz;
				return &Data[fup];
			}
			else {
				ClearAll();
				return _New(byteSiz);
			}
		}

		bool _Delete(byte8* variable, unsigned int size) {
			return false;
		}

		void ClearAll() {
			Fup = 0;
		}

		void PrintState() {
			cout << "FreeMemory Model 0 State -----------------" << endl;
			CheckRemainMemorySize();
			cout << "MAX byte : \t" << siz << " byte \t(" << (float)siz / 1024.0f << " KB \t(" << (float)siz / powf(1024.0f, 2) << " MB \t(" << (float)siz / powf(1024.0f, 3) << " GB ) ) )" << endl;
			cout << "Alloc Number : \t" << Fup << " byte \t(" << (float)Fup / 1024.0f << " KB \t(" << (float)Fup / powf(1024.0f, 2) << " MB \t(" << (float)Fup / powf(1024.0f, 3) << " GB ) ) )" << endl;
			cout << "FreeMemory Model 0 State END -----------------" << endl;
		}
	};

	inline bool GetByte8(byte8 dat, int loc) {
		return (dat >> loc) % 2;
	}

	inline byte8 SetByte8(byte8 dat, int loc, bool is1) {
		if ((dat >> loc) % 2) {
			if (!is1) {
				return dat - (1 << loc);
			}
		}
		else if (is1) {
			return dat + (1 << loc);
		}

		return dat;
	}

	class FM_Model1 : FM_Model {
	public:
		bool isHeap = false; // true면 heap, false면 stack
		byte8* DataPtr = nullptr;
		unsigned int realDataSiz = 0;
		unsigned int sumDataSiz = 0;

		unsigned int Fup = 0;
		int isvalidNum = 0;

		FM_Model1() {

		}

		FM_Model1(unsigned int RDS, byte8* dataptr) {
			DataPtr = dataptr;
			realDataSiz = RDS;
			sumDataSiz = 9 * realDataSiz / 8;
		}

		virtual ~FM_Model1() {
			if (isHeap && DataPtr != nullptr) {
				delete[] DataPtr;
			}
		}

		void SetHeapData(byte8* dataptr, unsigned int SDS) {
			isHeap = true;
			DataPtr = dataptr;
			sumDataSiz = SDS;
			realDataSiz = 8 * sumDataSiz / 9;
		}

		bool isValid(unsigned int address) {
			int bigloc = address / 8;
			int smallLoc = address % 8;
			if (GetByte(DataPtr[realDataSiz + bigloc], smallLoc)) {
				return false;
			}
			else return true;
		}

		void SetValid(unsigned int address, bool enable) {
			int bigloc = address / 8;
			int smallLoc = address % 8;
			DataPtr[realDataSiz + bigloc] = SetByte8(DataPtr[realDataSiz + bigloc], smallLoc, enable);
		}

		byte8* _New(unsigned int size) {
			int stack = 0;
			if (Fup + size < realDataSiz) {
				int RAD = Fup;
				for (int i = 0; i < (int)size; ++i) {
					SetValid(RAD + i, true);
				}
				Fup += size;
				return &DataPtr[RAD];
			}
			else {
				for (int ad = 0; ad < (int)realDataSiz; ++ad) {
					if (isValid(ad)) {
						stack += 1;
						if (stack == size) {
							int RAD = ad - size + 1;
							for (int i = 0; i < (int)size; ++i) {
								SetValid(RAD + i, true);
							}
							return &DataPtr[RAD];
						}
					}
					else {
						stack = 0;
					}
				}
			}
			return nullptr;
		}

		bool _Delete(byte8* variable, unsigned int size) {
			unsigned int address = variable - DataPtr;
			//for (int i = 0; i < size; ++i) {
			//	if (isValid(address + i) == false) {
			//		return false;
			//	}
			//}

			for (int i = 0; i < (int)size; ++i) {
				SetValid(address + i, false);
			}

			return true;
		}

		//해당 주소에 메모리가 할당되었는지.
		bool bAlloc(byte8* variable, unsigned int size) {
			unsigned int address = variable - DataPtr;
			for (int i = 0; i < (int)size; ++i) {
				if (isValid(address + i) == false) {
					return false;
				}
			}

			return true;
		}

		void DebugAddresses() {
			int count = 0;
			isvalidNum = 0;
			for (int i = 0; i < (int)realDataSiz; ++i) {
				if (isValid(i) == false) {
					count += 1;
				}
			}

			cout << "Non Release Free Memory(no matter) : " << count << endl;

			ofstream out;
			out.open("DebugFile.txt");
			for (int i = 0; i < (int)Fup; ++i) {
				if (isValid(i)) {
					out << '0';
					isvalidNum += 1;
				}
				else {
					out << '1';
				}
			}
			out.close();
		}

		void ClearAll() {
			for (int i = 0; i < (int)realDataSiz; ++i) {
				SetValid(i, false);
			}
		}
	};

	typedef byte8* AllocPtr;

	class FM_Model2 : FM_Model {
	public:
		bool isHeap = false;
		byte8* DataPtr = nullptr;
		int Fup = 0;
		
		unsigned int DataPoolSize = 0;
		unsigned int SizeMemorySize = 1;
		unsigned int MinimumDataSize = 40; //SizeMemorySize * 8 * 5

		AllocPtr* AllocArr = nullptr;
		unsigned int AAsize = 0; // DataPoolSize / MinimumDataSize;
		int aaup = 0;
		

		FM_Model2() {

		}

		virtual ~FM_Model2() {

		}

		void ArrangeAllocArr() {
			//삽입정렬
			while (aaup > 0 && (uint32_t)AllocArr[aaup - 1] == ptr_max) {
				aaup -= 1;
			}

			int i, j;
			uint32_t key;
			for (i = 1; i < aaup; i++) {
				key = (uint32_t)AllocArr[i];
				for (j = i - 1; j >= 0 && (uint32_t)AllocArr[j] > key; j--) {
					AllocArr[j + 1] = AllocArr[j];
				}

				AllocArr[j + 1] = (byte8*)key;
			}
		}

		virtual byte8* _New(unsigned int byteSiz) {
			ArrangeAllocArr();
			if (Fup < (int)DataPoolSize) {
				if (byteSiz >= MinimumDataSize) {
					if (Fup + byteSiz < DataPoolSize) {
						if (SizeMemorySize == 1) {
							DataPtr[Fup] = (byte8)byteSiz;
						}
						else if (SizeMemorySize == 2) {
							*((unsigned short*)&DataPtr[Fup]) = (unsigned short)byteSiz;
						}
						Fup += SizeMemorySize;
						int ptr = Fup;
						Fup += byteSiz;
						if (aaup + 1 < (int)AAsize) {
							AllocArr[aaup] = &DataPtr[ptr];
							aaup += 1;
						}
						return &DataPtr[ptr];
					}
				}
			}
			else {
				//fup 이 꽉 찼을때
				ptr_type start = (ptr_type)DataPtr;
				ptr_type end = (ptr_type)DataPtr;
				for (int i = 0; i < (int)AAsize; ++i) {
					start = end;
					start = start + ((byte8)(*(byte8*)(start - 1)));
					end = (ptr_type)AllocArr[i];
					if (end - start > (int)byteSiz + 1) {
						int index = start + 1 - (ptr_type)DataPtr;
						if (SizeMemorySize == 1) {
							DataPtr[index] = (byte8)byteSiz;	
						}
						else if (SizeMemorySize == 2) {
							*((unsigned short*)&DataPtr[index]) = (unsigned short)byteSiz;
						}
						index += SizeMemorySize;
						int ptr = index;
						index += byteSiz;
						if (aaup + 1 < (int)AAsize) {
							AllocArr[aaup] = &DataPtr[ptr];
							aaup += 1;
						}
						return &DataPtr[ptr];
					}
				}
			}
			return nullptr;
		}

		virtual void ClearAll() {
			aaup = 0;
			Fup = 0;
			return;
		}

		virtual bool _Delete(byte8* variable, unsigned int size) {
			//삽입정렬
			ArrangeAllocArr();

			bool deleted = false;
			int start = 0;
			int end = aaup-1;
			int mid = (start + end) / 2;
			while (end - start > 10) {
				if (AllocArr[mid] > variable) {
					end = mid;
					mid = (start + end) / 2;
				}
				else if (AllocArr[mid] < variable) {
					start = mid;
					mid = (start + end) / 2;
				}
				else {
					AllocArr[mid] = (byte8*)ptr_max;
					deleted = true;
				}
			}

			if (deleted) {
				for (int i = start; i < end + 1; ++i) {
					if (AllocArr[i] == variable) {
						AllocArr[i] = (byte8*)ptr_max;
						deleted = true;
					}
				}
			}

			if (deleted) return true;
			else return false;
		}

		//해당 주소에 메모리가 할당되었는지.
		virtual bool bAlloc(byte8* variable, unsigned int size) {
			ArrangeAllocArr();

			bool alloc = false;
			int start = 0;
			int end = aaup - 1;
			int mid = (start + end) / 2;
			while (end - start > 10) {
				if (AllocArr[mid] > variable) {
					end = mid;
					mid = (start + end) / 2;
				}
				else if (AllocArr[mid] < variable) {
					start = mid;
					mid = (start + end) / 2;
				}
				else {
					alloc = true;
				}
			}

			if (alloc) {
				for (int i = start; i < end + 1; ++i) {
					if (AllocArr[i] == variable) {
						alloc = true;
					}
				}
			}

			return alloc;
		}

		void SetHeapData(byte8* dataptr, unsigned int dataSize, unsigned int sizedatasize) {
			isHeap = true;
			DataPtr = dataptr;
			DataPoolSize = dataSize;
			SizeMemorySize = sizedatasize;
			MinimumDataSize = (SizeMemorySize + ptr_size) * 8;
			AAsize = (DataPoolSize / MinimumDataSize) + 1;
			AllocArr = new AllocPtr[AAsize];
		}

		int GetSizeOfVariable(byte8* variable) {
			if (bAlloc(variable, NULL)) {
				byte8 b = *(variable - SizeMemorySize);
				int size = b;
				return size;
			}

			return -1;
		}
	};

	template<typename T> class InfiniteArray {
	public:
		FM_Model* FM;
		T* Arr;
		size_t maxsize = 0;
		size_t up = 0;

		InfiniteArray() :
			FM(nullptr),
			Arr(nullptr),
			maxsize(0)
		{

		}

		virtual ~InfiniteArray() {

		}

		void SetFM(FM_Model* fm) {
			FM = fm;
		}

		void NULLState() {
			FM = nullptr;
			Arr = nullptr;
			maxsize = 0;
			up = 0;
		}

		void Init(size_t siz) {
			Init_VPTR<InfiniteArray<T>>(this);

			T* newArr = (T*)FM->_New(sizeof(T) * siz);
			if (Arr != nullptr) {
				for (int i = 0; i < maxsize; ++i) {
					newArr[i] = Arr[i];
				}

				if (FM->bAlloc((byte8*)Arr, sizeof(T) * maxsize)) {
					FM->_Delete((byte8*)Arr, sizeof(T) * maxsize);
				}
			}

			Arr = newArr;
			maxsize = siz;
		}

		T& at(size_t i) {
			return Arr[i];
		}

		T& operator[](size_t i) {
			return Arr[i];
		}

		void push_back(const T& value) {
			if (up < maxsize) {
				Arr[up] = value;
				up += 1;
			}
			else {
				Init(maxsize * 2 + 1);
				Arr[up] = value;
				up += 1;
			}
		}

		void erase(size_t i) {
			for (int k = i; k < up; ++k) {
				Arr[k] = Arr[k + 1];
			}
			up -= 1;
		}

		void insert(size_t i, const T& value) {
			push_back(value);
			for (int k = maxsize - 1; k > i; k--) {
				Arr[k] = Arr[k - 1];
			}
			Arr[i] = value;
		}

		size_t size() {
			return up;
		}

		void SetVPTR() {
			for (int i = 0; i < up; ++i) {
				Init_VPTR<T>(&Arr[i]);
			}
		}

		void clear() {
			if (FM->bAlloc((byte8*)Arr, sizeof(T) * maxsize)) {
				FM->_Delete((byte8*)Arr, sizeof(T) * maxsize);
			}
			Arr = nullptr;
			up = 0;

			Init(2);
		}
	};

	//The storage method is classified by the size of the data.
	class FM_System0 {
	public:
		unsigned int tempSize = 0;
		unsigned int sshd_Size = 0;
		unsigned int mshd_Size = 0;
		unsigned int bshd_Size = 0;

		vector<FM_Model0*> TempFM; // any size temp memory
		vector<FM_Model1*> SmallSize_HeapDebugFM; // 1 ~ 39 byte small memeory
		vector<FM_Model2*> MidiumSize_HeapDebugFM; // 40 ~ 255 byte midium memeory
		vector<FM_Model2*> BigSize_HeapDebugFM; // 256 ~ 65535 byte big memeory

		FM_System0() {

		}

		virtual ~FM_System0() {

		}

		void SetHeapData(uint32_t temp, uint32_t sshd, uint32_t mshd, uint32_t bshd) {
			tempSize = temp;
			sshd_Size = sshd;
			mshd_Size = mshd;
			bshd_Size = bshd;

			FM_Model0* tempFM = new FM_Model0(new byte8[tempSize], tempSize);
			TempFM.push_back(tempFM);

			FM_Model1* sshdFM = new FM_Model1();
			sshdFM->SetHeapData(new byte8[9 * (sshd_Size / 8)], sshd_Size);
			SmallSize_HeapDebugFM.push_back(sshdFM);

			FM_Model2* mshdFM = new FM_Model2();
			mshdFM->SetHeapData(new byte8[mshd], mshd, 1);
			MidiumSize_HeapDebugFM.push_back(mshdFM);

			FM_Model2* bshdFM = new FM_Model2();
			bshdFM->SetHeapData(new byte8[bshd], bshd, 2);
			BigSize_HeapDebugFM.push_back(bshdFM);
		}

		byte8* _New(unsigned int byteSiz, bool isHeapDebug) {
			if (isHeapDebug == false) {
				for (int i = 0; i < (int)TempFM.size(); ++i) {
					byte8* ptr = TempFM[i]->_New(byteSiz);
					if (ptr != nullptr) {
						return ptr;
					}
				}

				FM_Model0* tempFM = new FM_Model0(new byte8[tempSize], tempSize);
				TempFM.push_back(tempFM);
				byte8* ptr = TempFM[TempFM.size()-1]->_New(byteSiz);
				return ptr;
			}
			else {
				if (1 <= byteSiz && byteSiz <= 39) {
					for (int i = 0; i < (int)SmallSize_HeapDebugFM.size(); ++i) {
						byte8* ptr = SmallSize_HeapDebugFM[i]->_New(byteSiz);
						if (ptr != nullptr) {
							return ptr;
						}
					}

					FM_Model1* sshdFM = new FM_Model1();
					sshdFM->SetHeapData(new byte8[9 * (sshd_Size / 8)], sshd_Size);
					SmallSize_HeapDebugFM.push_back(sshdFM);
					byte8* ptr = SmallSize_HeapDebugFM[SmallSize_HeapDebugFM.size() - 1]->_New(byteSiz);
					return ptr;
				}
				else if (40 <= byteSiz && byteSiz <= 255) {
					for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i) {
						byte8* ptr = MidiumSize_HeapDebugFM[i]->_New(byteSiz);
						if (ptr != nullptr) {
							return ptr;
						}
					}

					FM_Model2* mshdFM = new FM_Model2();
					mshdFM->SetHeapData(new byte8[mshd_Size], mshd_Size, 1);
					MidiumSize_HeapDebugFM.push_back(mshdFM);
					byte8* ptr = MidiumSize_HeapDebugFM[MidiumSize_HeapDebugFM.size() - 1]->_New(byteSiz);
					return ptr;
				}
				else if (256 <= byteSiz && byteSiz <= 65535) {
					for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i) {
						byte8* ptr = BigSize_HeapDebugFM[i]->_New(byteSiz);
						if (ptr != nullptr) {
							return ptr;
						}
					}

					FM_Model2* bshdFM = new FM_Model2();
					bshdFM->SetHeapData(new byte8[bshd_Size], bshd_Size, 2);
					BigSize_HeapDebugFM.push_back(bshdFM);
					byte8* ptr = BigSize_HeapDebugFM[BigSize_HeapDebugFM.size() - 1]->_New(byteSiz);
					return ptr;
				}
			}
		}

		bool _Delete(byte8* variable, unsigned int size) {
			if (1 <= size && size <= 39) {
				for (int i = 0; i < (int)SmallSize_HeapDebugFM.size(); ++i) {
					bool b = SmallSize_HeapDebugFM[i]->_Delete(variable, size);
					if (b) {
						return true;
					}
				}

				return false;
			}
			else if (40 <= size && size <= 255) {
				for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i) {
					bool b = MidiumSize_HeapDebugFM[i]->_Delete(variable, size);
					if (b) {
						return true;
					}
				}

				return false;
			}
			else if (256 <= size && size <= 65535) {
				for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i) {
					bool b = BigSize_HeapDebugFM[i]->_Delete(variable, size);
					if (b) {
						return true;
					}
				}

				return false;
			}

			return false;
		}

		bool bAlloc(byte8* variable, unsigned int size) {
			if (1 <= size && size <= 39) {
				for (int i = 0; i < (int)SmallSize_HeapDebugFM.size(); ++i) {
					bool b = SmallSize_HeapDebugFM[i]->bAlloc(variable, size);
					if (b) {
						return true;
					}
				}

				return false;
			}
			else if (40 <= size && size <= 255) {
				for (int i = 0; i < (int)MidiumSize_HeapDebugFM.size(); ++i) {
					bool b = MidiumSize_HeapDebugFM[i]->bAlloc(variable, size);
					if (b) {
						return true;
					}
				}

				return false;
			}
			else if (256 <= size && size <= 65535) {
				for (int i = 0; i < (int)BigSize_HeapDebugFM.size(); ++i) {
					bool b = BigSize_HeapDebugFM[i]->bAlloc(variable, size);
					if (b) {
						return true;
					}
				}

				return false;
			}

			return false;
		}

		void Temp_ClearAll(bool resetSize) {
			if (resetSize) {
				for (int i = 0; i < (int)TempFM.size(); ++i) {
					delete TempFM[i];
				}
				TempFM.clear();
				FM_Model0* tempFM = new FM_Model0(new byte8[tempSize], tempSize);
				TempFM.push_back(tempFM);
			}
			else {
				for (int i = 0; i < (int)TempFM.size(); ++i) {
					TempFM[i]->ClearAll();
				}
			}
		}
	};

	class BitArray {
	public:
		FM_Model* FM = nullptr;
		int bit_arr_size = 0; // saved bit count.
		int byte_arr_size = 0; // saved byte count.
		byte8* Arr = nullptr;
		int up = 0;

		BitArray() :
			FM(nullptr), bit_arr_size(0), byte_arr_size(0), Arr(nullptr), up(0)
		{

		}

		BitArray(FM_Model* fm, size_t bitsize) :
			FM(fm), bit_arr_size(bitsize), byte_arr_size((bitsize/8) + 1), up(0)
		{
			Arr = FM->_New(byte_arr_size);
		}

		virtual ~BitArray() {
			FM->_Delete(Arr, byte_arr_size);
		}

		string get_bit_char() {
			string str;
			int byteup = (up / 8) + 1;
			for (int i = 0; i < byteup; ++i) {
				for (int lo = 0; lo < 8; ++lo) {
					if (up <= i * 8 + lo) break;
					int n = GetByte(Arr[i], lo);
					if (n == 0) {
						str.push_back('0');
					}
					else if (n == 1) {
						str.push_back('1');
					}
				}
			}
			return str;
		}

		void addbit(bool bit) {
			if (up + 1 <= bit_arr_size) {
				++up;
				int i = up / 8;
				int loc = up % 8;
				SetByte(Arr[i], loc, bit);
			}
		}

		void SetUp(int n) {
			up = n;
			if (up <= bit_arr_size) {
				up = bit_arr_size;
			}
		}

		void setbit(int index, bool bit) {
			if (0 <= index && index < up) {
				int i = index / 8;
				int loc = index % 8;
				SetByte(Arr[i], loc, bit);
			}
		}

		bool getbit(int index) {
			if (0 <= index && index < up) {
				int i = index / 8;
				int loc = index % 8;
				return GetByte(Arr[i], loc);
			}
		}
	};
}

