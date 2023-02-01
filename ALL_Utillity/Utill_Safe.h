#pragma once
#include "Utill_FreeMemory.h"

using namespace freemem;
// 0  1  2  3
// 4  5  6  7
// 8  9  10 11
// 12 13 14 15
class HC2x2_16_11 {
public:
	byte8 data[2] = {};
	// y : 1(5, 9, 13, 3, 7, 11, 15), 2(6, 10, 14, 7, 11, 15) 
	// x : 4(5, 6, 7, 12, 13, 14, 15), 8(9, 10, 11, 13, 14, 15) / xy : 0
	bool isSafe() {
		bool isodd = GetByte8(data[0], 0);
		int stack = 0;
		//0
		{
			for (int i = 0; i < 2; ++i) {
				for (int k = 0; k < 8; ++k) {
					if (i == 0 && k == 0) continue;
					if (GetByte8(data[i], k)) {
						stack += 1;
					}
				}
			}

			if ((isodd && stack % 2 == 1) || (!isodd && stack % 2 == 0) == false) {
				return false;
			}
		}

		//1
		{
			isodd = GetByte8(data[0], 1);
			stack = 0;

			if (GetByte8(data[0], 5)) { // 5
				stack += 1;
			}
			if (GetByte8(data[0], 3)) { // 3
				stack += 1;
			}
			if (GetByte8(data[0], 7)) { // 7
				stack += 1;
			}
			if (GetByte8(data[1], 1)) { // 9
				stack += 1;
			}
			if (GetByte8(data[1], 5)) { // 13
				stack += 1;
			}
			if (GetByte8(data[1], 3)) { // 11
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if ((isodd && stack % 2 == 1) || (!isodd && stack % 2 == 0) == false) {
				return false;
			}
		}

		//2
		{
			isodd = GetByte8(data[0], 2);
			stack = 0;

			if (GetByte8(data[0], 6)) { // 6
				stack += 1;
			}
			if (GetByte8(data[0], 7)) { // 7
				stack += 1;
			}
			if (GetByte8(data[1], 2)) { // 10
				stack += 1;
			}
			if (GetByte8(data[1], 3)) { // 11
				stack += 1;
			}
			if (GetByte8(data[1], 6)) { // 14
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if ((isodd && stack % 2 == 1) || (!isodd && stack % 2 == 0) == false) {
				return false;
			}
		}

		//4
		{
			isodd = GetByte8(data[0], 2);
			stack = 0;

			if (GetByte8(data[0], 5)) { // 5
				stack += 1;
			}
			if (GetByte8(data[0], 6)) { // 6
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 7
				stack += 1;
			}
			if (GetByte8(data[1], 4)) { // 12
				stack += 1;
			}
			if (GetByte8(data[1], 5)) { // 13
				stack += 1;
			}
			if (GetByte8(data[1], 6)) { // 14
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if ((isodd && stack % 2 == 1) || (!isodd && stack % 2 == 0) == false) {
				return false;
			}
		}

		//8
		{
			isodd = GetByte8(data[1], 0);
			stack = 0;

			if (GetByte8(data[1], 1)) { // 9
				stack += 1;
			}
			if (GetByte8(data[1], 2)) { // 10
				stack += 1;
			}
			if (GetByte8(data[1], 3)) { // 11
				stack += 1;
			}
			if (GetByte8(data[1], 5)) { // 13
				stack += 1;
			}
			if (GetByte8(data[1], 6)) { // 14
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if ((isodd && stack % 2 == 1) || (!isodd && stack % 2 == 0) == false) {
				return false;
			}
		}

		return true;
	}

	unsigned int ConvertIndex(int loc) {
		switch (loc) {
		case 0:
			return 3;
		case 1:
			return 5;
		case 2:
			return 6;
		case 3:
			return 7;
		case 4:
			return 9;
		case 5:
			return 10;
		case 6:
			return 11;
		case 7:
			return 12;
		case 8:
			return 13;
		case 9:
			return 14;
		case 10:
			return 15;
		}
	}

	void SetData(byte8 dat) {
		for (int i = 0; i < 8; ++i) {
			bool b = GetByte8(dat, i);
			int ind = ConvertIndex(i) / 8;
			int loc = ConvertIndex(i) % 8;
			data[ind] = SetByte8(data[ind], loc, b);
		}

		int stack = 0;


		//1
		{
			stack = 0;

			if (GetByte8(data[0], 5)) { // 5
				stack += 1;
			}
			if (GetByte8(data[0], 3)) { // 3
				stack += 1;
			}
			if (GetByte8(data[0], 7)) { // 7
				stack += 1;
			}
			if (GetByte8(data[1], 1)) { // 9
				stack += 1;
			}
			if (GetByte8(data[1], 5)) { // 13
				stack += 1;
			}
			if (GetByte8(data[1], 3)) { // 11
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if (stack % 2 == 0) {
				data[0] = SetByte8(data[0], 1, false);
			}
			else {
				data[0] = SetByte8(data[0], 1, true);
			}
		}

		//2
		{
			stack = 0;

			if (GetByte8(data[0], 6)) { // 6
				stack += 1;
			}
			if (GetByte8(data[0], 7)) { // 7
				stack += 1;
			}
			if (GetByte8(data[1], 2)) { // 10
				stack += 1;
			}
			if (GetByte8(data[1], 3)) { // 11
				stack += 1;
			}
			if (GetByte8(data[1], 6)) { // 14
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if (stack % 2 == 0) {
				data[0] = SetByte8(data[0], 2, false);
			}
			else {
				data[0] = SetByte8(data[0], 2, true);
			}
		}

		//4
		{
			stack = 0;

			if (GetByte8(data[0], 5)) { // 5
				stack += 1;
			}
			if (GetByte8(data[0], 6)) { // 6
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 7
				stack += 1;
			}
			if (GetByte8(data[1], 4)) { // 12
				stack += 1;
			}
			if (GetByte8(data[1], 5)) { // 13
				stack += 1;
			}
			if (GetByte8(data[1], 6)) { // 14
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if (stack % 2 == 0) {
				data[0] = SetByte8(data[0], 4, false);
			}
			else {
				data[0] = SetByte8(data[0], 4, true);
			}
		}

		//8
		{
			stack = 0;

			if (GetByte8(data[1], 1)) { // 9
				stack += 1;
			}
			if (GetByte8(data[1], 2)) { // 10
				stack += 1;
			}
			if (GetByte8(data[1], 3)) { // 11
				stack += 1;
			}
			if (GetByte8(data[1], 5)) { // 13
				stack += 1;
			}
			if (GetByte8(data[1], 6)) { // 14
				stack += 1;
			}
			if (GetByte8(data[1], 7)) { // 15
				stack += 1;
			}

			if (stack % 2 == 0) {
				data[1] = SetByte8(data[1], 0, false);
			}
			else {
				data[1] = SetByte8(data[1], 0, true);
			}
		}

		//0
		{
			for (int i = 0; i < 2; ++i) {
				for (int k = 0; k < 8; ++k) {
					if (i == 0 && k == 0) continue;
					if (GetByte8(data[i], k)) {
						stack += 1;
					}
				}
			}

			if (stack % 2 == 0) {
				data[0] = SetByte8(data[0], 0, false);
			}
			else {
				data[0] = SetByte8(data[0], 0, true);
			}
		}
	}
};

class HammingCodeData_8nxm {
public:
	FM_Model* originFM;
	byte8* data;
	int n;
	int m; // 이상적으로는 8*n 정도의 크기여야 함.
	int realDataSiz_bit = 0;
	int realDataSiz_Byte = 0;
	int Fup_Bit = 0;
	HammingCodeData_8nxm(FM_Model* FM, int _n, int _m) {
		originFM = FM;
		n = _n;
		m = _m;
		data = originFM->_New(n * m);
		realDataSiz_bit = 8 * n * m - (3 + log2(n) + log2(m));
		realDataSiz_Byte = realDataSiz_bit / 8;
	}

	bool isSafe() {
		int stack = 0;
		bool isOdd = GetByte8(data[0], 0);
		for (int a = 0; a < 8 * n; ++a) {
			for (int b = 0; b < m; ++b) {
				if (a == 0 && b == 0) continue;
				int ind = (a / 8) + b * n;
				int loc = a % 8;
				if (GetByte8(data[ind], loc)) {
					stack += 1;
				}
			}
		}

		if ((stack % 2 == 1 && !isOdd) || (stack % 2 == 0 && isOdd)) {
			return false;
		}

		int xcheckNum = 3 + (int)log2(n);
		int ycheckNum = (int)log2(m);

		for (int i = 0; i < xcheckNum; ++i) {
			stack = 0;
			int rooti = pow(2, i);
			int rind = rooti / 8;
			int rloc = rooti % 8;
			isOdd = GetByte8(data[rind], rloc);
			for (int k = rooti; k < 8 * n; k += 2 * rooti) {
				for (int kk = k; kk < k + rooti; ++kk) {
					for (int b = 0; b < m; ++b) {
						if (kk == k && b == 0) continue;
						int ind = (kk / 8) + b * n;
						int loc = kk % 8;
						if (GetByte8(data[ind], loc)) {
							stack += 1;
						}
					}
				}
			}

			if ((stack % 2 == 1 && !isOdd) || (stack % 2 == 0 && isOdd)) {
				return false;
			}
		}

		for (int i = 0; i < ycheckNum; ++i) {
			stack = 0;
			int rooty = pow(2, i);
			int rind = rooty * n;
			int rloc = 0;
			isOdd = GetByte8(data[rind], rloc);

			for (int k = rooty; k < m; k += 2 * rooty) {
				for (int kk = k; kk < k + rooty; ++k) {
					for (int a = 0; a < 8 * n; ++a) {
						if (kk == k && a == 0) continue;

						int ind = (a / 8) + kk * n;
						int loc = a % 8;
						if (GetByte8(data[ind], loc)) {
							stack += 1;
						}
					}
				}
			}

			if ((stack % 2 == 1 && !isOdd) || (stack % 2 == 0 && isOdd)) {
				return false;
			}
		}
	}

	void ChangeSafing() {
		int stack = 0;
		for (int a = 0; a < 8 * n; ++a) {
			for (int b = 0; b < m; ++b) {
				if (a == 0 && b == 0) continue;
				int ind = (a / 8) + b * n;
				int loc = a % 8;
				if (GetByte8(data[ind], loc)) {
					stack += 1;
				}
			}
		}

		data[0] = SetByte8(data[0], 0, stack % 2 == 1);

		int xcheckNum = 3 + (int)log2(n);
		int ycheckNum = (int)log2(m);

		for (int i = 0; i < xcheckNum; ++i) {
			stack = 0;
			int rooti = pow(2, i);
			int rind = rooti / 8;
			int rloc = rooti % 8;
			for (int k = rooti; k < 8 * n; k += 2 * rooti) {
				for (int kk = k; kk < k + rooti; ++kk) {
					for (int b = 0; b < m; ++b) {
						if (kk == k && b == 0) continue;
						int ind = (kk / 8) + b * n;
						int loc = kk % 8;
						if (GetByte8(data[ind], loc)) {
							stack += 1;
						}
					}
				}
			}

			data[rind] = SetByte8(data[rind], rloc, stack % 2 == 1);
		}

		for (int i = 0; i < ycheckNum; ++i) {
			stack = 0;
			int rooty = pow(2, i);
			int rind = rooty * n;
			int rloc = 0;

			for (int k = rooty; k < m; k += 2 * rooty) {
				for (int kk = k; kk < k + rooty; ++k) {
					for (int a = 0; a < 8 * n; ++a) {
						if (kk == k && a == 0) continue;

						int ind = (a / 8) + kk * n;
						int loc = a % 8;
						if (GetByte8(data[ind], loc)) {
							stack += 1;
						}
					}
				}
			}

			data[rind] = SetByte8(data[rind], rloc, stack % 2 == 1);
		}
	}

	void AppendData(byte8 dat) {
		if (isSafe() == false) return;

		int xcheckNum = 3 + (int)log2(n);
		int ycheckNum = (int)log2(m);

		for (int i = 0; i < 8; ++i) {
			for (int k = 1; k < xcheckNum; k *= 2) {
				if (k == Fup_Bit) {
					++Fup_Bit;
				}
			}

			for (int k = 1; k < ycheckNum; k *= 2) {
				if (k * 8 * n == Fup_Bit) {
					++Fup_Bit;
				}
			}

			data[Fup_Bit / 8] = SetByte8(data[Fup_Bit / 8], Fup_Bit % 8, GetByte8(dat, i));
			++Fup_Bit;
		}

		//데이터가 안전하도록 바꿈
		ChangeSafing();
	}
};