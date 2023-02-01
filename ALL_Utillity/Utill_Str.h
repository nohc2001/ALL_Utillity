#pragma once
#include <string>
#include <vector>
#include "Utill_FreeMemory.h"

using namespace std;

namespace strf {
	constexpr int MAX_CALCUL_STRING = 4096;
	static freemem::FM_Model0 FM;

	int _Init() {
		FM.SetHeapData(new byte8[MAX_CALCUL_STRING], MAX_CALCUL_STRING);
		return 1;
	}

	int error = _Init();


	//cfm - clear free memory -> it mean that return value is temp memmory. delete soon.
	//		so An error may occur if a string-related function is called simultaneously from different threads.
	
	//cms - change max size -> if return value is overflow in present maxsiz, then expend maxsiz.
	
	//crv - cut return value -> if return value is overflow in present maxsiz, then cutting return value.

	//mpd - malloc. please delete. -> this return is original heap. please free after use it.
	
	//igm - ignore maxsize -> if return value is overflow in present maxsiz, then ignore maxsiz. 
	//	so The maxsize of the dest in igm must be greater than the maxsize of the return value. Failure to do so may result in errors.


	//convenient and safe type - because it include maxsize.
	typedef class lcstr {
	public:
		int maxSize = 0;
		char* str = nullptr;
		freemem::FM_Model* fm;

		lcstr() {

		}

		lcstr(freemem::FM_Model* FM, const char* text) {
			int len = strlen(text);
			Init(len + 2, FM);
			strcpy_s(str, maxSize, text);
		}

		lcstr(const lcstr& ref) {
			maxSize = ref.maxSize;
			fm = ref.fm;
			if (str != nullptr && fm->bAlloc((byte8*)str, maxSize)) {
				fm->_Delete((byte8*)str, maxSize);
			}
			str = (char*)fm->_New(sizeof(char) * maxSize);
		}

		virtual ~lcstr() {
			if (str != nullptr && fm->bAlloc((byte8*)str, maxSize)) {
				fm->_Delete((byte8*)str, maxSize);
			}
		}

		lcstr* Init(int max, freemem::FM_Model* FM) {
			if (str != nullptr && fm->bAlloc((byte8*)str, maxSize)) {
				fm->_Delete((byte8*)str, maxSize);
			}

			Init_VPTR<lcstr>(this);

			maxSize = max;
			fm = FM;
			str = (char*)fm->_New(sizeof(char) * max);

			return this;
		}

		char& at(size_t i) {
			return str[i];
		}

		char& operator[](size_t i) {
			return str[i];
		}

		void operator=(const char* cstr) {
			int len = strlen(cstr);
			if (len > maxSize) {
				if (str != nullptr && fm->bAlloc((byte8*)str, maxSize)) {
					fm->_Delete((byte8*)str, maxSize);
				}
				maxSize = len + 2;
				str = (char*)fm->_New(sizeof(char) * maxSize);
				strcpy_s(str, maxSize, cstr);
			}
			else {
				strcpy_s(str, maxSize, cstr);
			}
		}

		void operator=(lcstr cstr) {
			int len = strlen(cstr.str);
			if (len > maxSize) {
				if (str != nullptr && fm->bAlloc((byte8*)str, maxSize)) {
					fm->_Delete((byte8*)str, maxSize);
				}
				maxSize = len + 2;
				str = (char*)fm->_New(sizeof(char) * maxSize);
				strcpy_s(str, maxSize, cstr.str);
			}
			else {
				strcpy_s(str, maxSize, cstr.str);
			}
		}

		void size_expand(int newsiz) {
			char* newstr = (char*)fm->_New(sizeof(char) * newsiz);
			int len = strlen(str);
			for (int i = 0; i < newsiz; ++i) {
				if (i < len) {
					newstr[i] = str[i];
				}
				else {
					newstr[i] = 0;
				}
			}

			fm->_Delete((byte8*)str, sizeof(char) * maxSize);
			str = newstr;
		}
	};

