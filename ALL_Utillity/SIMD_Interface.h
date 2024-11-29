#ifndef SIMD_INTERFACE_H
#define SIMD_INTERFACE_H

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
#include "CodeDependency.h"
#include <xmmintrin.h> // SSE
#include <immintrin.h> // AVX, AVX2, FMA
#include <intrin.h>

//how to 
//#define UNSIGNED_SIMD_CMP_SHIFTCMP
#define UNSIGNED_SIMD_CMP_PACKCMP

//half float
struct hfloat {
	ui16 data;

	__forceinline ui16 getSign() { return data & 0x8000; }
	__forceinline void setSign(ui16 ispositive) { data |= ispositive; }

	void operator=(float f) {
		data = 0;
		ui32 n = *reinterpret_cast<ui32*>(&f);
		ui32 dn = (n & 0x80000000) >> 16;
		data = dn;
		dn = n & 0x0F800000 >> 13;
		data |= (short)dn;
		dn = n & 0x007FE000 >> 13;
		data |= (short)dn;
	}

	float toFloat() {
		ui32 fd = 0;
		fd |= getSign() << 16;
		fd |= (data & 0x03FF) << 13;
		fd |= (data & 0x7C00) >> 3;
		return *reinterpret_cast<float*>(fd);
	}
};

#ifdef CPUID_SSE
constexpr __m128i zero128 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr __m128i max128 = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

struct m128ui8 {
	union {
		__m128i v;
		ui8 data[16];
	};

	__forceinline m128ui8& operator+(const m128ui8& B) {
		m128ui8 r;
		r.v = _mm_add_epi8(v, B.v);
		return r;
	}

	__forceinline m128ui8& operator-(const m128ui8& B) {
		m128ui8 r;
		r.v = _mm_sub_epi8(v, B.v);
		return r;
	}

	__forceinline m128ui8& operator*(const m128ui8& B) {
		m128ui8 r;
		for (int i = 0; i < 8; ++i) {
			r.data[i] = v.m128i_i8[i] * B.data[i];
		}
	}

	__forceinline m128ui8& operator/(const m128ui8& B) {
		m128ui8 r;
		r.v = _mm_div_epu8(v, B.v);
		return r;
	}

