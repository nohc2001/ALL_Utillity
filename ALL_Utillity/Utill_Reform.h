#pragma once
#include <complex>
#include <math.h>
#include <iostream>
#include <valarray>
#include "Utill_FreeMemory.h"

using namespace freemem;

namespace reform {
	const double PI = 3.14159265358979323846;

	typedef std::complex<double> Complex;
	typedef std::valarray<Complex> CArray;

	constexpr int fmsize = 4096;
	static freemem::FM_Model0 reformFM;

	inline void reformInit() {
		reformFM.SetHeapData(new byte8[fmsize], fmsize);
	}

	// Function to reverse bits of num
	inline unsigned int reverseBits(unsigned int num, int bits)
	{
		unsigned int reverse_num = 0;
		for (int i = 0; i < bits; i++)
		{
			if ((num & (1 << i)))
				reverse_num |= 1 << ((bits - 1) - i);
		}
		return reverse_num;
	}

	// Iterative in-place bit-reversed Cooley-Tukey FFT
	inline void fft(CArray& x)
	{
		const size_t N = x.size();
		const size_t M = log2(N);

		// bit-reversed permutation
		for (size_t i = 0; i < N; i++)
		{
			size_t j = reverseBits(i, M);
			if (i < j) std::swap(x[i], x[j]);
		}

		// butterfly updates
		for (size_t s = 1; s <= M; s++)
		{
			size_t m = 1 << s;
			Complex wm = std::polar(1.0, -2 * PI / m);

			for (size_t k = 0; k < N; k += m)
			{
				Complex w = 1;
				for (size_t j = 0; j < m / 2; j++)
				{
					Complex t = w * x[k + j + m / 2];
					Complex u = x[k + j];

					x[k + j] = u + t;
					x[k + j + m / 2] = u - t;
					w *= wm;
				}
			}
		}
	}

	// Iterative in-place bit-reversed Cooley-Tukey IFFT
	inline void ifft(CArray& x)
	{
		// conjugate the complex numbers
		x = x.apply(std::conj);

		// forward fft
		fft(x);

		// conjugate the complex numbers again
		x = x.apply(std::conj);

		// scale the numbers
		x /= x.size();
	}

	inline void fastConvolusion(CArray& a, CArray& b, Complex* resultdata_out, int rsiz) {
		reformFM.ClearAll();
		int maxsiz = (a.size() > b.size()) ? a.size() : b.size();

		Complex* adata = (Complex*)reformFM._New(maxsiz * sizeof(Complex));
		Complex* bdata = (Complex*)reformFM._New(maxsiz * sizeof(Complex));
		for (int i = 0; i < maxsiz; ++i) {
			adata[i] = complex<double>();
			bdata[i] = complex<double>();
			adata[i] = a[i];
			bdata[i] = b[i];
		}
		CArray aarr(adata, maxsiz);
		CArray barr(adata, maxsiz);
		fft(aarr);
		fft(barr);

		for (int i = 0; i < maxsiz; ++i) {
			aarr[i] = aarr[i] * barr[i];
		}
		ifft(aarr);

		int m = 0;
		if (maxsiz >= rsiz) m = maxsiz;
		else m = rsiz;
		for (int i = 0; i < m; ++i) {
			resultdata_out[i] = aarr[i];
		}
	}
};