	typedef class lwstr {
	public:
		int maxSize = 0;
		wchar_t* str = nullptr;
		freemem::FM_Model* fm;

		lwstr() {

		}

		lwstr(freemem::FM_Model* FM, const wchar_t* text) {
			int len = wcslen(text);
			Init(len + 2, FM);
			wcscpy_s(str, maxSize, text);
		}

		lwstr(const lwstr& ref) {
			maxSize = ref.maxSize;
			fm = ref.fm;
			if (str != nullptr && fm->bAlloc((byte8*)str, sizeof(wchar_t) * maxSize)) {
				fm->_Delete((byte8*)str, sizeof(wchar_t) * maxSize);
			}
			str = (wchar_t*)fm->_New(sizeof(wchar_t) * maxSize);
		}

		virtual ~lwstr() {
			if (str != nullptr && fm->bAlloc((byte8*)str, sizeof(wchar_t) * maxSize)) {
				fm->_Delete((byte8*)str, sizeof(wchar_t) * maxSize);
			}
		}

		lwstr* Init(int max, freemem::FM_Model* FM) {
			Init_VPTR<lwstr>(this);

			maxSize = max;
			fm = FM;
			str = (wchar_t*)fm->_New(sizeof(wchar_t) * max);

			return this;
		}

		wchar_t& at(size_t i) {
			return str[i];
		}

		wchar_t& operator[](size_t i) {
			return str[i];
		}

		void operator=(const wchar_t* cstr) {
			int len = wcslen(cstr);
			if (len > maxSize) {
				if (str != nullptr && fm->bAlloc((byte8*)str, sizeof(wchar_t) * maxSize)) {
					fm->_Delete((byte8*)str, sizeof(wchar_t) * maxSize);
				}
				maxSize = len + 2;
				str = (wchar_t*)fm->_New(sizeof(wchar_t) * maxSize);
				wcscpy_s(str, maxSize, cstr);
			}
			else {
				wcscpy_s(str, maxSize, cstr);
			}
		}

		void operator=(lwstr cstr) {
			int len = wcslen(cstr.str);
			if (len > maxSize) {
				if (str != nullptr && fm->bAlloc((byte8*)str, sizeof(wchar_t) * maxSize)) {
					fm->_Delete((byte8*)str, sizeof(wchar_t) * maxSize);
				}
				maxSize = len + 2;
				str = (wchar_t*)fm->_New(sizeof(wchar_t) * maxSize);
				wcscpy_s(str, maxSize, cstr.str);
			}
			else {
				wcscpy_s(str, maxSize, cstr.str);
			}
		}

		void size_expand(int newsiz) {
			wchar_t* newstr = (wchar_t*)fm->_New(sizeof(wchar_t) * newsiz);
			int len = wcslen(str);
			for (int i = 0; i < newsiz; ++i) {
				if (i < len) {
					newstr[i] = str[i];
				}
				else {
					newstr[i] = 0;
				}
			}

			fm->_Delete((byte8*)str, sizeof(wchar_t) * maxSize);
			str = newstr;
		}
	};

	//compare
	int compare_str(const char* str1, const char* str2) {
		return strcmp(str1, str2);
	}

	bool isSame_str(const char* str1, const char* str2) {
		if (compare_str(str1, str2) == 0) {
			return true;
		}
		else return false;
	}

	int compare_wstr(const wchar_t* wstr1, const wchar_t* wstr2) {
		return wcscmp(wstr1, wstr2);
	}

	bool isSame_wstr(const wchar_t* wstr1, const wchar_t* wstr2) {
		if (compare_wstr(wstr1, wstr2) == 0) {
			return true;
		}
		else return false;
	}

	int compare_length_str(const char* str1, const char* str2) {
		int l1 = strlen(str1);
		int l2 = strlen(str2);
		return l1 - l2;
	}

	bool isSame_length_str(const char* str1, const char* str2) {
		if (compare_length_str(str1, str2) == 0) {
			return true;
		}
		else return false;
	}

	int compare_length_wstr(const wchar_t* wstr1, const wchar_t* wstr2) {
		int l1 = wcslen(wstr1);
		int l2 = wcslen(wstr2);
		return l1 - l2;
	}

