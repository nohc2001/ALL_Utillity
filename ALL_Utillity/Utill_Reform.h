#pragma once
#include <complex>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <queue>
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


	// Fast Convolution
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


	//FZ77 compress algrithm
	struct DeflateBlock {
		uint32_t length;
		uint32_t distance;
	};

	std::vector<DeflateBlock> deflate_FZ77(const uint8_t* data, size_t length) {
		std::vector<DeflateBlock> blocks;
		std::map<std::string, int> window;
		int window_start = 0;
		int search_start = 0;
		for (int i = 0; i < length; i++) {
			int search_end = std::min(i + 32767, (int)length - 1);
			int best_length = 0;
			int best_distance = 0;
			for (int j = search_start; j <= search_end; j++) {
				int current_length = 0;
				while (i + current_length < length &&
					data[j + current_length] == data[i + current_length]) {
					current_length++;
					if (j + current_length > search_end) {
						break;
					}
				}
				if (current_length > best_length) {
					best_length = current_length;
					best_distance = i - j;
				}
			}

			DeflateBlock block;
			block.distance = best_distance;
			block.length = best_length;
			blocks.push_back(block);

			std::string window_string(reinterpret_cast<const char*>(data + i),
				best_length);
			window[window_string] = i;
			i += best_length - 1;
			if (window.size() > 32768) {
				window.erase(window.begin());
				window_start++;
			}
			search_start = std::max(search_start, window_start);
		}
		return blocks;
	}


	//Huffman Coding
	struct Node {
		int value;
		int frequency;
		Node* left;
		Node* right;

		Node(int value, int frequency) : value(value), frequency(frequency), left(nullptr), right(nullptr) {}

		bool operator<(const Node& other) const { return frequency > other.frequency; }
	};

	struct Leaf : public Node {
		Leaf(int value, int frequency) : Node(value, frequency) {}
	};

	struct InternalNode : public Node {
		InternalNode(Node* left, Node* right) : Node(0, left->frequency + right->frequency) {
			this->left = left;
			this->right = right;
		}
	};

	std::map<int, std::vector<bool>> assignCodes(Node* node, bool clear) {
		static std::map<int, std::vector<bool>> codes;
		static std::vector<bool> code;

		if (clear) {
			codes.clear();
			code.clear();
		}

		if (node->left == nullptr && node->right == nullptr) {
			codes[node->value] = code;
		}
		else {
			code.push_back(false);
			assignCodes(node->left, false);
			code.back() = true;
			assignCodes(node->right, false);
			code.pop_back();
		}

		return codes;
	};

	std::map<int, std::vector<bool>> huffmanCoding(const std::map<int, int>& frequency) {
		std::priority_queue<Node*> q;
		for (const auto& p : frequency) {
			q.push(new Leaf(p.first, p.second));
		}
		while (q.size() > 1) {
			Node* left = q.top();
			q.pop();
			Node* right = q.top();
			q.pop();
			q.push(new InternalNode(left, right));
		}
		
		return assignCodes(q.top(), true);
	}
};