	__forceinline m128ui8& operator%(const m128ui8& B) {
		m128ui8 r;
		m128ui8 temp;
		temp.v = _mm_div_epu8(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128ui8& operator&(const m128ui8& B) {
		m128ui8 r;
		r.v = _mm_and_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui8& operator|(const m128ui8& B) {
		m128ui8 r;
		r.v = _mm_or_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui8& operator^(const m128ui8& B) {
		m128ui8 r;
		r.v = _mm_xor_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui8& operator~() {
		m128ui8 r;
		r.v = _mm_xor_epi32(v, zero128);
		return r;
	}

	__forceinline m128ui8& operator<<(ui8 n) {
		m128ui8 r;
		ui8 c = ~((1 << n) - 1);
		r.v = _mm_slli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128ui8& operator>>(ui8 n) {
		m128ui8 r;
		ui8 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128ui8& operator<(const m128ui8& B) {
		constexpr m128ui8 oneData = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		constexpr m128ui8 lastzero = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F };
		m128ui8 A1, A2, A3;
		m128ui8 B1, B2, B3;
		m128ui8& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B;
		B1 = B1 >> 1;
		B2 = B;
		B2 = B2 & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm_cmpgt_epi8(A1.v, B1.v);
		A2.v = _mm_cmpgt_epi8(A2.v, B2.v);
		A3.v = _mm_cmpgt_epi8(A3.v, B3.v);
		A1.v = _mm_and_epi64(A1.v, A2.v);
		A3.v = _mm_or_epi64(A1.v, A3.v);
		return A3;
	}

	__forceinline m128ui8& operator>(const m128ui8& B) {
		m128ui8 B1;
		return B1 < *this;
	}

	__forceinline m128ui8& operator<=(const m128ui8& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128ui8& operator>=(const m128ui8& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128ui8& operator==(const m128ui8& B) {
		m128ui8& R = *this;
		m128ui8 r;
		r.v = _mm_cmpeq_epi8(R.v, B.v);
	}

	__forceinline m128ui8& operator!=(const m128ui8& B) {
		m128ui8 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}
};

struct m128si8 {
	union {
		__m128i v;
		si8 data[16];
	};

	__forceinline m128si8& operator+(const m128si8& B) {
		m128si8 r;
		r.v = _mm_add_epi8(v, B.v);
		return r;
	}

	__forceinline m128si8& operator-(const m128si8& B) {
		m128si8 r;
		r.v = _mm_sub_epi8(v, B.v);
		return r;
	}

	__forceinline m128si8& operator*(const m128si8& B) {
		m128ui8 r;
		for (int i = 0; i < 8; ++i) {
			r.data[i] = v.m128i_i8[i] * B.data[i];
		}
	}

	__forceinline m128si8& operator/(const m128si8& B) {
		m128si8 r;
		r.v = _mm_div_epu8(v, B.v);
		return r;
	}

	__forceinline m128si8& operator%(const m128si8& B) {
		m128si8 r;
		m128si8 temp;
		temp.v = _mm_div_epu8(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128si8& operator&(const m128si8& B) {
		m128si8 r;
		r.v = _mm_and_epi32(v, B.v);
		return r;
	}

	__forceinline m128si8& operator|(const m128si8& B) {
		m128si8 r;
		r.v = _mm_or_epi32(v, B.v);
		return r;
	}

	__forceinline m128si8& operator^(const m128si8& B) {
		m128si8 r;
		r.v = _mm_xor_epi32(v, B.v);
		return r;
	}

	__forceinline m128si8& operator~() {
		m128si8 r;
		r.v = _mm_xor_epi32(v, zero128);
		return r;
	}

	__forceinline m128si8& operator<<(ui8 n) {
		m128si8 r;
		ui8 c = ~((1 << n) - 1);
		r.v = _mm_slli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128si8& operator>>(ui8 n) {
		m128si8 r;
		ui8 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128si8& operator<(const m128si8& B) {
		m128si8 r;
		r.v = _mm_cmpgt_epi8(v, B.v);
		return r;
	}

	__forceinline m128si8& operator>(const m128si8& B) {
		m128si8 B1;
		return B1 < *this;
	}

	__forceinline m128si8& operator<=(const m128si8& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128si8& operator>=(const m128si8& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128si8& operator==(const m128si8& B) {
		m128si8& R = *this;
		m128si8 r;
		r.v = _mm_cmpeq_epi8(R.v, B.v);
	}

	__forceinline m128si8& operator!=(const m128si8& B) {
		m128si8 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}
};

struct m128ui16 {
	union {
		__m128i v;
		ui16 data[8];
	};

	__forceinline m128ui16& operator+(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_add_epi16(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator-(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_sub_epi16(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator*(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_mullo_epi16(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator/(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_div_epu16(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator%(const m128ui16& B) {
		m128ui16 r;
		m128ui16 temp;
		temp.v = _mm_div_epu16(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128ui16& operator&(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_and_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator|(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_or_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator^(const m128ui16& B) {
		m128ui16 r;
		r.v = _mm_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui16& operator~() {
		m128ui16 r;
		r.v = _mm_xor_epi64(v, zero128);
		return r;
	}

	__forceinline m128ui16& operator<<(ui8 n) {
		m128ui16 r;
		r.v = _mm_slli_epi16(v, n); // full shift
		return r;
	}

	__forceinline m128ui16& operator>>(ui8 n) {
		m128ui16 r;
		ui16 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_broadcastmw_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128ui16& operator<(const m128ui16& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m128ui16 oneData = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
		constexpr m128ui16 lastzero = { 0x00, 0x7F, 0x00, 0x7F, 0x00, 0x7F, 0x00, 0x7F,
			0x00, 0x7F, 0x00, 0x7F, 0x00, 0x7F, 0x00, 0x7F };
		m128ui16 A1, A2, A3;
		m128ui16 B1, B2, B3;
		m128ui16& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm_cmpgt_epi16(A1.v, B1.v);
		A2.v = _mm_cmpgt_epi16(A2.v, B2.v);
		A3.v = _mm_cmpgt_epi16(A3.v, B3.v);
		A1.v = _mm_and_epi64(A1.v, A2.v);
		A3.v = _mm_or_epi64(A1.v, A3.v);
		return A3;
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m128i halfData = { 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255 };
		__m128i A1, A2;
		__m128i B1, B2;
		A1 = _mm_unpacklo_epi16(v, zero128);
		A2 = _mm_unpackhi_epi16(v, zero128);
		B1 = _mm_unpacklo_epi16(B.v, zero128);
		B2 = _mm_unpackhi_epi16(B.v, zero128);
		A1 = _mm_cmpgt_epi32(A1, B1);
		A2 = _mm_cmpgt_epi32(A2, B2);
		m128ui16 r;
		r.v = _mm_or_epi64(_mm_and_epi64(A1, halfData), _mm_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m128ui16& operator>(const m128ui16& B) {
		m128ui16 B1 = B;
		return B1 < *this;
	}

	__forceinline m128ui16& operator<=(const m128ui16& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128ui16& operator>=(const m128ui16& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128ui16& operator==(const m128ui16& B) {
		m128ui16& R = *this;
		m128ui16 r;
		r.v = _mm_cmpeq_epi16(R.v, B.v);
	}

	__forceinline m128ui16& operator!=(const m128ui16& B) {
		m128ui16 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}

	__forceinline m128ui16& operator<<(const m128ui16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128ui16 r;
		r.v = _mm_sllv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m128ui16& operator>>(const m128ui16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128ui16 r, c, k;
		c.v = _mm_broadcastmw_epi32(8);
		c = c - n;
		k.v = _mm_broadcastmw_epi32(1);
		c = k << c;
		c = c - k;
		r.v = _mm_srlv_epi16(v, n.v);
		r = r & c;
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m128si16 {
	union {
		__m128i v;
		si16 data[8];
	};

	__forceinline m128si16& operator+(const m128si16& B) {
		m128si16 r;
		r.v = _mm_add_epi16(v, B.v);
		return r;
	}

	__forceinline m128si16& operator-(const m128si16& B) {
		m128si16 r;
		r.v = _mm_sub_epi16(v, B.v);
		return r;
	}

	__forceinline m128si16& operator*(const m128si16& B) {
		m128si16 r;
		r.v = _mm_mullo_epi16(v, B.v);
		return r;
	}

	__forceinline m128si16& operator/(const m128si16& B) {
		m128si16 r;
		r.v = _mm_div_epu16(v, B.v);
		return r;
	}

	__forceinline m128si16& operator%(const m128si16& B) {
		m128si16 r;
		m128si16 temp;
		temp.v = _mm_div_epu16(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128si16& operator&(const m128si16& B) {
		m128si16 r;
		r.v = _mm_and_epi64(v, B.v);
		return r;
	}

	__forceinline m128si16& operator|(const m128si16& B) {
		m128si16 r;
		r.v = _mm_or_epi64(v, B.v);
		return r;
	}

	__forceinline m128si16& operator^(const m128si16& B) {
		m128si16 r;
		r.v = _mm_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m128si16& operator~() {
		m128si16 r;
		r.v = _mm_xor_epi64(v, zero128);
		return r;
	}

	__forceinline m128si16& operator<<(ui8 n) {
		m128si16 r;
		r.v = _mm_slli_epi16(v, n); // full shift
		return r;
	}

	__forceinline m128si16& operator>>(ui8 n) {
		m128si16 r;
		ui16 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_broadcastmw_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128si16& operator<(const m128si16& B) {
		m128si16 r;
		r.v = _mm_cmpgt_epi16(v, B.v);
		return r;
	}

	__forceinline m128si16& operator>(const m128si16& B) {
		m128si16 r;
		r.v = _mm_cmplt_epi16(v, B.v);
		return r;
	}

	__forceinline m128si16& operator<=(const m128si16& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128si16& operator>=(const m128si16& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128si16& operator==(const m128si16& B) {
		m128si16& R = *this;
		m128si16 r;
		r.v = _mm_cmpeq_epi16(R.v, B.v);
	}

	__forceinline m128si16& operator!=(const m128si16& B) {
		m128si16 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}

	__forceinline m128si16& operator<<(const m128si16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128si16 r;
		r.v = _mm_sllv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m128si16& operator>>(const m128si16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128si16 r;
		r.v = _mm_srlv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m128ui32 {
	union {
		__m128i v;
		ui32 data[4];
	};

	__forceinline m128ui32& operator+(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_add_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator-(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_sub_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator*(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_mullo_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator/(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_div_epu32(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator%(const m128ui32& B) {
		m128ui32 r;
		m128ui32 temp;
		temp.v = _mm_div_epu32(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128ui32& operator&(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_and_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator|(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_or_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator^(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator~() {
		m128ui32 r;
		r.v = _mm_xor_epi64(v, zero128);
		return r;
	}

	__forceinline m128ui32& operator<<(ui8 n) {
		m128ui32 r;
		r.v = _mm_slli_epi32(v, n);
		return r;
	}

	__forceinline m128ui32& operator>>(ui8 n) {
		m128ui32 r;
		ui32 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128ui32& operator<(const m128ui32& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m128ui32 oneData = { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
		constexpr m128ui32 lastzero = { 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF };
		m128ui32 A1, A2, A3;
		m128ui32 B1, B2, B3;
		m128ui32& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm_cmpgt_epi32(A1.v, B1.v);
		A2.v = _mm_cmpgt_epi32(A2.v, B2.v);
		A3.v = _mm_cmpgt_epi32(A3.v, B3.v);
		A1.v = _mm_and_epi64(A1.v, A2.v);
		A3.v = _mm_or_epi64(A1.v, A3.v);
		return A3;
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m128i halfData = { 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255 };
		__m128i A1, A2;
		__m128i B1, B2;
		A1 = _mm_unpacklo_epi32(v, zero128);
		A2 = _mm_unpackhi_epi32(v, zero128);
		B1 = _mm_unpacklo_epi32(B.v, zero128);
		B2 = _mm_unpackhi_epi32(B.v, zero128);
		A1 = _mm_cmpgt_epi32(A1, B1);
		A2 = _mm_cmpgt_epi32(A2, B2);
		m128ui32 r;
		r.v = _mm_or_epi64(_mm_and_epi64(A1, halfData), _mm_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m128ui32& operator>(const m128ui32& B) {
		m128ui32 r;
		r.v = _mm_cmplt_epi32(v, B.v);
		return r;
	}

	__forceinline m128ui32& operator<=(const m128ui32& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128ui32& operator>=(const m128ui32& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128ui32& operator==(const m128ui32& B) {
		m128ui32& R = *this;
		m128ui32 r;
		r.v = _mm_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m128ui32& operator!=(const m128ui32& B) {
		m128ui32 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}

	__forceinline m128ui32& operator<<(const m128ui32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128ui32 r;
		r.v = _mm_sllv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m128ui32& operator>>(const m128ui32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128ui32 r;
		r.v = _mm_srlv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m128si32 {
	union {
		__m128i v;
		si32 data[4];
	};

	__forceinline m128si32& operator+(const m128si32& B) {
		m128si32 r;
		r.v = _mm_add_epi32(v, B.v);
		return r;
	}

	__forceinline m128si32& operator-(const m128si32& B) {
		m128si32 r;
		r.v = _mm_sub_epi32(v, B.v);
		return r;
	}

	__forceinline m128si32& operator*(const m128si32& B) {
		m128si32 r;
		r.v = _mm_mullo_epi32(v, B.v);
		return r;
	}

	__forceinline m128si32& operator/(const m128si32& B) {
		m128si32 r;
		r.v = _mm_div_epi32(v, B.v);
		return r;
	}

	__forceinline m128si32& operator%(const m128si32& B) {
		m128si32 r;
		m128si32 temp;
		temp.v = _mm_div_epi32(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128si32& operator&(const m128si32& B) {
		m128si32 r;
		r.v = _mm_and_epi64(v, B.v);
		return r;
	}

	__forceinline m128si32& operator|(const m128si32& B) {
		m128si32 r;
		r.v = _mm_or_epi64(v, B.v);
		return r;
	}

	__forceinline m128si32& operator^(const m128si32& B) {
		m128si32 r;
		r.v = _mm_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m128si32& operator~() {
		m128si32 r;
		r.v = _mm_xor_epi64(v, zero128);
		return r;
	}

	__forceinline m128si32& operator<<(ui8 n) {
		m128si32 r;
		r.v = _mm_slli_epi32(v, n);
		return r;
	}

	__forceinline m128si32& operator>>(ui8 n) {
		m128si32 r;
		r.v = _mm_srli_epi32(v, n);
		return r;
	}

	__forceinline m128si32& operator<(const m128si32& B) {
		m128si32 r;
		r.v = _mm_cmpgt_epi32(B.v, v);
		return r;
	}

	__forceinline m128si32& operator>(const m128si32& B) {
		m128si32 r;
		r.v = _mm_cmpgt_epi32(v, B.v);
		return r;
	}

	__forceinline m128si32& operator<=(const m128si32& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128si32& operator>=(const m128si32& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128si32& operator==(const m128si32& B) {
		m128si32& R = *this;
		m128si32 r;
		r.v = _mm_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m128si32& operator!=(const m128si32& B) {
		m128si32 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}

	__forceinline m128si32& operator<<(const m128si32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128si32 r;
		r.v = _mm_sllv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 4; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m128si32& operator>>(const m128si32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128si32 r;
		r.v = _mm_srlv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 4; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m128ui64 {
	union {
		__m128i v;
		ui64 data[2];
	};

	__forceinline m128ui64& operator+(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_add_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator-(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_sub_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator*(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_mullo_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator/(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_div_epu64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator%(const m128ui64& B) {
		m128ui64 r;
		m128ui64 temp;
		temp.v = _mm_div_epu64(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128ui64& operator&(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_and_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator|(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_or_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator^(const m128ui64& B) {
		m128ui64 r;
		r.v = _mm_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m128ui64& operator~() {
		m128ui64 r;
		r.v = _mm_xor_epi64(v, zero128);
		return r;
	}

	__forceinline m128ui64& operator<<(ui8 n) {
		m128ui64 r;
		r.v = _mm_slli_epi64(v, n);
		return r;
	}

	__forceinline m128ui64& operator>>(ui8 n) {
		m128ui64 r;
		ui64 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128ui64& operator<(const m128ui64& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m128ui64 oneData = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
		constexpr m128ui64 lastzero = { 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		m128ui64 A1, A2, A3;
		m128ui64 B1, B2, B3;
		m128ui64& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm_cmpgt_epi64(A1.v, B1.v);
		A2.v = _mm_cmpgt_epi64(A2.v, B2.v);
		A3.v = _mm_cmpgt_epi64(A3.v, B3.v);
		A1.v = _mm_and_epi64(A1.v, A2.v);
		A3.v = _mm_or_epi64(A1.v, A3.v);
		return A3;
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m128i halfData = { 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255 };
		__m128i A1, A2;
		__m128i B1, B2;
		A1 = _mm_unpacklo_epi64(v, zero128);
		A2 = _mm_unpackhi_epi64(v, zero128);
		B1 = _mm_unpacklo_epi64(B.v, zero128);
		B2 = _mm_unpackhi_epi64(B.v, zero128);
		A1 = _mm_cmpgt_epi32(A1, B1);
		A2 = _mm_cmpgt_epi32(A2, B2);
		m128ui64 r;
		r.v = _mm_or_epi64(_mm_and_epi64(A1, halfData), _mm_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m128ui64& operator>(const m128ui64& B) {
		m128ui64 r = B;
		r = r < *this;
		return r;
	}

	__forceinline m128ui64& operator<=(const m128ui64& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128ui64& operator>=(const m128ui64& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128ui64& operator==(const m128ui64& B) {
		m128ui64& R = *this;
		m128ui64 r;
		r.v = _mm_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m128ui64& operator!=(const m128ui64& B) {
		m128ui64 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}

	__forceinline m128ui64& operator<<(const m128ui64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128ui64 r;
		r.v = _mm_sllv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m128ui64& operator>>(const m128ui64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128ui64 r;
		m128ui64 co;
		constexpr m128ui64 one = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
		constexpr m128ui64 eight = { 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8 };
		// ((1 << (8-n)) - 1)
		r = eight;
		r = r - n;
		co = one;
		r = co << r;
		co = r - one;
		r.v = _mm_srlv_epi64(v, n.v);
		r.v = _mm_and_epi64(r.v, co.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m128si64 {
	union {
		__m128i v;
		si64 data[2];
	};

	__forceinline m128si64& operator+(const m128si64& B) {
		m128si64 r;
		r.v = _mm_add_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator-(const m128si64& B) {
		m128si64 r;
		r.v = _mm_sub_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator*(const m128si64& B) {
		m128si64 r;
		r.v = _mm_mullo_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator/(const m128si64& B) {
		m128si64 r;
		r.v = _mm_div_epu64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator%(const m128si64& B) {
		m128si64 r;
		m128si64 temp;
		temp.v = _mm_div_epu64(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128si64& operator&(const m128si64& B) {
		m128si64 r;
		r.v = _mm_and_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator|(const m128si64& B) {
		m128si64 r;
		r.v = _mm_or_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator^(const m128si64& B) {
		m128si64 r;
		r.v = _mm_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator~() {
		m128si64 r;
		r.v = _mm_xor_epi64(v, zero128);
		return r;
	}

	__forceinline m128si64& operator<<(ui8 n) {
		m128si64 r;
		r.v = _mm_slli_epi64(v, n);
		return r;
	}

	__forceinline m128si64& operator>>(ui8 n) {
		m128si64 r;
		ui64 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm_srli_epi64(v, n); // full shift
		r.v = _mm_and_epi64(r.v, _mm_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m128si64& operator<(const m128si64& B) {
		m128si64 r;
		r.v = _mm_cmpgt_epi64(B.v, v);
		return r;
	}

	__forceinline m128si64& operator>(const m128si64& B) {
		m128si64 r;
		r.v = _mm_cmpgt_epi64(v, B.v);
		return r;
	}

	__forceinline m128si64& operator<=(const m128si64& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m128si64& operator>=(const m128si64& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m128si64& operator==(const m128si64& B) {
		m128si64& R = *this;
		m128si64 r;
		r.v = _mm_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m128si64& operator!=(const m128si64& B) {
		m128si64 r = *this == B;
		r.v = _mm_xor_epi64(r.v, zero128);
		return r;
	}

	__forceinline m128si64& operator<<(const m128si64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128si64 r;
		r.v = _mm_sllv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m128si64& operator>>(const m128si64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m128si64 r;
		r.v = _mm_srlv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m128d {
	union {
		__m128d v;
		__m128i iv;
		double data[2];
	};

	__forceinline m128d& operator+(const m128d& B) {
		m128d r;
		r.v = _mm_add_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator-(const m128d& B) {
		m128d r;
		r.v = _mm_sub_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator*(const m128d& B) {
		m128d r;
		r.v = _mm_mul_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator/(const m128d& B) {
		m128d r;
		r.v = _mm_div_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator%(const m128d& B) {
		m128d r;
		m128d temp;
		temp.v = _mm_div_pd(v, B.v);
		temp.v = _mm_floor_pd(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128d& operator&(const m128d& B) {
		m128d r;
		r.v = _mm_and_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator|(const m128d& B) {
		m128d r;
		r.v = _mm_or_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator^(const m128d& B) {
		m128d r;
		r.v = _mm_xor_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator~() {
		m128d r;
		m128d aa;
		aa.iv = zero128;
		r.v = _mm_xor_pd(v, aa.v);
		return r;
	}

	__forceinline m128d& operator<(const m128d& B) {
		m128d r;
		r.v = _mm_cmpgt_pd(B.v, v);
		return r;
	}

	__forceinline m128d& operator>(const m128d& B) {
		m128d r;
		r.v = _mm_cmpgt_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator<=(const m128d& B) {
		m128d r;
		r.v = _mm_cmpge_pd(B.v, v);
		return r;
	}

	__forceinline m128d& operator>=(const m128d& B) {
		m128d r;
		r.v = _mm_cmpge_pd(v, B.v);
		return r;
	}

	__forceinline m128d& operator==(const m128d& B) {
		m128d& R = *this;
		m128d r;
		r.v = _mm_cmpeq_pd(R.v, B.v);
		return r;
	}

	__forceinline m128d& operator!=(const m128d& B) {
		m128d& R = *this;
		m128d r;
		r.v = _mm_cmpneq_pd(R.v, B.v);
		return r;
	}
};

struct m128f {
	union {
		__m128 v;
		__m128i iv;
		float data[4];
	};

	__forceinline m128f& operator+(const m128f& B) {
		m128f r;
		r.v = _mm_add_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator-(const m128f& B) {
		m128f r;
		r.v = _mm_sub_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator*(const m128f& B) {
		m128f r;
		r.v = _mm_mul_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator/(const m128f& B) {
		m128f r;
		r.v = _mm_div_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator%(const m128f& B) {
		m128f r;
		m128f temp;
		temp.v = _mm_div_ps(v, B.v);
		temp.v = _mm_floor_ps(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m128f& operator&(const m128f& B) {
		m128f r;
		r.v = _mm_and_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator|(const m128f& B) {
		m128f r;
		r.v = _mm_or_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator^(const m128f& B) {
		m128f r;
		r.v = _mm_xor_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator~() {
		m128f r;
		m128f z;
		z.iv = zero128;
		r.v = _mm_xor_ps(v, z.v);
		return r;
	}

	__forceinline m128f& operator<(const m128f& B) {
		m128f r;
		r.v = _mm_cmpgt_ps(B.v, v);
		return r;
	}

	__forceinline m128f& operator>(const m128f& B) {
		m128f r;
		r.v = _mm_cmpgt_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator<=(const m128f& B) {
		m128f r;
		r.v = _mm_cmpge_ps(B.v, v);
		return r;
	}

	__forceinline m128f& operator>=(const m128f& B) {
		m128f r;
		r.v = _mm_cmpge_ps(v, B.v);
		return r;
	}

	__forceinline m128f& operator==(const m128f& B) {
		m128f& R = *this;
		m128f r;
		r.v = _mm_cmpeq_ps(R.v, B.v);
		return r;
	}

	__forceinline m128f& operator!=(const m128f& B) {
		m128f& R = *this;
		m128f r;
		r.v = _mm_cmpneq_ps(R.v, B.v);
		return r;
	}
};

struct m128hf {
	union {
		__m128h v;
		__m128 sv;
		__m128i iv;
		hfloat data[8];
	};

	__forceinline m128hf& operator+(const m128hf& B) {
		m128hf r;
		r.v = _mm_add_ph(v, B.v);
		return r;
	}

	__forceinline m128hf& operator-(const m128hf& B) {
		m128hf r;
		r.v = _mm_sub_ph(v, B.v);
		return r;
	}

	__forceinline m128hf& operator*(const m128hf& B) {
		m128hf r;
		r.v = _mm_mul_ph(v, B.v);
		return r;
	}

	__forceinline m128hf& operator/(const m128hf& B) {
		m128hf r;
		r.v = _mm_div_ph(v, B.v);
		return r;
	}

	/*__forceinline m128hf& operator%(const m128hf& B) {
		m128hf r;
		m128hf temp;
		temp.v = _mm_div_ph(v, B.v);
		temp.v = _mm_floor_pd(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}*/
	//_mm_floor_ph() is not exist.

	__forceinline m128hf& operator&(const m128hf& B) {
		m128hf r;
		r.sv = _mm_and_ps(sv, B.sv);
		return r;
	}

	__forceinline m128hf& operator|(const m128hf& B) {
		m128hf r;
		r.sv = _mm_or_ps(sv, B.sv);
		return r;
	}

	__forceinline m128hf& operator^(const m128hf& B) {
		m128hf r;
		r.sv = _mm_xor_ps(sv, B.sv);
		return r;
	}

	__forceinline m128hf& operator~() {
		m128hf r;
		m128hf aa;
		aa.iv = zero128;
		r.sv = _mm_xor_ps(sv, aa.sv);
		return r;
	}

	//_mm_cmpgt_ph function is doesn't exist.
	// so later, it have to made with simd subtraction and inline simd toFloat operation of hfloat and pack/unpack operation
	/*__forceinline m128hf& operator<(const m128hf& B) {
		m128hf r;
		r.v = _mm_cmpgt_ph(B.v, v);
		return r;
	}

	__forceinline m128hf& operator>(const m128hf& B) {
		m128hf r;
		r.sv = _mm_cmpgt_ps(sv, B.sv);
		return r;
	}

	__forceinline m128hf& operator<=(const m128hf& B) {
		m128hf r;
		r.v = _mm_cmpge_ps(B.v, v);
		return r;
	}

	__forceinline m128hf& operator>=(const m128hf& B) {
		m128hf r;
		r.v = _mm_cmpge_ps(v, B.v);
		return r;
	}

	__forceinline m128hf& operator==(const m128hf& B) {
		m128hf& R = *this;
		m128hf r;
		r.v = _mm_cmpeq_ps(R.v, B.v);
		return r;
	}

	__forceinline m128hf& operator!=(const m128hf& B) {
		m128hf& R = *this;
		m128hf r;
		r.v = _mm_cmpneq_ps(R.v, B.v);
		return r;
	}*/
};

union M128 {
	ui8 d8[16];
	__m128 simd;
	__m128i nums;
	__m128d d_real;
	__m128h hf_real;
	m128ui8 _ui8;
	m128si8 _si8;
	m128ui16 _ui16;
	m128si16 _si16;
	m128ui32 _ui32;
	m128si32 _si32;
	m128ui64 _ui64;
	m128si64 _si64;
	m128d _d64;
	m128f _f32;
	m128hf _hf16;
};

#endif

//AVX CMP CONSTANT EXPLANE
/*
* https://stackoverflow.com/questions/16988199/how-to-choose-avx-compare-predicate-variants
O : ordered - if parameter is Nan, return true
U : unorderd - if parameter is Nan, return false
Q : None Signaling
S : Signaling - if parameter is Nan, throw excaption
* CASE (imm8[4:0]) OF
0: OP := _CMP_EQ_OQ
1: OP := _CMP_LT_OS
2: OP := _CMP_LE_OS
3: OP := _CMP_UNORD_Q
4: OP := _CMP_NEQ_UQ
5: OP := _CMP_NLT_US
6: OP := _CMP_NLE_US
7: OP := _CMP_ORD_Q
8: OP := _CMP_EQ_UQ
9: OP := _CMP_NGE_US
10: OP := _CMP_NGT_US
11: OP := _CMP_FALSE_OQ
12: OP := _CMP_NEQ_OQ
13: OP := _CMP_GE_OS
14: OP := _CMP_GT_OS
15: OP := _CMP_TRUE_UQ
16: OP := _CMP_EQ_OS
17: OP := _CMP_LT_OQ
18: OP := _CMP_LE_OQ
19: OP := _CMP_UNORD_S
20: OP := _CMP_NEQ_US
21: OP := _CMP_NLT_UQ
22: OP := _CMP_NLE_UQ
23: OP := _CMP_ORD_S
24: OP := _CMP_EQ_US
25: OP := _CMP_NGE_UQ
26: OP := _CMP_NGT_UQ
27: OP := _CMP_FALSE_OS
28: OP := _CMP_NEQ_OS
29: OP := _CMP_GE_OQ
30: OP := _CMP_GT_OQ
31: OP := _CMP_TRUE_US
*/

#ifdef CPUID_AVX

constexpr __m256i zero256 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr __m256i max256 = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

struct m256ui8 {
	union {
		__m256i v;
		ui8 data[32];
	};

	__forceinline m256ui8& operator+(const m256ui8& B) {
		m256ui8 r;
		r.v = _mm256_add_epi8(v, B.v);
		return r;
	}

	__forceinline m256ui8& operator-(const m256ui8& B) {
		m256ui8 r;
		r.v = _mm256_sub_epi8(v, B.v);
		return r;
	}

	__forceinline m256ui8& operator*(const m256ui8& B) {
		m256ui8 r;
		for (int i = 0; i < 8; ++i) {
			r.data[i] = v.m256i_i8[i] * B.data[i];
		}
	}

	__forceinline m256ui8& operator/(const m256ui8& B) {
		m256ui8 r;
		r.v = _mm256_div_epu8(v, B.v);
		return r;
	}

	__forceinline m256ui8& operator%(const m256ui8& B) {
		m256ui8 r;
		m256ui8 temp;
		temp.v = _mm256_div_epu8(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256ui8& operator&(const m256ui8& B) {
		m256ui8 r;
		r.v = _mm256_and_epi32(v, B.v);
		return r;
	}

	__forceinline m256ui8& operator|(const m256ui8& B) {
		m256ui8 r;
		r.v = _mm256_or_epi32(v, B.v);
		return r;
	}

	__forceinline m256ui8& operator^(const m256ui8& B) {
		m256ui8 r;
		r.v = _mm256_xor_epi32(v, B.v);
		return r;
	}

	__forceinline m256ui8& operator~() {
		m256ui8 r;
		r.v = _mm256_xor_epi32(v, zero256);
		return r;
	}

	__forceinline m256ui8& operator<<(ui8 n) {
		m256ui8 r;
		ui8 c = ~((1 << n) - 1);
		r.v = _mm256_slli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256ui8& operator>>(ui8 n) {
		m256ui8 r;
		ui8 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256ui8& operator<(const m256ui8& B) {
		constexpr m256ui8 oneData = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		constexpr m256ui8 lastzero = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F };
		m256ui8 A1, A2, A3;
		m256ui8 B1, B2, B3;
		m256ui8& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B;
		B1 = B1 >> 1;
		B2 = B;
		B2 = B2 & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm256_cmpgt_epi8(A1.v, B1.v);
		A2.v = _mm256_cmpgt_epi8(A2.v, B2.v);
		A3.v = _mm256_cmpgt_epi8(A3.v, B3.v);
		A1.v = _mm256_and_epi64(A1.v, A2.v);
		A3.v = _mm256_or_epi64(A1.v, A3.v);
		return A3;
	}

	__forceinline m256ui8& operator>(const m256ui8& B) {
		m256ui8 B1;
		return B1 < *this;
	}

	__forceinline m256ui8& operator<=(const m256ui8& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256ui8& operator>=(const m256ui8& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256ui8& operator==(const m256ui8& B) {
		m256ui8& R = *this;
		m256ui8 r;
		r.v = _mm256_cmpeq_epi8(R.v, B.v);
	}

	__forceinline m256ui8& operator!=(const m256ui8& B) {
		m256ui8 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}
};

struct m256si8 {
	union {
		__m256i v;
		si8 data[32];
	};

	__forceinline m256si8& operator+(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_add_epi8(v, B.v);
		return r;
	}

	__forceinline m256si8& operator-(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_sub_epi8(v, B.v);
		return r;
	}

	__forceinline m256si8& operator*(const m256si8& B) {
		m256ui8 r;
		for (int i = 0; i < 8; ++i) {
			r.data[i] = v.m256i_i8[i] * B.data[i];
		}
	}

	__forceinline m256si8& operator/(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_div_epu8(v, B.v);
		return r;
	}

	__forceinline m256si8& operator%(const m256si8& B) {
		m256si8 r;
		m256si8 temp;
		temp.v = _mm256_div_epu8(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256si8& operator&(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_and_epi32(v, B.v);
		return r;
	}

	__forceinline m256si8& operator|(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_or_epi32(v, B.v);
		return r;
	}

	__forceinline m256si8& operator^(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_xor_epi32(v, B.v);
		return r;
	}

	__forceinline m256si8& operator~() {
		m256si8 r;
		r.v = _mm256_xor_epi32(v, zero256);
		return r;
	}

	__forceinline m256si8& operator<<(ui8 n) {
		m256si8 r;
		ui8 c = ~((1 << n) - 1);
		r.v = _mm256_slli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256si8& operator>>(ui8 n) {
		m256si8 r;
		ui8 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256si8& operator<(const m256si8& B) {
		m256si8 r;
		r.v = _mm256_cmpgt_epi8(v, B.v);
		return r;
	}

	__forceinline m256si8& operator>(const m256si8& B) {
		m256si8 B1;
		return B1 < *this;
	}

	__forceinline m256si8& operator<=(const m256si8& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256si8& operator>=(const m256si8& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256si8& operator==(const m256si8& B) {
		m256si8& R = *this;
		m256si8 r;
		r.v = _mm256_cmpeq_epi8(R.v, B.v);
	}

	__forceinline m256si8& operator!=(const m256si8& B) {
		m256si8 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}
};

struct m256ui16 {
	union {
		__m256i v;
		ui16 data[16];
	};

	__forceinline m256ui16& operator+(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_add_epi16(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator-(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_sub_epi16(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator*(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_mullo_epi16(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator/(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_div_epu16(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator%(const m256ui16& B) {
		m256ui16 r;
		m256ui16 temp;
		temp.v = _mm256_div_epu16(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256ui16& operator&(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_and_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator|(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_or_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator^(const m256ui16& B) {
		m256ui16 r;
		r.v = _mm256_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui16& operator~() {
		m256ui16 r;
		r.v = _mm256_xor_epi64(v, zero256);
		return r;
	}

	__forceinline m256ui16& operator<<(ui8 n) {
		m256ui16 r;
		r.v = _mm256_slli_epi16(v, n); // full shift
		return r;
	}

	__forceinline m256ui16& operator>>(ui8 n) {
		m256ui16 r;
		ui16 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_broadcastmw_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256ui16& operator<(const m256ui16& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m256ui16 oneData = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
		constexpr m256ui16 lastzero = { 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
			0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
			0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
			0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF };
		m256ui16 A1, A2, A3;
		m256ui16 B1, B2, B3;
		m256ui16& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm256_cmpgt_epi16(A1.v, B1.v);
		A2.v = _mm256_cmpgt_epi16(A2.v, B2.v);
		A3.v = _mm256_cmpgt_epi16(A3.v, B3.v);
		A1.v = _mm256_and_epi64(A1.v, A2.v);
		A3.v = _mm256_or_epi64(A1.v, A3.v);
		return A3;
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m256i halfData = { 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255,
			0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255 };
		__m256i A1, A2;
		__m256i B1, B2;
		A1 = _mm256_unpacklo_epi16(v, zero256);
		A2 = _mm256_unpackhi_epi16(v, zero256);
		B1 = _mm256_unpacklo_epi16(B.v, zero256);
		B2 = _mm256_unpackhi_epi16(B.v, zero256);
		A1 = _mm256_cmpgt_epi32(A1, B1);
		A2 = _mm256_cmpgt_epi32(A2, B2);
		m256ui16 r;
		r.v = _mm256_or_epi64(_mm256_and_epi64(A1, halfData), _mm256_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m256ui16& operator>(const m256ui16& B) {
		m256ui16 B1 = B;
		return B1 < *this;
	}

	__forceinline m256ui16& operator<=(const m256ui16& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256ui16& operator>=(const m256ui16& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256ui16& operator==(const m256ui16& B) {
		m256ui16& R = *this;
		m256ui16 r;
		r.v = _mm256_cmpeq_epi16(R.v, B.v);
	}

	__forceinline m256ui16& operator!=(const m256ui16& B) {
		m256ui16 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}

	__forceinline m256ui16& operator<<(const m256ui16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256ui16 r;
		r.v = _mm256_sllv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m256ui16& operator>>(const m256ui16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256ui16 r, c, k;
		c.v = _mm256_broadcastmw_epi32(8);
		c = c - n;
		k.v = _mm256_broadcastmw_epi32(1);
		c = k << c;
		c = c - k;
		r.v = _mm256_srlv_epi16(v, n.v);
		r = r & c;
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m256si16 {
	union {
		__m256i v;
		si16 data[16];
	};

	__forceinline m256si16& operator+(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_add_epi16(v, B.v);
		return r;
	}

	__forceinline m256si16& operator-(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_sub_epi16(v, B.v);
		return r;
	}

	__forceinline m256si16& operator*(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_mullo_epi16(v, B.v);
		return r;
	}

	__forceinline m256si16& operator/(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_div_epu16(v, B.v);
		return r;
	}

	__forceinline m256si16& operator%(const m256si16& B) {
		m256si16 r;
		m256si16 temp;
		temp.v = _mm256_div_epu16(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256si16& operator&(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_and_epi64(v, B.v);
		return r;
	}

	__forceinline m256si16& operator|(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_or_epi64(v, B.v);
		return r;
	}

	__forceinline m256si16& operator^(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m256si16& operator~() {
		m256si16 r;
		r.v = _mm256_xor_epi64(v, zero256);
		return r;
	}

	__forceinline m256si16& operator<<(ui8 n) {
		m256si16 r;
		r.v = _mm256_slli_epi16(v, n); // full shift
		return r;
	}

	__forceinline m256si16& operator>>(ui8 n) {
		m256si16 r;
		ui16 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_broadcastmw_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256si16& operator<(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_cmpgt_epi16(v, B.v);
		return r;
	}

	__forceinline m256si16& operator>(const m256si16& B) {
		m256si16 r;
		r.v = _mm256_cmpgt_epi16(B.v, v);
		return r;
	}

	__forceinline m256si16& operator<=(const m256si16& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256si16& operator>=(const m256si16& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256si16& operator==(const m256si16& B) {
		m256si16& R = *this;
		m256si16 r;
		r.v = _mm256_cmpeq_epi16(R.v, B.v);
	}

	__forceinline m256si16& operator!=(const m256si16& B) {
		m256si16 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}

	__forceinline m256si16& operator<<(const m256si16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256si16 r;
		r.v = _mm256_sllv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m256si16& operator>>(const m256si16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256si16 r;
		r.v = _mm256_srlv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m256ui32 {
	union {
		__m256i v;
		ui32 data[8];
	};

	__forceinline m256ui32& operator+(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_add_epi32(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator-(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_sub_epi32(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator*(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_mullo_epi32(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator/(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_div_epu32(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator%(const m256ui32& B) {
		m256ui32 r;
		m256ui32 temp;
		temp.v = _mm256_div_epu32(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256ui32& operator&(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_and_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator|(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_or_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator^(const m256ui32& B) {
		m256ui32 r;
		r.v = _mm256_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui32& operator~() {
		m256ui32 r;
		r.v = _mm256_xor_epi64(v, zero256);
		return r;
	}

	__forceinline m256ui32& operator<<(ui8 n) {
		m256ui32 r;
		r.v = _mm256_slli_epi32(v, n);
		return r;
	}

	__forceinline m256ui32& operator>>(ui8 n) {
		m256ui32 r;
		ui32 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256ui32& operator<(const m256ui32& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m256ui32 oneData = { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
			0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
		constexpr m256ui32 lastzero = { 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF };
		m256ui32 A1, A2, A3;
		m256ui32 B1, B2, B3;
		m256ui32& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm256_cmpgt_epi32(A1.v, B1.v);
		A2.v = _mm256_cmpgt_epi32(A2.v, B2.v);
		A3.v = _mm256_cmpgt_epi32(A3.v, B3.v);
		A1.v = _mm256_and_epi64(A1.v, A2.v);
		A3.v = _mm256_or_epi64(A1.v, A3.v);
		return A3;
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m256i halfData = { 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255,
			0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255 };
		__m256i A1, A2;
		__m256i B1, B2;
		A1 = _mm256_unpacklo_epi32(v, zero256);
		A2 = _mm256_unpackhi_epi32(v, zero256);
		B1 = _mm256_unpacklo_epi32(B.v, zero256);
		B2 = _mm256_unpackhi_epi32(B.v, zero256);
		A1 = _mm256_cmpgt_epi32(A1, B1);
		A2 = _mm256_cmpgt_epi32(A2, B2);
		m256ui32 r;
		r.v = _mm256_or_epi64(_mm256_and_epi64(A1, halfData), _mm256_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m256ui32& operator>(const m256ui32& B) {
		m256ui32 r = B;
		return (r < *this);
	}

	__forceinline m256ui32& operator<=(const m256ui32& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256ui32& operator>=(const m256ui32& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256ui32& operator==(const m256ui32& B) {
		m256ui32& R = *this;
		m256ui32 r;
		r.v = _mm256_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m256ui32& operator!=(const m256ui32& B) {
		m256ui32 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}

	__forceinline m256ui32& operator<<(const m256ui32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256ui32 r;
		r.v = _mm256_sllv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m256ui32& operator>>(const m256ui32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256ui32 r;
		r.v = _mm256_srlv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m256si32 {
	union {
		__m256i v;
		si32 data[8];
	};

	__forceinline m256si32& operator+(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_add_epi32(v, B.v);
		return r;
	}

	__forceinline m256si32& operator-(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_sub_epi32(v, B.v);
		return r;
	}

	__forceinline m256si32& operator*(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_mullo_epi32(v, B.v);
		return r;
	}

	__forceinline m256si32& operator/(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_div_epi32(v, B.v);
		return r;
	}

	__forceinline m256si32& operator%(const m256si32& B) {
		m256si32 r;
		m256si32 temp;
		temp.v = _mm256_div_epi32(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256si32& operator&(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_and_epi64(v, B.v);
		return r;
	}

	__forceinline m256si32& operator|(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_or_epi64(v, B.v);
		return r;
	}

	__forceinline m256si32& operator^(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m256si32& operator~() {
		m256si32 r;
		r.v = _mm256_xor_epi64(v, zero256);
		return r;
	}

	__forceinline m256si32& operator<<(ui8 n) {
		m256si32 r;
		r.v = _mm256_slli_epi32(v, n);
		return r;
	}

	__forceinline m256si32& operator>>(ui8 n) {
		m256si32 r;
		r.v = _mm256_srli_epi32(v, n);
		return r;
	}

	__forceinline m256si32& operator<(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_cmpgt_epi32(B.v, v);
		return r;
	}

	__forceinline m256si32& operator>(const m256si32& B) {
		m256si32 r;
		r.v = _mm256_cmpgt_epi32(v, B.v);
		return r;
	}

	__forceinline m256si32& operator<=(const m256si32& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256si32& operator>=(const m256si32& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256si32& operator==(const m256si32& B) {
		m256si32& R = *this;
		m256si32 r;
		r.v = _mm256_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m256si32& operator!=(const m256si32& B) {
		m256si32 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}

	__forceinline m256si32& operator<<(const m256si32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256si32 r;
		r.v = _mm256_sllv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 4; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m256si32& operator>>(const m256si32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256si32 r;
		r.v = _mm256_srlv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 4; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m256ui64 {
	union {
		__m256i v;
		ui64 data[4];
	};

	__forceinline m256ui64& operator+(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_add_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator-(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_sub_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator*(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_mullo_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator/(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_div_epu64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator%(const m256ui64& B) {
		m256ui64 r;
		m256ui64 temp;
		temp.v = _mm256_div_epu64(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256ui64& operator&(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_and_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator|(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_or_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator^(const m256ui64& B) {
		m256ui64 r;
		r.v = _mm256_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m256ui64& operator~() {
		m256ui64 r;
		r.v = _mm256_xor_epi64(v, zero256);
		return r;
	}

	__forceinline m256ui64& operator<<(ui8 n) {
		m256ui64 r;
		r.v = _mm256_slli_epi64(v, n);
		return r;
	}

	__forceinline m256ui64& operator>>(ui8 n) {
		m256ui64 r;
		ui64 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256ui64& operator<(const m256ui64& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m256ui64 oneData = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
			0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
		constexpr m256ui64 lastzero = { 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		m256ui64 A1, A2, A3;
		m256ui64 B1, B2, B3;
		m256ui64& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		A1.v = _mm256_cmpgt_epi64(A1.v, B1.v);
		A2.v = _mm256_cmpgt_epi64(A2.v, B2.v);
		A3.v = _mm256_cmpgt_epi64(A3.v, B3.v);
		A1.v = _mm256_and_epi64(A1.v, A2.v);
		A3.v = _mm256_or_epi64(A1.v, A3.v);
		return A3;
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m256i halfData = { 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255,
			0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255 };
		__m256i A1, A2;
		__m256i B1, B2;
		A1 = _mm256_unpacklo_epi64(v, zero256);
		A2 = _mm256_unpackhi_epi64(v, zero256);
		B1 = _mm256_unpacklo_epi64(B.v, zero256);
		B2 = _mm256_unpackhi_epi64(B.v, zero256);
		A1 = _mm256_cmpgt_epi32(A1, B1);
		A2 = _mm256_cmpgt_epi32(A2, B2);
		m256ui64 r;
		r.v = _mm256_or_epi64(_mm256_and_epi64(A1, halfData), _mm256_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m256ui64& operator>(const m256ui64& B) {
		m256ui64 r = B;
		r = r < *this;
		return r;
	}

	__forceinline m256ui64& operator<=(const m256ui64& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256ui64& operator>=(const m256ui64& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256ui64& operator==(const m256ui64& B) {
		m256ui64& R = *this;
		m256ui64 r;
		r.v = _mm256_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m256ui64& operator!=(const m256ui64& B) {
		m256ui64 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}

	__forceinline m256ui64& operator<<(const m256ui64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256ui64 r;
		r.v = _mm256_sllv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m256ui64& operator>>(const m256ui64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256ui64 r;
		m256ui64 co;
		constexpr m256ui64 one = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
			0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
		constexpr m256ui64 eight = { 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8,
			0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8 };
		// ((1 << (8-n)) - 1)
		r = eight;
		r = r - n;
		co = one;
		r = co << r;
		co = r - one;
		r.v = _mm256_srlv_epi64(v, n.v);
		r.v = _mm256_and_epi64(r.v, co.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m256si64 {
	union {
		__m256i v;
		si64 data[4];
	};

	__forceinline m256si64& operator+(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_add_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator-(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_sub_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator*(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_mullo_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator/(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_div_epu64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator%(const m256si64& B) {
		m256si64 r;
		m256si64 temp;
		temp.v = _mm256_div_epu64(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256si64& operator&(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_and_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator|(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_or_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator^(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator~() {
		m256si64 r;
		r.v = _mm256_xor_epi64(v, zero256);
		return r;
	}

	__forceinline m256si64& operator<<(ui8 n) {
		m256si64 r;
		r.v = _mm256_slli_epi64(v, n);
		return r;
	}

	__forceinline m256si64& operator>>(ui8 n) {
		m256si64 r;
		ui64 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm256_srli_epi64(v, n); // full shift
		r.v = _mm256_and_epi64(r.v, _mm256_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m256si64& operator<(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_cmpgt_epi64(B.v, v);
		return r;
	}

	__forceinline m256si64& operator>(const m256si64& B) {
		m256si64 r;
		r.v = _mm256_cmpgt_epi64(v, B.v);
		return r;
	}

	__forceinline m256si64& operator<=(const m256si64& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m256si64& operator>=(const m256si64& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m256si64& operator==(const m256si64& B) {
		m256si64& R = *this;
		m256si64 r;
		r.v = _mm256_cmpeq_epi32(R.v, B.v);
	}

	__forceinline m256si64& operator!=(const m256si64& B) {
		m256si64 r = *this == B;
		r.v = _mm256_xor_epi64(r.v, zero256);
		return r;
	}

	__forceinline m256si64& operator<<(const m256si64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256si64 r;
		r.v = _mm256_sllv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m256si64& operator>>(const m256si64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m256si64 r;
		r.v = _mm256_srlv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m256d {
	union {
		__m256d v;
		__m256i iv;
		double data[4];
	};

	__forceinline m256d& operator+(const m256d& B) {
		m256d r;
		r.v = _mm256_add_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator-(const m256d& B) {
		m256d r;
		r.v = _mm256_sub_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator*(const m256d& B) {
		m256d r;
		r.v = _mm256_mul_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator/(const m256d& B) {
		m256d r;
		r.v = _mm256_div_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator%(const m256d& B) {
		m256d r;
		m256d temp;
		temp.v = _mm256_div_pd(v, B.v);
		temp.v = _mm256_floor_pd(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256d& operator&(const m256d& B) {
		m256d r;
		r.v = _mm256_and_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator|(const m256d& B) {
		m256d r;
		r.v = _mm256_or_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator^(const m256d& B) {
		m256d r;
		r.v = _mm256_xor_pd(v, B.v);
		return r;
	}

	__forceinline m256d& operator~() {
		m256d r;
		m256d z;
		z.iv = zero256;
		r.v = _mm256_xor_pd(v, z.v);
		return r;
	}

	__forceinline m256d& operator<(const m256d& B) {
		m256d r;
		r.v = _mm256_cmp_pd(B.v, v, 30); // cmp gt oq(ordered, not signaling)
		return r;
	}

	__forceinline m256d& operator>(const m256d& B) {
		m256d r;
		r.v = _mm256_cmp_pd(v, B.v, 30); // cmp gt oq(ordered, not signaling)
		return r;
	}

	__forceinline m256d& operator<=(const m256d& B) {
		m256d r;
		r.v = _mm256_cmp_pd(B.v, v, 29);
		return r;
	}

	__forceinline m256d& operator>=(const m256d& B) {
		m256d r;
		r.v = _mm256_cmp_pd(v, B.v, 29);
		return r;
	}

	__forceinline m256d& operator==(const m256d& B) {
		m256d& R = *this;
		m256d r;
		r.v = _mm256_cmp_pd(R.v, B.v, 0);
		return r;
	}

	__forceinline m256d& operator!=(const m256d& B) {
		m256d& R = *this;
		m256d r;
		r.v = _mm256_cmp_pd(R.v, B.v, 28);
		return r;
	}
};

struct m256f {
	union {
		__m256 v;
		__m256i iv;
		float data[8];
	};

	__forceinline m256f& operator+(const m256f& B) {
		m256f r;
		r.v = _mm256_add_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator-(const m256f& B) {
		m256f r;
		r.v = _mm256_sub_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator*(const m256f& B) {
		m256f r;
		r.v = _mm256_mul_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator/(const m256f& B) {
		m256f r;
		r.v = _mm256_div_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator%(const m256f& B) {
		m256f r;
		m256f temp;
		temp.v = _mm256_div_ps(v, B.v);
		temp.v = _mm256_floor_ps(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m256f& operator&(const m256f& B) {
		m256f r;
		r.v = _mm256_and_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator|(const m256f& B) {
		m256f r;
		r.v = _mm256_or_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator^(const m256f& B) {
		m256f r;
		r.v = _mm256_xor_ps(v, B.v);
		return r;
	}

	__forceinline m256f& operator~() {
		m256f r;
		m256f z;
		z.iv = zero256;
		r.v = _mm256_xor_ps(v, z.v);
		return r;
	}

	__forceinline m256f& operator<(const m256f& B) {
		m256f r;
		r.v = _mm256_cmp_ps(B.v, v, 30);
		return r;
	}

	__forceinline m256f& operator>(const m256f& B) {
		m256f r;
		r.v = _mm256_cmp_ps(v, B.v, 30);
		return r;
	}

	__forceinline m256f& operator<=(const m256f& B) {
		m256f r;
		r.v = _mm256_cmp_ps(B.v, v, 29);
		return r;
	}

	__forceinline m256f& operator>=(const m256f& B) {
		m256f r;
		r.v = _mm256_cmp_ps(v, B.v, 29);
		return r;
	}

	__forceinline m256f& operator==(const m256f& B) {
		m256f& R = *this;
		m256f r;
		r.v = _mm256_cmp_ps(R.v, B.v, 0);
		return r;
	}

	__forceinline m256f& operator!=(const m256f& B) {
		m256f& R = *this;
		m256f r;
		r.v = _mm256_cmp_ps(R.v, B.v, 28);
		return r;
	}
};

struct m256hf {
	union {
		__m256h v;
		__m256 sv;
		__m256i iv;
		hfloat data[16];
	};

	__forceinline m256hf& operator+(const m256hf& B) {
		m256hf r;
		r.v = _mm256_add_ph(v, B.v);
		return r;
	}

	__forceinline m256hf& operator-(const m256hf& B) {
		m256hf r;
		r.v = _mm256_sub_ph(v, B.v);
		return r;
	}

	__forceinline m256hf& operator*(const m256hf& B) {
		m256hf r;
		r.v = _mm256_mul_ph(v, B.v);
		return r;
	}

	__forceinline m256hf& operator/(const m256hf& B) {
		m256hf r;
		r.v = _mm256_div_ph(v, B.v);
		return r;
	}

	/*__forceinline m256hf& operator%(const m256hf& B) {
		m256hf r;
		m256hf temp;
		temp.v = _mm_div_ph(v, B.v);
		temp.v = _mm_floor_pd(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}*/
	//_mm_floor_ph() is not exist.

	__forceinline m256hf& operator&(const m256hf& B) {
		m256hf r;
		r.sv = _mm256_and_ps(sv, B.sv);
		return r;
	}

	__forceinline m256hf& operator|(const m256hf& B) {
		m256hf r;
		r.sv = _mm256_or_ps(sv, B.sv);
		return r;
	}

	__forceinline m256hf& operator^(const m256hf& B) {
		m256hf r;
		r.sv = _mm256_xor_ps(sv, B.sv);
		return r;
	}

	__forceinline m256hf& operator~() {
		m256hf r;
		m256hf z;
		z.iv = zero256;
		r.sv = _mm256_xor_ps(sv, z.sv);
		return r;
	}

	//_mm_cmpgt_ph function is doesn't exist.
	// so later, it have to made with simd subtraction and inline simd toFloat operation of hfloat and pack/unpack operation
	/*__forceinline m256hf& operator<(const m256hf& B) {
		m256hf r;
		r.v = _mm_cmpgt_ph(B.v, v);
		return r;
	}

	__forceinline m256hf& operator>(const m256hf& B) {
		m256hf r;
		r.sv = _mm_cmpgt_ps(sv, B.sv);
		return r;
	}

	__forceinline m256hf& operator<=(const m256hf& B) {
		m256hf r;
		r.v = _mm_cmpge_ps(B.v, v);
		return r;
	}

	__forceinline m256hf& operator>=(const m256hf& B) {
		m256hf r;
		r.v = _mm_cmpge_ps(v, B.v);
		return r;
	}

	__forceinline m256hf& operator==(const m256hf& B) {
		m256hf& R = *this;
		m256hf r;
		r.v = _mm_cmpeq_ps(R.v, B.v);
		return r;
	}

	__forceinline m256hf& operator!=(const m256hf& B) {
		m256hf& R = *this;
		m256hf r;
		r.v = _mm_cmpneq_ps(R.v, B.v);
		return r;
	}*/
};

union M256 {
	ui8 d8[32];
	__m256 simd;
	__m256i nums;
	__m256d d_real;
	__m256h hf_real;
	m256ui8 _ui8;
	m256si8 _si8;
	m256ui16 _ui16;
	m256si16 _si16;
	m256ui32 _ui32;
	m256si32 _si32;
	m256ui64 _ui64;
	m256si64 _si64;
	m256d _d64;
	m256f _f32;
	m256hf _hf16;
};

#endif

#ifdef CPUID_AVX512

constexpr __m512i zero512 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr __m512i max512 = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

struct m512ui8 {
	union {
		__m512i v;
		ui8 data[64];
	};

	static __forceinline m512ui8& mask_to_memory(ui64 mask) {
		m512ui8 r;
		r.v = _mm512_mask_set1_epi8(zero512, mask, 0xFF);
		return r;
	}

	static __forceinline ui64 memory_to_mask(const m512ui8& mem) {
		return _mm512_cmpeq_epi8_mask(max512, mem.v);
	}

	__forceinline m512ui8& operator+(const m512ui8& B) {
		m512ui8 r;
		r.v = _mm512_add_epi8(v, B.v);
		return r;
	}

	__forceinline m512ui8& operator-(const m512ui8& B) {
		m512ui8 r;
		r.v = _mm512_sub_epi8(v, B.v);
		return r;
	}

	__forceinline m512ui8& operator*(const m512ui8& B) {
		m512ui8 r;
		for (int i = 0; i < 8; ++i) {
			r.data[i] = v.m512i_i8[i] * B.data[i];
		}
	}

	__forceinline m512ui8& operator/(const m512ui8& B) {
		m512ui8 r;
		r.v = _mm512_div_epu8(v, B.v);
		return r;
	}

	__forceinline m512ui8& operator%(const m512ui8& B) {
		m512ui8 r;
		m512ui8 temp;
		temp.v = _mm512_div_epu8(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512ui8& operator&(const m512ui8& B) {
		m512ui8 r;
		r.v = _mm512_and_epi32(v, B.v);
		return r;
	}

	__forceinline m512ui8& operator|(const m512ui8& B) {
		m512ui8 r;
		r.v = _mm512_or_epi32(v, B.v);
		return r;
	}

	__forceinline m512ui8& operator^(const m512ui8& B) {
		m512ui8 r;
		r.v = _mm512_xor_epi32(v, B.v);
		return r;
	}

	__forceinline m512ui8& operator~() {
		m512ui8 r;
		r.v = _mm512_xor_epi32(v, zero512);
		return r;
	}

	__forceinline m512ui8& operator<<(ui8 n) {
		m512ui8 r;
		ui8 c = ~((1 << n) - 1);
		r.v = _mm512_slli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512ui8& operator>>(ui8 n) {
		m512ui8 r;
		ui8 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512ui8& operator<(const m512ui8& B) {
		constexpr m512ui8 oneData = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		constexpr m512ui8 lastzero = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
			0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F };
		m512ui8 A1, A2, A3;
		m512ui8 B1, B2, B3;
		m512ui8& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B;
		B1 = B1 >> 1;
		B2 = B;
		B2 = B2 & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		ui64 mask0 = _mm512_cmpgt_epi8_mask(A1.v, B1.v) & _mm512_cmpgt_epi8_mask(A2.v, B2.v);
		mask0 = mask0 | _mm512_cmpgt_epi8_mask(A3.v, B3.v);
		return m512ui8::mask_to_memory(mask0);
	}

	__forceinline m512ui8& operator>(const m512ui8& B) {
		m512ui8 B1;
		return B1 < *this;
	}

	__forceinline m512ui8& operator<=(const m512ui8& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512ui8& operator>=(const m512ui8& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512ui8& operator==(const m512ui8& B) {
		m512ui8& R = *this;
		return m512ui8::mask_to_memory(_mm512_cmpeq_epi8_mask(R.v, B.v));
	}

	__forceinline m512ui8& operator!=(const m512ui8& B) {
		m512ui8& R = *this;
		return m512ui8::mask_to_memory(_mm512_cmpneq_epi8_mask(R.v, B.v));
	}
};

struct m512si8 {
	union {
		__m512i v;
		si8 data[64];
	};

	static __forceinline m512si8& mask_to_memory(ui64 mask) {
		m512si8 r;
		r.v = _mm512_mask_set1_epi8(zero512, mask, 0xFF);
		return r;
	}

	static __forceinline ui32 memory_to_mask(const m512si8& mem) {
		return _mm512_cmpeq_epi8_mask(max512, mem.v);
	}

	__forceinline m512si8& operator+(const m512si8& B) {
		m512si8 r;
		r.v = _mm512_add_epi8(v, B.v);
		return r;
	}

	__forceinline m512si8& operator-(const m512si8& B) {
		m512si8 r;
		r.v = _mm512_sub_epi8(v, B.v);
		return r;
	}

	__forceinline m512si8& operator*(const m512si8& B) {
		m512ui8 r;
		for (int i = 0; i < 8; ++i) {
			r.data[i] = v.m512i_i8[i] * B.data[i];
		}
	}

	__forceinline m512si8& operator/(const m512si8& B) {
		m512si8 r;
		r.v = _mm512_div_epu8(v, B.v);
		return r;
	}

	__forceinline m512si8& operator%(const m512si8& B) {
		m512si8 r;
		m512si8 temp;
		temp.v = _mm512_div_epu8(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512si8& operator&(const m512si8& B) {
		m512si8 r;
		r.v = _mm512_and_epi32(v, B.v);
		return r;
	}

	__forceinline m512si8& operator|(const m512si8& B) {
		m512si8 r;
		r.v = _mm512_or_epi32(v, B.v);
		return r;
	}

	__forceinline m512si8& operator^(const m512si8& B) {
		m512si8 r;
		r.v = _mm512_xor_epi32(v, B.v);
		return r;
	}

	__forceinline m512si8& operator~() {
		m512si8 r;
		r.v = _mm512_xor_epi32(v, zero512);
		return r;
	}

	__forceinline m512si8& operator<<(ui8 n) {
		m512si8 r;
		ui8 c = ~((1 << n) - 1);
		r.v = _mm512_slli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512si8& operator>>(ui8 n) {
		m512si8 r;
		ui8 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_broadcastmb_epi64(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512si8& operator<(const m512si8& B) {
		return m512si8::mask_to_memory(_mm512_cmpgt_epi8_mask(v, B.v));
	}

	__forceinline m512si8& operator>(const m512si8& B) {
		m512si8 B1;
		return B1 < *this;
	}

	__forceinline m512si8& operator<=(const m512si8& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512si8& operator>=(const m512si8& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512si8& operator==(const m512si8& B) {
		m512si8& R = *this;
		return m512si8::mask_to_memory(_mm512_cmpeq_epi8_mask(R.v, B.v));
	}

	__forceinline m512si8& operator!=(const m512si8& B) {
		m512si8& R = *this;
		return m512si8::mask_to_memory(_mm512_cmpneq_epi8_mask(R.v, B.v));
	}
};

struct m512ui16 {
	union {
		__m512i v;
		ui16 data[32];
	};

	static __forceinline m512ui16& mask_to_memory(ui32 mask) {
		m512ui16 r;
		r.v = _mm512_mask_set1_epi16(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui64 memory_to_mask(const m512ui16& mem) {
		return _mm512_cmpeq_epi16_mask(max512, mem.v);
	}

	__forceinline m512ui16& operator+(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_add_epi16(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator-(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_sub_epi16(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator*(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_mullo_epi16(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator/(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_div_epu16(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator%(const m512ui16& B) {
		m512ui16 r;
		m512ui16 temp;
		temp.v = _mm512_div_epu16(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512ui16& operator&(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_and_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator|(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_or_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator^(const m512ui16& B) {
		m512ui16 r;
		r.v = _mm512_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui16& operator~() {
		m512ui16 r;
		r.v = _mm512_xor_epi64(v, zero512);
		return r;
	}

	__forceinline m512ui16& operator<<(ui8 n) {
		m512ui16 r;
		r.v = _mm512_slli_epi16(v, n); // full shift
		return r;
	}

	__forceinline m512ui16& operator>>(ui8 n) {
		m512ui16 r;
		ui16 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_broadcastmw_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512ui16& operator<(const m512ui16& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m512ui16 oneData = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
		constexpr m512ui16 lastzero = { 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
			0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
			0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
			0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF };
		m512ui16 A1, A2, A3;
		m512ui16 B1, B2, B3;
		m512ui16& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		ui32 mask0 = _mm512_cmpgt_epi16_mask(A1.v, B1.v) & _mm512_cmpgt_epi16_mask(A2.v, B2.v);
		mask0 = mask0 | _mm512_cmpgt_epi16_mask(A3.v, B3.v);
		return m512ui16::mask_to_memory(mask0);
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m512i halfData = { 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255,
			0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255 };
		__m512i A1, A2;
		__m512i B1, B2;
		A1 = _mm512_unpacklo_epi16(v, zero512);
		A2 = _mm512_unpackhi_epi16(v, zero512);
		B1 = _mm512_unpacklo_epi16(B.v, zero512);
		B2 = _mm512_unpackhi_epi16(B.v, zero512);
		A1 = m512ui16::mask_to_memory(_mm512_cmpgt_epi32_mask(A1, B1)).v;
		A2 = m512ui16::mask_to_memory(_mm512_cmpgt_epi32_mask(A2, B2)).v;
		m512ui16 r;
		r.v = _mm512_or_epi64(_mm512_and_epi64(A1, halfData), _mm512_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m512ui16& operator>(const m512ui16& B) {
		m512ui16 B1 = B;
		return B1 < *this;
	}

	__forceinline m512ui16& operator<=(const m512ui16& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512ui16& operator>=(const m512ui16& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512ui16& operator==(const m512ui16& B) {
		m512ui16& R = *this;
		return m512ui16::mask_to_memory(_mm512_cmpeq_epi16_mask(R.v, B.v));
	}

	__forceinline m512ui16& operator!=(const m512ui16& B) {
		m512ui16 r = *this == B;
		r.v = _mm512_xor_epi64(r.v, zero512);
		return r;
	}

	__forceinline m512ui16& operator<<(const m512ui16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512ui16 r;
		r.v = _mm512_sllv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m512ui16& operator>>(const m512ui16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512ui16 r, c, k;
		c.v = _mm512_broadcastmw_epi32(8);
		c = c - n;
		k.v = _mm512_broadcastmw_epi32(1);
		c = k << c;
		c = c - k;
		r.v = _mm512_srlv_epi16(v, n.v);
		r = r & c;
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m512si16 {
	union {
		__m512i v;
		si16 data[32];
	};

	static __forceinline m512si16& mask_to_memory(ui32 mask) {
		m512si16 r;
		r.v = _mm512_mask_set1_epi16(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui32 memory_to_mask(const m512si16& mem) {
		return _mm512_cmpeq_epi16_mask(max512, mem.v);
	}

	__forceinline m512si16& operator+(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_add_epi16(v, B.v);
		return r;
	}

	__forceinline m512si16& operator-(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_sub_epi16(v, B.v);
		return r;
	}

	__forceinline m512si16& operator*(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_mullo_epi16(v, B.v);
		return r;
	}

	__forceinline m512si16& operator/(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_div_epu16(v, B.v);
		return r;
	}

	__forceinline m512si16& operator%(const m512si16& B) {
		m512si16 r;
		m512si16 temp;
		temp.v = _mm512_div_epu16(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512si16& operator&(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_and_epi64(v, B.v);
		return r;
	}

	__forceinline m512si16& operator|(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_or_epi64(v, B.v);
		return r;
	}

	__forceinline m512si16& operator^(const m512si16& B) {
		m512si16 r;
		r.v = _mm512_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m512si16& operator~() {
		m512si16 r;
		r.v = _mm512_xor_epi64(v, zero512);
		return r;
	}

	__forceinline m512si16& operator<<(ui8 n) {
		m512si16 r;
		r.v = _mm512_slli_epi16(v, n); // full shift
		return r;
	}

	__forceinline m512si16& operator>>(ui8 n) {
		m512si16 r;
		ui16 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_broadcastmw_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512si16& operator<(const m512si16& B) {
		return m512si16::mask_to_memory(_mm512_cmpgt_epi16_mask(v, B.v));
	}

	__forceinline m512si16& operator>(const m512si16& B) {
		return m512si16::mask_to_memory(_mm512_cmpgt_epi16_mask(B.v, v));
	}

	__forceinline m512si16& operator<=(const m512si16& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512si16& operator>=(const m512si16& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512si16& operator==(const m512si16& B) {
		m512si16& R = *this;
		return m512si16::mask_to_memory(_mm512_cmpeq_epi16_mask(R.v, B.v));
	}

	__forceinline m512si16& operator!=(const m512si16& B) {
		m512si16& R = *this;
		return m512si16::mask_to_memory(_mm512_cmpneq_epi16_mask(R.v, B.v));
	}

	__forceinline m512si16& operator<<(const m512si16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512si16 r;
		r.v = _mm512_sllv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m512si16& operator>>(const m512si16& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512si16 r;
		r.v = _mm512_srlv_epi16(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m512ui32 {
	union {
		__m512i v;
		ui32 data[16];
	};

	static __forceinline m512ui32& mask_to_memory(ui16 mask) {
		m512ui32 r;
		r.v = _mm512_mask_set1_epi32(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui16 memory_to_mask(const m512ui32& mem) {
		return _mm512_cmpeq_epi32_mask(max512, mem.v);
	}

	__forceinline m512ui32& operator+(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_add_epi32(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator-(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_sub_epi32(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator*(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_mullo_epi32(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator/(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_div_epu32(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator%(const m512ui32& B) {
		m512ui32 r;
		m512ui32 temp;
		temp.v = _mm512_div_epu32(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512ui32& operator&(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_and_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator|(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_or_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator^(const m512ui32& B) {
		m512ui32 r;
		r.v = _mm512_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui32& operator~() {
		m512ui32 r;
		r.v = _mm512_xor_epi64(v, zero512);
		return r;
	}

	__forceinline m512ui32& operator<<(ui8 n) {
		m512ui32 r;
		r.v = _mm512_slli_epi32(v, n);
		return r;
	}

	__forceinline m512ui32& operator>>(ui8 n) {
		m512ui32 r;
		ui32 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512ui32& operator<(const m512ui32& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m512ui32 oneData = { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
			0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
		constexpr m512ui32 lastzero = { 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF };
		m512ui32 A1, A2, A3;
		m512ui32 B1, B2, B3;
		m512ui32& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		ui32 mask0 = _mm512_cmpgt_epi32_mask(A1.v, B1.v) & _mm512_cmpgt_epi32_mask(A2.v, B2.v);
		mask0 = mask0 | _mm512_cmpgt_epi32_mask(A3.v, B3.v);
		return m512ui32::mask_to_memeory(mask0);
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m512i halfData = { 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255,
			0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255 };
		__m512i A1, A2;
		__m512i B1, B2;
		A1 = _mm512_unpacklo_epi32(v, zero512);
		A2 = _mm512_unpackhi_epi32(v, zero512);
		B1 = _mm512_unpacklo_epi32(B.v, zero512);
		B2 = _mm512_unpackhi_epi32(B.v, zero512);
		A1 = m512ui32::mask_to_memory(_mm512_cmpgt_epi32_mask(A1, B1)).v;
		A2 = m512ui32::mask_to_memory(_mm512_cmpgt_epi32_mask(A2, B2)).v;
		m512ui32 r;
		r.v = _mm512_or_epi64(_mm512_and_epi64(A1, halfData), _mm512_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m512ui32& operator>(const m512ui32& B) {
		m512ui32 r = B;
		return (r < *this);
	}

	__forceinline m512ui32& operator<=(const m512ui32& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512ui32& operator>=(const m512ui32& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512ui32& operator==(const m512ui32& B) {
		m512ui32& R = *this;
		return m512ui32::mask_to_memory(_mm512_cmpeq_epi32_mask(R.v, B.v));
	}

	__forceinline m512ui32& operator!=(const m512ui32& B) {
		m512ui32 r = *this == B;
		r.v = _mm512_xor_epi64(r.v, zero512);
		return r;
	}

	__forceinline m512ui32& operator<<(const m512ui32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512ui32 r;
		r.v = _mm512_sllv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m512ui32& operator>>(const m512ui32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512ui32 r;
		r.v = _mm512_srlv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 8; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m512si32 {
	union {
		__m512i v;
		si32 data[16];
	};

	static __forceinline m512si32& mask_to_memory(ui16 mask) {
		m512si32 r;
		r.v = _mm512_mask_set1_epi32(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui16 memory_to_mask(const m512si32& mem) {
		return _mm512_cmpeq_epi32_mask(max512, mem.v);
	}

	__forceinline m512si32& operator+(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_add_epi32(v, B.v);
		return r;
	}

	__forceinline m512si32& operator-(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_sub_epi32(v, B.v);
		return r;
	}

	__forceinline m512si32& operator*(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_mullo_epi32(v, B.v);
		return r;
	}

	__forceinline m512si32& operator/(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_div_epi32(v, B.v);
		return r;
	}

	__forceinline m512si32& operator%(const m512si32& B) {
		m512si32 r;
		m512si32 temp;
		temp.v = _mm512_div_epi32(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512si32& operator&(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_and_epi64(v, B.v);
		return r;
	}

	__forceinline m512si32& operator|(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_or_epi64(v, B.v);
		return r;
	}

	__forceinline m512si32& operator^(const m512si32& B) {
		m512si32 r;
		r.v = _mm512_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m512si32& operator~() {
		m512si32 r;
		r.v = _mm512_xor_epi64(v, zero512);
		return r;
	}

	__forceinline m512si32& operator<<(ui8 n) {
		m512si32 r;
		r.v = _mm512_slli_epi32(v, n);
		return r;
	}

	__forceinline m512si32& operator>>(ui8 n) {
		m512si32 r;
		r.v = _mm512_srli_epi32(v, n);
		return r;
	}

	__forceinline m512si32& operator<(const m512si32& B) {
		return m512si32::mask_to_memory(_mm512_cmpgt_epi32_mask(B.v, v));
	}

	__forceinline m512si32& operator>(const m512si32& B) {
		return m512si32::mask_to_memory(_mm512_cmpgt_epi32_mask(v, B.v));
	}

	__forceinline m512si32& operator<=(const m512si32& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512si32& operator>=(const m512si32& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512si32& operator==(const m512si32& B) {
		m512si32& R = *this;
		return m512si32::mask_to_memory(_mm512_cmpeq_epi32_mask(R.v, B.v));
	}

	__forceinline m512si32& operator!=(const m512si32& B) {
		m512si32& R = *this;
		return m512si32::mask_to_memory(_mm512_cmpneq_epi32_mask(R.v, B.v));
	}

	__forceinline m512si32& operator<<(const m512si32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512si32 r;
		r.v = _mm512_sllv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 4; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m512si32& operator>>(const m512si32& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512si32 r;
		r.v = _mm512_srlv_epi32(v, n.v);
		return r;
#else
		for (int i = 0; i < 4; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m512ui64 {
	union {
		__m512i v;
		ui64 data[8];
	};

	static __forceinline m512ui64& mask_to_memory(ui8 mask) {
		m512ui64 r;
		r.v = _mm512_mask_set1_epi64(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui8 memory_to_mask(const m512ui64& mem) {
		return _mm512_cmpeq_epi64_mask(max512, mem.v);
	}

	__forceinline m512ui64& operator+(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_add_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator-(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_sub_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator*(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_mullo_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator/(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_div_epu64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator%(const m512ui64& B) {
		m512ui64 r;
		m512ui64 temp;
		temp.v = _mm512_div_epu64(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512ui64& operator&(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_and_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator|(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_or_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator^(const m512ui64& B) {
		m512ui64 r;
		r.v = _mm512_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m512ui64& operator~() {
		m512ui64 r;
		r.v = _mm512_xor_epi64(v, zero512);
		return r;
	}

	__forceinline m512ui64& operator<<(ui8 n) {
		m512ui64 r;
		r.v = _mm512_slli_epi64(v, n);
		return r;
	}

	__forceinline m512ui64& operator>>(ui8 n) {
		m512ui64 r;
		ui64 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512ui64& operator<(const m512ui64& B) {
#if defined(UNSIGNED_SIMD_CMP_SHIFTCMP)
		constexpr m512ui64 oneData = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
			0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
		constexpr m512ui64 lastzero = { 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		m512ui64 A1, A2, A3;
		m512ui64 B1, B2, B3;
		m512ui64& R = *this;
		A1 = R >> 1;
		A2 = R & oneData;
		B1 = B >> 1;
		B2 = B & oneData;
		A3 = A1 & lastzero;
		B3 = B1 & lastzero;
		ui8 mask0 = _mm512_cmpgt_epi64_mask(A1.v, B1.v) & _mm512_cmpgt_epi64_mask(A2.v, B2.v);
		mask0 = mask0 | _mm512_cmpgt_epi64_mask(A3.v, B3.v);
		return m512ui64::mask_to_memory(mask0);
#elif defined(UNSIGNED_SIMD_CMP_PACKCMP)
		constexpr __m512i halfData = { 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255,
			0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255 };
		__m512i A1, A2;
		__m512i B1, B2;
		A1 = _mm512_unpacklo_epi64(v, zero512);
		A2 = _mm512_unpackhi_epi64(v, zero512);
		B1 = _mm512_unpacklo_epi64(B.v, zero512);
		B2 = _mm512_unpackhi_epi64(B.v, zero512);
		A1 = m512ui64::mask_to_memory(_mm512_cmpgt_epi32_mask(A1, B1)).v;
		A2 = m512ui64::mask_to_memory(_mm512_cmpgt_epi32_mask(A2, B2)).v;
		m512ui64 r;
		r.v = _mm512_or_epi64(_mm512_and_epi64(A1, halfData), _mm512_slli_epi32(A2, 16));
		return r;
#endif
	}

	__forceinline m512ui64& operator>(const m512ui64& B) {
		m512ui64 r = B;
		r = r < *this;
		return r;
	}

	__forceinline m512ui64& operator<=(const m512ui64& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512ui64& operator>=(const m512ui64& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512ui64& operator==(const m512ui64& B) {
		m512ui64& R = *this;
		return m512ui64::mask_to_memory(_mm512_cmpeq_epi32_mask(R.v, B.v));
	}

	__forceinline m512ui64& operator!=(const m512ui64& B) {
		m512ui64 r = *this == B;
		r.v = _mm512_xor_epi64(r.v, zero512);
		return r;
	}

	__forceinline m512ui64& operator<<(const m512ui64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512ui64 r;
		r.v = _mm512_sllv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m512ui64& operator>>(const m512ui64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512ui64 r;
		m512ui64 co;
		constexpr m512ui64 one = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
			0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
		constexpr m512ui64 eight = { 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8,
			0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8 };
		// ((1 << (8-n)) - 1)
		r = eight;
		r = r - n;
		co = one;
		r = co << r;
		co = r - one;
		r.v = _mm512_srlv_epi64(v, n.v);
		r.v = _mm512_and_epi64(r.v, co.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m512si64 {
	union {
		__m512i v;
		si64 data[8];
	};

	static __forceinline m512si64& mask_to_memory(ui8 mask) {
		m512si64 r;
		r.v = _mm512_mask_set1_epi64(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui8 memory_to_mask(const m512si64& mem) {
		return _mm512_cmpeq_epi64_mask(max512, mem.v);
	}

	__forceinline m512si64& operator+(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_add_epi64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator-(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_sub_epi64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator*(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_mullo_epi64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator/(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_div_epu64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator%(const m512si64& B) {
		m512si64 r;
		m512si64 temp;
		temp.v = _mm512_div_epu64(v, B.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512si64& operator&(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_and_epi64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator|(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_or_epi64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator^(const m512si64& B) {
		m512si64 r;
		r.v = _mm512_xor_epi64(v, B.v);
		return r;
	}

	__forceinline m512si64& operator~() {
		m512si64 r;
		r.v = _mm512_xor_epi64(v, zero512);
		return r;
	}

	__forceinline m512si64& operator<<(ui8 n) {
		m512si64 r;
		r.v = _mm512_slli_epi64(v, n);
		return r;
	}

	__forceinline m512si64& operator>>(ui8 n) {
		m512si64 r;
		ui64 c = ((1 << (8 - n)) - 1); // n=5 -> 00000111
		r.v = _mm512_srli_epi64(v, n); // full shift
		r.v = _mm512_and_epi64(r.v, _mm512_set1_epi32(c)); // broadcast c and and r
		return r;
	}

	__forceinline m512si64& operator<(const m512si64& B) {
		return m512si64::mask_to_memory(_mm512_cmpgt_epi64_mask(B.v, v));
	}

	__forceinline m512si64& operator>(const m512si64& B) {
		return m512si64::mask_to_memory(_mm512_cmpgt_epi64_mask(v, B.v));
	}

	__forceinline m512si64& operator<=(const m512si64& B) {
		return (*this < B) | (*this == B);
	}

	__forceinline m512si64& operator>=(const m512si64& B) {
		return (*this > B) | (*this == B);
	}

	__forceinline m512si64& operator==(const m512si64& B) {
		m512si64& R = *this;
		return m512si64::mask_to_memory(_mm512_cmpeq_epi32_mask(R.v, B.v));
	}

	__forceinline m512si64& operator!=(const m512si64& B) {
		m512si64 r = *this == B;
		r.v = _mm512_xor_epi64(r.v, zero512);
		return r;
	}

	__forceinline m512si64& operator<<(const m512si64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512si64 r;
		r.v = _mm512_sllv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] << n.data[i];
		}
#endif
	}

	__forceinline m512si64& operator>>(const m512si64& n) {
		// there is no support 8bit offsets vectors << vectors shifter
#ifdef CPUID_AVX2
		m512si64 r;
		r.v = _mm512_srlv_epi64(v, n.v);
		return r;
#else
		for (int i = 0; i < 2; ++i) {
			r.data[i] = r.data[i] >> n.data[i];
		}
#endif
	}
};

struct m512d {
	union {
		__m512d v;
		__m512i bv;
		double data[8];
	};

	static __forceinline m512d& mask_to_memory(ui8 mask) {
		m512d r;
		r.bv = _mm512_mask_set1_epi64(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui8 memory_to_mask(const m512d& mem) {
		return _mm512_cmpeq_epi64_mask(max512, mem.bv);
	}

	__forceinline m512d& operator+(const m512d& B) {
		m512d r;
		r.v = _mm512_add_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator-(const m512d& B) {
		m512d r;
		r.v = _mm512_sub_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator*(const m512d& B) {
		m512d r;
		r.v = _mm512_mul_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator/(const m512d& B) {
		m512d r;
		r.v = _mm512_div_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator%(const m512d& B) {
		m512d r;
		m512d temp;
		temp.v = _mm512_div_pd(v, B.v);
		temp.v = _mm512_floor_pd(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512d& operator&(const m512d& B) {
		m512d r;
		r.v = _mm512_and_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator|(const m512d& B) {
		m512d r;
		r.v = _mm512_or_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator^(const m512d& B) {
		m512d r;
		r.v = _mm512_xor_pd(v, B.v);
		return r;
	}

	__forceinline m512d& operator~() {
		m512d r, z;
		z.bv = zero512;
		r.v = _mm512_xor_pd(v, z.v);
		return r;
	}

	__forceinline m512d& operator<(const m512d& B) {
		return m512d::mask_to_memory(_mm512_cmp_pd_mask(B.v, v, 30)); // cmp gt oq(ordered, not signaling)
	}

	__forceinline m512d& operator>(const m512d& B) {
		return m512d::mask_to_memory(_mm512_cmp_pd_mask(v, B.v, 30)); // cmp gt oq(ordered, not signaling)
	}

	__forceinline m512d& operator<=(const m512d& B) {
		return m512d::mask_to_memory(_mm512_cmp_pd_mask(B.v, v, 29));
	}

	__forceinline m512d& operator>=(const m512d& B) {
		return m512d::mask_to_memory(_mm512_cmp_pd_mask(v, B.v, 29));
	}

	__forceinline m512d& operator==(const m512d& B) {
		m512d& R = *this;
		return m512d::mask_to_memory(_mm512_cmp_pd_mask(R.v, B.v, 0));
	}

	__forceinline m512d& operator!=(const m512d& B) {
		m512d& R = *this;
		return m512d::mask_to_memory(_mm512_cmp_pd_mask(R.v, B.v, 28));
	}
};

struct m512f {
	union {
		__m512 v;
		__m512i bv;
		float data[16];
	};

	static __forceinline m512f& mask_to_memory(ui16 mask) {
		m512f r;
		r.bv = _mm512_mask_set1_epi32(zero512, mask, 0xFFFF);
		return r;
	}

	static __forceinline ui16 memory_to_mask(const m512f& mem) {
		return _mm512_cmpeq_epi32_mask(max512, mem.bv);
	}

	__forceinline m512f& operator+(const m512f& B) {
		m512f r;
		r.v = _mm512_add_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator-(const m512f& B) {
		m512f r;
		r.v = _mm512_sub_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator*(const m512f& B) {
		m512f r;
		r.v = _mm512_mul_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator/(const m512f& B) {
		m512f r;
		r.v = _mm512_div_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator%(const m512f& B) {
		m512f r;
		m512f temp;
		temp.v = _mm512_div_ps(v, B.v);
		temp.v = _mm512_floor_ps(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}

	__forceinline m512f& operator&(const m512f& B) {
		m512f r;
		r.v = _mm512_and_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator|(const m512f& B) {
		m512f r;
		r.v = _mm512_or_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator^(const m512f& B) {
		m512f r;
		r.v = _mm512_xor_ps(v, B.v);
		return r;
	}

	__forceinline m512f& operator~() {
		m512f r, z;
		z.bv = zero512;
		r.v = _mm512_xor_ps(v, z.v);
		return r;
	}

	__forceinline m512f& operator<(const m512f& B) {
		return m512f::mask_to_memory(_mm512_cmp_ps_mask(B.v, v, 30));
	}

	__forceinline m512f& operator>(const m512f& B) {
		return m512f::mask_to_memory(_mm512_cmp_ps_mask(v, B.v, 30));
	}

	__forceinline m512f& operator<=(const m512f& B) {
		return m512f::mask_to_memory(_mm512_cmp_ps_mask(B.v, v, 29));
	}

	__forceinline m512f& operator>=(const m512f& B) {
		return m512f::mask_to_memory(_mm512_cmp_ps_mask(v, B.v, 29));
	}

	__forceinline m512f& operator==(const m512f& B) {
		m512f& R = *this;
		return m512f::mask_to_memory(_mm512_cmp_ps_mask(R.v, B.v, 0));
	}

	__forceinline m512f& operator!=(const m512f& B) {
		m512f& R = *this;
		return m512f::mask_to_memory(_mm512_cmp_ps_mask(R.v, B.v, 28));
	}
};

struct m512hf {
	union {
		__m512h v;
		__m512 sv;
		__m512i iv;
		hfloat data[32];
	};

	__forceinline m512hf& operator+(const m512hf& B) {
		m512hf r;
		r.v = _mm512_add_ph(v, B.v);
		return r;
	}

	__forceinline m512hf& operator-(const m512hf& B) {
		m512hf r;
		r.v = _mm512_sub_ph(v, B.v);
		return r;
	}

	__forceinline m512hf& operator*(const m512hf& B) {
		m512hf r;
		r.v = _mm512_mul_ph(v, B.v);
		return r;
	}

	__forceinline m512hf& operator/(const m512hf& B) {
		m512hf r;
		r.v = _mm512_div_ph(v, B.v);
		return r;
	}

	/*__forceinline m512hf& operator%(const m512hf& B) {
		m512hf r;
		m512hf temp;
		temp.v = _mm_div_ph(v, B.v);
		temp.v = _mm_floor_pd(temp.v);
		temp = temp * B;
		r = r - temp;
		return r;
	}*/
	//_mm_floor_ph() is not exist.

	__forceinline m512hf& operator&(const m512hf& B) {
		m512hf r;
		r.sv = _mm512_and_ps(sv, B.sv);
		return r;
	}

	__forceinline m512hf& operator|(const m512hf& B) {
		m512hf r;
		r.sv = _mm512_or_ps(sv, B.sv);
		return r;
	}

	__forceinline m512hf& operator^(const m512hf& B) {
		m512hf r;
		r.sv = _mm512_xor_ps(sv, B.sv);
		return r;
	}

	__forceinline m512hf& operator~() {
		m512hf r, z;
		z.iv = zero512;
		r.sv = _mm512_xor_ps(sv, z.sv);
		return r;
	}

	//_mm_cmpgt_ph function is doesn't exist.
	// so later, it have to made with simd subtraction and inline simd toFloat operation of hfloat and pack/unpack operation
	/*__forceinline m512hf& operator<(const m512hf& B) {
		m512hf r;
		r.v = _mm_cmpgt_ph(B.v, v);
		return r;
	}

	__forceinline m512hf& operator>(const m512hf& B) {
		m512hf r;
		r.sv = _mm_cmpgt_ps(sv, B.sv);
		return r;
	}

	__forceinline m512hf& operator<=(const m512hf& B) {
		m512hf r;
		r.v = _mm_cmpge_ps(B.v, v);
		return r;
	}

	__forceinline m512hf& operator>=(const m512hf& B) {
		m512hf r;
		r.v = _mm_cmpge_ps(v, B.v);
		return r;
	}

	__forceinline m512hf& operator==(const m512hf& B) {
		m512hf& R = *this;
		m512hf r;
		r.v = _mm_cmpeq_ps(R.v, B.v);
		return r;
	}

	__forceinline m512hf& operator!=(const m512hf& B) {
		m512hf& R = *this;
		m512hf r;
		r.v = _mm_cmpneq_ps(R.v, B.v);
		return r;
	}*/
};

union M512 {
	ui8 d8[64];
	__m512 simd;
	__m512i nums;
	__m512d d_real;
	__m512h hf_real;
	m512ui8 _ui8;
	m512si8 _si8;
	m512ui16 _ui16;
	m512si16 _si16;
	m512ui32 _ui32;
	m512si32 _si32;
	m512ui64 _ui64;
	m512si64 _si64;
	m512d _d64;
	m512f _f32;
	m512hf _hf16;
};

#endif

//compiler dependency
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

#endif