	bool isSame_length_wstr(const wchar_t* wstr1, const wchar_t* wstr2) {
		if (compare_length_wstr(wstr1, wstr2) == 0) {
			return true;
		}
		else return false;
	}

	//transform
	//transform - charactor type transform
	wchar_t char_to_wchar(char c) {
		wchar_t wc = c;
		return wc;
	}

	char wchar_to_char(wchar_t wc) {
		char c = wc;
		return wc;
	}

	//transform - string type transform
	wchar_t* str_to_wstr_cfm(const char* str) {
		FM.ClearAll();
		int len = strlen(str);
		wchar_t* wstr = (wchar_t*)FM._New(sizeof(wchar_t) * len);

		for (int i = 0; i < len; ++i) {
			wstr[i] = char_to_wchar(str[i]);
		}

		return wstr;
	}

	char* wstr_to_str_cfm(const wchar_t* wstr) {
		FM.ClearAll();

		int len = wcslen(wstr);
		char* str = (char*)FM._New(sizeof(char) * len);

		for (int i = 0; i < len; ++i) {
			str[i] = wchar_to_char(wstr[i]);
		}

		return str;
	}

	void str_to_wstr_igm(wchar_t* dest, const char* source) {
		wcscpy_s(dest, strlen(source), str_to_wstr_cfm(source));
	}

	void wstr_to_str_igm(char* dest, const wchar_t* source) {
		strcpy_s(dest, wcslen(source), wstr_to_str_cfm(source));
	}

	void str_to_wstr_crv(lwstr dest, lcstr source) {
		int len = strlen(source.str);
		for (int i = 0; i < dest.maxSize; ++i) {
			if (len <= i) {
				dest[i] = 0;
			}
			else {
				dest[i] = char_to_wchar(source.str[i]);
			}
		}
	}

	void wstr_to_str_crv(lcstr dest, lwstr source) {
		int len = wcslen(source.str);
		for (int i = 0; i < dest.maxSize; ++i) {
			if (len <= i) {
				dest[i] = 0;
			}
			else {
				dest[i] = wchar_to_char(source.str[i]);
			}
		}
	}

	void str_to_wstr_cms(lwstr dest, lcstr source) {
		int len = strlen(source.str);
		if (len > dest.maxSize) {
			if (dest.str != nullptr && dest.fm->bAlloc((byte8*)dest.str, sizeof(wchar_t) * dest.maxSize)) {
				dest.fm->_Delete((byte8*)dest.str, sizeof(wchar_t) * dest.maxSize);
			}
			dest.maxSize = len + 2;
			dest.str = (wchar_t*)dest.fm->_New(sizeof(wchar_t) * dest.maxSize);
			
			str_to_wstr_crv(dest, source);
		}
		else {
			str_to_wstr_crv(dest, source);
		}
	}

	void wstr_to_str_cms(lcstr dest, lwstr source) {
		int len = wcslen(source.str);
		if (len > dest.maxSize) {
			if (dest.str != nullptr && dest.fm->bAlloc((byte8*)dest.str, sizeof(char) * dest.maxSize)) {
				dest.fm->_Delete((byte8*)dest.str, sizeof(char) * dest.maxSize);
			}
			dest.maxSize = len + 2;
			dest.str = (char*)dest.fm->_New(sizeof(char) * dest.maxSize);

			wstr_to_str_crv(dest, source);
		}
		else {
			wstr_to_str_crv(dest, source);
		}
	}

	char* string_to_str_cfm(string s) {
		int maxsiz = s.size();
		char* rstr = (char*)FM._New(sizeof(char) * maxsiz);
		strcpy_s(rstr, maxsiz, s.c_str());
		return rstr;
	}

	wchar_t* wstring_to_wstr_cfm(wstring s) {
		int maxsiz = s.size();
		wchar_t* rstr = (wchar_t*)FM._New(sizeof(wchar_t) * maxsiz);
		wcscpy_s(rstr, maxsiz, s.c_str());
		return rstr;
	}

