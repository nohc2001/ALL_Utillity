#include "Utill_FreeMemory.h"
using namespace freemem;

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#define REPEAT_NUM 1

#define TEST_HAVESTR0

#if defined(TEST_REV)

#define _$Work0 ui32_rev_table
#define _$Work1 ui32_rev_for
#define _$Work2 ui32_rev_hd
#define _$Work3 ui32_rev_table_shuffle

constexpr ui8 rev8bit[256] = { 0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248, 4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252, 2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250, 6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245, 13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247, 15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255 };

//111 clock -> 37
__forceinline ui32 ui32_rev_table(ui32 x)
{
	/*
	ui32 r = rev8bit[x & 255] << 24;
	r |= rev8bit[(x >> 8) & 255] << 16;
	r |= rev8bit[(x >> 16) & 255] << 8;
	r |= rev8bit[(x >> 24) & 255];
	return r;
	step : 6
	*/
	//ui32 x1, x2, x3, x4;
	//// step1
	//x1 = x & 255;
	//x2 = x >> 8;
	//x3 = x >> 16;
	//x4 = x >> 24;
	//// step2
	//x1 = rev8bit[x1];
	//x2 = x2 & 255;
	//x3 = x3 & 255;
	//x4 = x4 & 255;
	//// step3;
	//x1 = x1 << 24;
	//x2 = rev8bit[x2];
	//x3 = rev8bit[x3];
	//x4 = rev8bit[x4];
	//// step4;
	//x1 |= x4;
	//x2 = x2 << 16;
	//x3 = x3 << 8;
	//// step5
	//x1 |= x2;
	//// step6
	//x1 |= x3;
	//return x1;

	ui32 r = rev8bit[x & 255] << 24;
	r |= rev8bit[(x >> 8) & 255] << 16;
	r |= rev8bit[(x >> 16) & 255] << 8;
	r |= rev8bit[(x >> 24) & 255];
	return r;
}

//703 clock -> 148
__forceinline ui32 ui32_rev_for(ui32 x) {
	ui32 r = 0;
	for (int i = 0; i < 32; ++i) {
		ui32 x1 = ((x & (1 << i)) >> i) << (31 - i);
		r |= x1;
	}
	return r;
}

//444 clock -> 37 (the best)
__forceinline ui32 ui32_rev_hd(ui32 x) {
	x = (x & 0x55555555) << 1 | (x >> 1) & 0x55555555;
	x = (x & 0x33333333) << 2 | (x >> 2) & 0x33333333;
	x = (x & 0x0F0F0F0F) << 4 | (x >> 4) & 0x0F0F0F0F;
	x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

//37 clock
__forceinline ui32 ui32_rev_table_shuffle(ui32 x) {
	ui16 s0 = x & 0x0000FFFF;
	ui16 s1 = (x & 0xFFFF0000) >> 16;
	s0 = (rev8bit[s0 & 255] << 8) + rev8bit[(s0 & 0xFF00) >> 8];
	s1 = (rev8bit[s1 & 255] << 8) + rev8bit[(s1 & 0xFF00) >> 8];
	x = (s0 << 16) + s1;
	return x;
}

#elif defined(TEST_HAVESTR0)
#define _$Work0 ui32_havestr0
#define _$Work1 ui32_havestr0
#define _$Work2 ui32_havestr0
#define _$Work3 ui32_havestr0

#define _$PreWork AddFlagArr

#define RType bool

ui32 behaving_0str_flag_arr[32];
void AddFlagArr(ui32 n) {
	ui32 rp = log2(32 / n);
	for (int i = 0; i < n; ++i) {
		behaving_0str_flag_arr[i] = (1 << n) - 1;
		for (int k = 1; k <= rp; ++k) {
			behaving_0str_flag_arr[i] &= behaving_0str_flag_arr[i] << k * n;
		}
	}
}

__forceinline bool ui32_havestr0(ui32 x, int n)
{
	ui32 total = 0;
	for (int i = 0; i < n; ++i) {
		total += !(x | behaving_0str_flag_arr[i]);
	}
	return total;
}

#endif

void Test() {
	ui32 data;
	cin >> data;
	ui64 t0 = __rdtsc();

	RType r[REPEAT_NUM] = {};
	r[0] = data;
	for (int i = 1; i < REPEAT_NUM; i++) {
		r[i] = _$Work0(r[i-1]);
	}

	ui64 t1 = __rdtsc();
	cout << STR(_$Work0) << " time : \n" << t1 - t0 << "\n" << r0 << endl;
	////////////////////////////////////////////////////////
	t0 = __rdtsc();
	//todo 
	r[0] = data;
	for (int i = 1; i < REPEAT_NUM; i++) {
		r[i] = _$Work1(r[i - 1]);
	}
	t1 = __rdtsc();
	cout << STR(_$Work1) << " time : \n" << t1 - t0 << "\n" << r0 << endl;
	////////////////////////////////////////////////////////
	t0 = __rdtsc();
	//todo
	r[0] = data;
	for (int i = 1; i < REPEAT_NUM; i++) {
		r[i] = _$Work2(r[i - 1]);
	}
	t1 = __rdtsc();
	cout << STR(_$Work2) << " time : \n" << t1 - t0 << "\n" << r0 << endl;
	////////////////////////////////////////////////////////
	t0 = __rdtsc();
	//todo
	r[0] = data;
	for (int i = 1; i < REPEAT_NUM; i++) {
		r[i] = _$Work3(r[i - 1]);
	}
	t1 = __rdtsc();
	cout << STR(_$Work3) << " time : \n" << t1 - t0 << "\n" << r0 << endl;
}

int main() {
	Test();
	return 0;
}