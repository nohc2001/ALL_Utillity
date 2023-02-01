#pragma once
#include <string>
#include <vector>

using namespace std;

namespace strarr {
	typedef vector<string> Strarr;

	inline Strarr GetStrArrFromStrArr(strarr::Strarr arr, int start, int end);
	inline void InsertStrArr(strarr::Strarr* bigarr, int start, strarr::Strarr smallarr);
	inline vector<string> GetSegmentStrArrFromStrArr(strarr::Strarr arr, int start, string open, string close);
	inline vector<string> GetInvSegmentStrArrFromStrArr(strarr::Strarr arr, int end, string open, string close);
	inline void DeleteRangeFromStrArr(strarr::Strarr* arr, int start, int end);
	inline string GetStrFromArr(strarr::Strarr arr, char seperateChar);
};

strarr::Strarr strarr::GetStrArrFromStrArr(Strarr arr, int start, int end) {
	vector<string> rarr;
	int minn = min((int)arr.size(), end + 1);
	for (int i = start; i < minn; ++i) {
		rarr.push_back(arr[i]);
	}
	return rarr;
}

void strarr::InsertStrArr(Strarr* bigarr, int start, Strarr smallarr) {
	for (int i = start; i < start + (int)smallarr.size(); ++i) {
		int n = i - start;
		if (i >= (int)bigarr->size()) {
			bigarr->push_back(smallarr[n].c_str());
		}
		else {
			bigarr->insert(bigarr->begin() + i, smallarr[n].c_str());
		}
	}
}

strarr::Strarr strarr::GetSegmentStrArrFromStrArr(Strarr arr, int start, string open, string close) {
	vector<string> rarr;
	int stack = 0;
	bool isin = false;
	for (int i = start; i < (int)arr.size(); ++i) {
		if (arr[i] == open) {
			if (isin == false) isin = true;
			++stack;
		}

		if (arr[i] == close) {
			--stack;
		}

		if (isin) {
			rarr.push_back(arr[i].c_str());
			if (stack == 0) {
				break;
			}
		}
	}

	return rarr;
}

strarr::Strarr strarr::GetInvSegmentStrArrFromStrArr(Strarr arr, int end, string open, string close) {
	vector<string> rarr;
	int stack = 0;
	bool isin = false;
	int start = 0;
	for (int i = end; i >= 0; --i) {
		if (arr[i] == close) {
			if (isin == false) isin = true;
			++stack;
		}

		if (arr[i] == open) {
			--stack;
		}

		if (isin) {
			if (stack == 0) {
				start = i;
				break;
			}
		}
	}

	for (int i = start; i < end + 1; ++i) {
		rarr.push_back(arr[i].c_str());
	}

	return rarr;
}

void strarr::DeleteRangeFromStrArr(Strarr* arr, int start, int end) {
	for (int i = start; i < end + 1; ++i) {
		arr->erase(arr->begin() + start);
	}
}

string strarr::GetStrFromArr(strarr::Strarr arr, char seperateChar)
{
	string rstr;
	for (int i = 0; i < arr.size(); ++i) {
		for (int k = 0; k < arr[i].size(); ++k) {
			rstr.push_back(arr[i][k]);
		}
		rstr.push_back(seperateChar);
	}
	return rstr;
}