	void string_to_str_crv(lcstr dest, string s) {
		int len = strlen(s.c_str());
		for (int i = 0; i < dest.maxSize; ++i) {
			if (i >= len) {
				dest[i] = 0;
			}
			else {
				dest[i] = s[i];
			}
		}
	}

	void wstring_to_wstr_crv(lwstr dest, wstring s) {
		int len = wcslen(s.c_str());
		for (int i = 0; i < dest.maxSize; ++i) {
			if (i >= len) {
				dest[i] = 0;
			}
			else {
				dest[i] = s[i];
			}
		}
	}

	void string_to_str_cms(lcstr dest, string s) {
		int len = strlen(s.c_str());
		if (len > dest.maxSize) {
			if (dest.str != nullptr && dest.fm->bAlloc((byte8*)dest.str, sizeof(char) * dest.maxSize)) {
				dest.fm->_Delete((byte8*)dest.str, sizeof(char) * dest.maxSize);
			}
			dest.maxSize = len + 2;
			dest.str = (char*)dest.fm->_New(sizeof(char) * dest.maxSize);

			string_to_str_crv(dest, s);
		}
		else {
			string_to_str_crv(dest, s);
		}
	}

	void wstring_to_wstr_cms(lwstr dest, wstring s) {
		int len = wcslen(s.c_str());
		if (len > dest.maxSize) {
			if (dest.str != nullptr && dest.fm->bAlloc((byte8*)dest.str, sizeof(wchar_t) * dest.maxSize)) {
				dest.fm->_Delete((byte8*)dest.str, sizeof(wchar_t) * dest.maxSize);
			}
			dest.maxSize = len + 2;
			dest.str = (wchar_t*)dest.fm->_New(sizeof(wchar_t) * dest.maxSize);

			wstring_to_wstr_crv(dest, s);
		}
		else {
			wstring_to_wstr_crv(dest, s);
		}
	}

	//transform - string type to number type transform
	int str_to_int(const char* source) {
		int len = strlen(source);
		int ptr = 0;
		int num = 0;
		bool minus = false;
		if (source[0] == '-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned int str_to_uint(const char* source) {
		int len = strlen(source);
		unsigned int num = 0;
		for (int i = 0; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	short str_to_short(const char* source) {
		int len = strlen(source);
		int ptr = 0;
		short num = 0;
		bool minus = false;
		if (source[0] == '-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned short str_to_ushort(const char* source) {
		int len = strlen(source);
		unsigned short num = 0;
		for (int i = 0; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	long str_to_long(const char* source) {
		int len = strlen(source);
		int ptr = 0;
		long num = 0;
		bool minus = false;
		if (source[0] == '-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned long str_to_ulong(const char* source) {
		int len = strlen(source);
		unsigned long num = 0;
		for (int i = 0; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	long long str_to_longlong(const char* source) {
		int len = strlen(source);
		int ptr = 0;
		long long num = 0;
		bool minus = false;
		if (source[0] == '-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned long long str_to_ulonglong(const char* source) {
		int len = strlen(source);
		unsigned long long num = 0;
		for (int i = 0; i < len; ++i) {
			if ('0' <= source[i] && source[i] <= '9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	double str_to_double(const char* source) {
		return atof(source);
	}

	float str_to_float(const char* source) {
		return (float)str_to_double(source);
	}

	bool str_to_bool(const char* source) {
		if (strcmp(source, "true")) {
			return true;
		}
		else return false;
	}

	int wstr_to_int(const wchar_t* source) {
		int len = wcslen(source);
		int ptr = 0;
		int num = 0;
		bool minus = false;
		if (source[0] == L'-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned int wstr_to_uint(const wchar_t* source) {
		int len = wcslen(source);
		unsigned int num = 0;
		for (int i = 0; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	short wstr_to_short(const wchar_t* source) {
		int len = wcslen(source);
		int ptr = 0;
		short num = 0;
		bool minus = false;
		if (source[0] == L'-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - '0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned short wstr_to_ushort(const wchar_t* source) {
		int len = wcslen(source);
		unsigned short num = 0;
		for (int i = 0; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	long wstr_to_long(const wchar_t* source) {
		int len = wcslen(source);
		int ptr = 0;
		long num = 0;
		bool minus = false;
		if (source[0] == L'-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned long wstr_to_ulong(const wchar_t* source) {
		int len = wcslen(source);
		unsigned long num = 0;
		for (int i = 0; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	long long wstr_to_longlong(const wchar_t* source) {
		int len = wcslen(source);
		int ptr = 0;
		long long num = 0;
		bool minus = false;
		if (source[0] == L'-') {
			minus = true;
			ptr += 1;
		}

		for (int i = ptr; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		if (minus) {
			num = -num;
		}

		return num;
	}

	unsigned long long wstr_to_ulonglong(const wchar_t* source) {
		int len = wcslen(source);
		unsigned long long num = 0;
		for (int i = 0; i < len; ++i) {
			if (L'0' <= source[i] && source[i] <= L'9') {
				num += source[i] - L'0';
			}
			else {
				return 0;
			}

			if (i != len - 1) {
				num *= 10;
			}
		}

		return num;
	}

	double wstr_to_double(const wchar_t* source) {
		return atof(wstr_to_str_cfm(source));
	}

	float wstr_to_float(const wchar_t* source) {
		return (float)str_to_double(wstr_to_str_cfm(source));
	}

	bool wstr_to_bool(const wchar_t* source) {
		if (wcscmp(source, L"true")) {
			return true;
		}
		else return false;
	}

	//transform - number type to string type transform
	//cfm
	char* int_to_str_cfm(int value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* uint_to_str_cfm(unsigned int value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* short_to_str_cfm(short value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* ushort_to_str_cfm(unsigned short value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* long_to_str_cfm(long value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* ulong_to_str_cfm(unsigned long value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* longlong_to_str_cfm(long long value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* ulonglong_to_str_cfm(long long value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* double_to_str_cfm(double value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	char* float_to_str_cfm(float value) {
		string s = to_string(value);
		return string_to_str_cfm(s);
	}

	wchar_t* int_to_wstr_cfm(int value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* uint_to_wstr_cfm(unsigned int value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* short_to_wstr_cfm(short value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* ushort_to_wstr_cfm(unsigned short value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* long_to_wstr_cfm(long value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* ulong_to_wstr_cfm(unsigned long value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* longlong_to_wstr_cfm(long long value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* ulonglong_to_wstr_cfm(long long value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* double_to_wstr_cfm(double value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	wchar_t* float_to_wstr_cfm(float value) {
		wstring s = to_wstring(value);
		return wstring_to_wstr_cfm(s);
	}

	//crv
	void int_to_str_crv(lcstr dest, int value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void uint_to_str_crv(lcstr dest, unsigned int value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void short_to_str_crv(lcstr dest, short value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void ushort_to_str_crv(lcstr dest, unsigned short value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void long_to_str_crv(lcstr dest, long value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void ulong_to_str_crv(lcstr dest, unsigned long value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void longlong_to_str_crv(lcstr dest, long long value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void ulonglong_to_str_crv(lcstr dest, long long value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void double_to_str_crv(lcstr dest, double value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void float_to_str_crv(lcstr dest, float value) {
		string s = to_string(value);
		string_to_str_crv(dest, s);
	}

	void int_to_wstr_crv(lwstr dest, int value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void uint_to_wstr_crv(lwstr dest, unsigned int value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void short_to_wstr_crv(lwstr dest, short value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void ushort_to_wstr_crv(lwstr dest, unsigned short value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void long_to_wstr_crv(lwstr dest, long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void ulong_to_wstr_crv(lwstr dest, unsigned long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void longlong_to_wstr_crv(lwstr dest, long long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void ulonglong_to_wstr_crv(lwstr dest, long long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void double_to_wstr_crv(lwstr dest, double value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	void float_to_wstr_crv(lwstr dest, float value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_crv(dest, s);
	}

	//cms
	void int_to_str_cms(lcstr dest, int value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void uint_to_str_cms(lcstr dest, unsigned int value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void short_to_str_cms(lcstr dest, short value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void ushort_to_str_cms(lcstr dest, unsigned short value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void long_to_str_cms(lcstr dest, long value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void ulong_to_str_cms(lcstr dest, unsigned long value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void longlong_to_str_cms(lcstr dest, long long value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void ulonglong_to_str_cms(lcstr dest, long long value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void double_to_str_cms(lcstr dest, double value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void float_to_str_cms(lcstr dest, float value) {
		string s = to_string(value);
		string_to_str_cms(dest, s);
	}

	void int_to_wstr_cms(lwstr dest, int value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void uint_to_wstr_cms(lwstr dest, unsigned int value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void short_to_wstr_cms(lwstr dest, short value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void ushort_to_wstr_cms(lwstr dest, unsigned short value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void long_to_wstr_cms(lwstr dest, long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void ulong_to_wstr_cms(lwstr dest, unsigned long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void longlong_to_wstr_cms(lwstr dest, long long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void ulonglong_to_wstr_cms(lwstr dest, long long value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void double_to_wstr_cms(lwstr dest, double value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	void float_to_wstr_cms(lwstr dest, float value) {
		wstring s = to_wstring(value);
		wstring_to_wstr_cms(dest, s);
	}

	// string add function
	void stradd_cms(lcstr dest, const char* source) {
		int destlen = strlen(dest.str);
		int sourcelen = strlen(source);
		int addlen = destlen + sourcelen + 2;
		if (dest.maxSize < addlen) {
			dest.size_expand(addlen);
		}

		for (int i = destlen; i < addlen; ++i) {
			if (i - destlen < sourcelen) {
				dest.str[i] = source[i - destlen];
			}
			else {
				dest.str[i] = 0;
			}
		}
	}

	void wstradd_cms(lwstr dest, const wchar_t* source) {
		int destlen = wcslen(dest.str);
		int sourcelen = wcslen(source);
		int addlen = destlen + sourcelen + 2;
		if (dest.maxSize < addlen) {
			dest.size_expand(addlen);
		}

		for (int i = destlen; i < addlen; ++i) {
			if (i - destlen < sourcelen) {
				dest.str[i] = source[i - destlen];
			}
			else {
				dest.str[i] = 0;
			}
		}
	}

	// string search function
	vector<int> str_search_char(const char* str, char charactor) {
		vector<int> rvi;
		int len = strlen(str);
		for (int i = 0; i < len; ++i) {
			if (str[i] == charactor) {
				rvi.push_back(i);
			}
		}

		return rvi;
	}

	vector<int> wstr_search_char(const wchar_t* str, char charactor) {
		vector<int> rvi;
		int len = wcslen(str);
		for (int i = 0; i < len; ++i) {
			if (str[i] == charactor) {
				rvi.push_back(i);
			}
		}

		return rvi;
	}

	vector<int> str_search_str(const char* bigstr, const char* smallstr) {
		vector<int> rvi;
		int len = strlen(bigstr);
		int slen = strlen(smallstr);
		for (int i = 0; i < len; ++i) {
			if (i + slen > len) break;

			bool isadd = true;
			for (int k = 0; k < slen; ++k) {
				if (bigstr[i + k] != smallstr[k]) {
					isadd = false;
					break;
				}
			}

			if (isadd) {
				rvi.push_back(i);
			}
		}

		return rvi;
	}

	vector<int> wstr_search_wstr(const wchar_t* bigstr, const wchar_t* smallstr) {
		vector<int> rvi;
		int len = wcslen(bigstr);
		int slen = wcslen(smallstr);
		for (int i = 0; i < len; ++i) {
			if (i + slen > len) break;

			bool isadd = true;
			for (int k = 0; k < slen; ++k) {
				if (bigstr[i + k] != smallstr[k]) {
					isadd = false;
					break;
				}
			}

			if (isadd) {
				rvi.push_back(i);
			}
		}

		return rvi;
	}
};