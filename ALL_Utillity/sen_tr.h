#ifndef H_SEN_TR
#define H_SEN_TR

#include "arr_expend.h"
#include "Utill_FreeMemory.h"
#include "multifor.h"

using namespace freemem;

union segstr
{
	char* str;
	char symbol;
};

struct segment
{
	char type = 'w';			// w : word, x : variable
	segstr data;				// word str
};

typedef fmvecarr < segment > sen;

struct seg_variable
{
	char symbol = 0;
	sen* value;
};

struct seg_range
{
	int start = 0;
	int end = 0;
};

typedef fmvecarr < fmvecarr < seg_variable >*>var_cases;

class word_base_sen_sys
{
public:
	fmvecarr < char* >wordlist;
	// data rule : every word mem siz is len + 1

	word_base_sen_sys()
	{
	}

	virtual ~word_base_sen_sys()
	{
	}

	void Init() {
		wordlist.NULLState();
		wordlist.Init(2, false, true);
	}

	void Release() {
		for (int i = 0; i < wordlist.size(); ++i) {
			char* cstr = wordlist.at(i);
			//cout << "Release : " << cstr << endl;
			int siz = strlen(cstr) + 1;
			fm->_Delete((byte8*)cstr, siz);
			wordlist.at(i) = nullptr;
		}
		wordlist.release();
		wordlist.NULLState();
	}

	void dbg_sen(sen* s)
	{
		for (int i = 0; i < s->size(); ++i)
		{
			if (s->at(i).type == 'w')
			{
				cout << s->at(i).data.str << " ";
			}
			else
			{
				cout << "var(" << s->at(i).data.symbol << ") ";
			}
		}
		cout << endl;
	}

	void dbg_varcases(var_cases* cases)
	{
		for (int i = 0; i < cases->size(); ++i)
		{
			cout << i << " case : " << endl;
			for (int k = 0; k < cases->at(i)->size(); ++k)
			{
				char sym = cases->at(i)->at(k).symbol;
				if (sym == 0)
				{
					cout << "\tbs ";
				}
				else
				{
					cout << "\t[" << sym << "]";
				}

				dbg_sen(cases->at(i)->at(k).value);
			}
		}
	}

	char* addword(char* str)
	{
		int siz = strlen(str) + 1;
		char* cstr = (char*)fm->_New(siz, true);
		strcpy_s(cstr, siz, str);
		wordlist.push_back(cstr);
		return cstr;
	}

	char* searchword(char* str)
	{
		for (int i = 0; i < wordlist.size(); ++i)
		{
			if (strcmp(wordlist.at(i), str) == 0)
			{
				return wordlist[i];
			}
		}
		return nullptr;
	}

	sen* makesen(const char* segs)
	{
		sen* ten = (sen*)fm->_New(sizeof(sen), true);
		int len = strlen(segs);
		fmlcstr str;
		str.NULLState();
		str.Init(len + 1, false);
		int _stack = 0;
		for (int i = 0; i < len; ++i)
		{
			if (_stack == 2)
			{
				segment sgm;
				sgm.data.symbol = segs[i];
				sgm.type = 'x';
				ten->push_back(sgm);
				str.clear();
			}
			else
			{
				if (segs[i] != ' ' && i + 1 < len)
				{
					str.push_back(segs[i]);
				}
				else
				{
					if (i + 1 == len)
					{
						str.push_back(segs[i]);
					}

					char* cstr = searchword(str.c_str());
					if (cstr == nullptr)
					{
						continue;
					}
					else
					{
						segment sgm;
						sgm.data.str = cstr;
						sgm.type = 'w';
						ten->push_back(sgm);
					}
					str.clear();
				}
			}

			if (segs[i] == '_')
			{
				++_stack;
			}
			else
			{
				_stack = 0;
			}
		}

		return ten;
	}

	fmvecarr < seg_range >* searchsen(sen* bigsen, sen* formsen)
	{
		int len = bigsen->size() - formsen->size() + 1;
		fmvecarr < seg_range >* ret =
			(fmvecarr < seg_range > *)fm->_New(sizeof(fmvecarr < seg_range >), true);
		ret->NULLState();
		ret->Init(2, false, true);
		int fsiz = formsen->size();
		for (int i = 0; i < len; ++i)
		{
			bool perfect = true;
			for (int k = 0; k < fsiz; ++k)
			{
				if (formsen->at(k).data.str != bigsen->at(i + k).data.str)
				{
					perfect = false;
					break;
				}
			}

			if (perfect)
			{
				seg_range r;
				r.start = i;
				r.end = i + fsiz - 1;
				ret->push_back(r);
			}
		}

		return ret;
	}

	var_cases* formming(sen* bigsen, sen* formsen)
	{
		var_cases* ret = (var_cases*)fm->_New(sizeof(var_cases), true);
		ret->NULLState();
		ret->Init(2, false, true);

		// get sentences not include variable
		fmvecarr < sen* >sentences;
		sentences.NULLState();
		sentences.Init(2, false, true);

		sen* ten = (sen*)fm->_New(sizeof(sen), true);
		ten->NULLState();
		ten->Init(2, false, true);
		for (int i = 0; i < formsen->size(); ++i)
		{
			if (formsen->at(i).type == 'x')
			{
				// dbg_sen(ten);
				sentences.push_back(ten);
				ten = (sen*)fm->_New(sizeof(sen), true);
				ten->NULLState();
				ten->Init(2, false, true);
			}
			else
			{
				ten->push_back(formsen->at(i));
			}
		}
		// dbg_sen(ten);
		sentences.push_back(ten);


		// check sentence in bigsen
		fmvecarr < fmvecarr < seg_range >*>rangess;
		rangess.NULLState();
		rangess.Init(2, false, true);
		for (int i = 0; i < sentences.size(); ++i)
		{
			fmvecarr < seg_range >* vecran;
			// dbg_sen(bigsen);
			// dbg_sen(sentences.at(i));
			vecran = searchsen(bigsen, sentences.at(i));
			if (vecran->size() == 0)
			{
				return nullptr;
			}
			else
			{
				rangess.push_back(vecran);
			}
		}

		// multifor for sentences combination
		fmvecarr < int >* sizes = (fmvecarr < int >*)fm->_New(sizeof(fmvecarr < int >), true);
		sizes->NULLState();
		sizes->Init(2, false, true);
		fmvecarr < seg_range > senranges;
		senranges.NULLState();
		senranges.Init(2, false, true);
		int rsiz = rangess.size();
		for (int i = 0; i < rangess.size(); ++i)
		{
			sizes->push_back(rangess.at(i)->size());
			seg_range sr;
			senranges.push_back(sr);
		}
		int total = get_total_loop(sizes);

		for (int i = 0; i < total; ++i)
		{
			bool perfect = true;
			for (int k = 0; k < rsiz; ++k)
			{
				senranges[k] = rangess[k]->operator[](get_index(sizes, k, i));
			}

			for (int k = 0; k < rsiz - 1; ++k)
			{
				if (senranges[k].end + 1 >= senranges[k + 1].start)
				{
					perfect = false;
					break;
				}
			}

			if (perfect)
			{
				// get symbols in formsen
				fmvecarr < seg_variable >* vararr =
					(fmvecarr < seg_variable > *)fm->_New(sizeof(fmvecarr < seg_variable >), true);
				vararr->NULLState();
				vararr->Init(2, false, true);
				seg_variable var;
				var.symbol = 0;
				var.value = bigsen;
				vararr->push_back(var);
				for (int i = 0; i < formsen->size(); ++i)
				{
					if (formsen->at(i).type == 'x')
					{
						seg_variable varv;
						varv.symbol = formsen->at(i).data.symbol;
						varv.value = nullptr;
						vararr->push_back(varv);
					}
				}

				// if code enter here, then update [ret].
				for (int k = 0; k < rsiz - 1; ++k)
				{
					sen* newsen = (sen*)fm->_New(sizeof(sen), true);
					for (int h = senranges[k].end + 1; h < senranges[k + 1].start; ++h)
					{
						// push variable
						newsen->push_back(bigsen->at(h));
					}
					vararr->at(k + 1).value = newsen;
				}

				ret->push_back(vararr);
			}
		}

		return ret;
	}

	int search_word_first(int index, sen* arr, const char* word) {
		for (int i = index; i < arr->size(); ++i) {
			if (strcmp(arr->at(i).data.str, word) == 0) {
				return i;
			}
		}

		return -1;
	}

	int search_word_first_cd(int index, sen* arr, bool (*cd)(char*)) {
		for (int i = index; i < arr->size(); ++i) {
			if (cd(arr->at(i).data.str)) {
				return i;
			}
		}

		return -1;
	}

	sen* sen_cut(sen* arr, int start, int end) {
		sen* rarr = (sen*)fm->_New(sizeof(sen), true);
		rarr->NULLState();
		rarr->Init(2, false, true);
		for (int i = start; i <= end; ++i) {
			rarr->push_back(arr->at(i));
		}
		return rarr;
	}

	sen* oc_search(sen* arr, int start, const char* open, const char* close)
	{
		sen* rarr = (sen*)fm->_New(sizeof(sen), true);
		rarr->NULLState();
		rarr->Init(2, false, true);
		int stack = 0;
		bool isin = false;
		for (int i = start; i < (int)arr->size(); ++i)
		{
			if (strcmp(arr->at(i).data.str, open) == 0)
			{
				if (isin == false)
					isin = true;
				++stack;
			}

			if (strcmp(arr->at(i).data.str, close) == 0)
			{
				--stack;
			}

			if (isin)
			{
				rarr->push_back(arr->at(i));
				if (stack == 0)
				{
					break;
				}
			}
		}

		return rarr;
	}

	sen* oc_search_inv(sen* arr, int end, const char* open, const char* close)
	{
		sen* rarr = (sen*)fm->_New(sizeof(sen), true);
		rarr->NULLState();
		rarr->Init(2, false, true);
		int stack = 0;
		bool isin = false;
		int start = 0;
		for (int i = end; i >= 0; --i)
		{
			if (strcmp(arr->at(i).data.str, close) == 0)
			{
				if (isin == false)
					isin = true;
				++stack;
			}

			if (strcmp(arr->at(i).data.str, open) == 0)
			{
				--stack;
			}

			if (isin)
			{
				if (stack == 0)
				{
					start = i;
					break;
				}
			}
		}

		for (int i = start; i < end + 1; ++i)
		{
			rarr->push_back(arr->at(i));
		}

		return rarr;
	}

	int search_word_first_in_specific_oc_layer(sen* arr, int start, const char* open, const char* close, int layer, const char* word) {
		int stack = 0;
		for (int i = start; i < (int)arr->size(); ++i)
		{
			if (strcmp(arr->at(i).data.str, open) == 0)
			{
				++stack;
				continue;
			}

			if (strcmp(arr->at(i).data.str, close) == 0)
			{
				--stack;
				continue;
			}

			if (stack == layer) {
				if (strcmp(arr->at(i).data.str, word) == 0) {
					return i;
				}
			}
		}

		return -1;
	}

	int search_word_end_in_specific_oc_layer(sen* arr, int end, const char* open, const char* close, int layer, const char* word) {
		int stack = 0;
		for (int i = end; i >= 0; --i)
		{
			if (strcmp(arr->at(i).data.str, close) == 0)
			{
				++stack;
				continue;
			}

			if (strcmp(arr->at(i).data.str, open) == 0)
			{
				--stack;
				continue;
			}

			if (stack == layer) {
				if (strcmp(arr->at(i).data.str, word) == 0) {
					return i;
				}
			}
		}

		return -1;
	}
};

char* addform(char* str) {
	int len = strlen(str);
	char* rstr = (char*)fm->_New(len + 1, true);
	strcpy_s(rstr, len, str);
	for (int i = 0; i < len; ++i) {
		//if variable
		if (rstr[i] == '$' && i + 1 < len) {
			//variable number find
			int s, e;
			s = i + 1;
			for (int k = i + 1; k < len; ++k) {
				if (('0' <= rstr[k] && rstr[k] <= '9') == false) {
					e = k - 1;
					break;
				}
			}
			int varnumlen = e - s + 1;
			fm->_tempPushLayer();
			char* varnum = (char*)fm->_tempNew(varnumlen+1);
			for (int k = 0; k < varnumlen+1; ++k) {
				varnum[k] = rstr[s + k];
			}
			varnum[varnumlen] = 0;
			int n = atoi(varnum);
			if (n > 127) n = 127;
			fm->_tempPopLayer();

			//replace form data
			rstr[i] = 128 + n;
			for (int k = i + 1; k < len - varnumlen + 1; ++k) {
				rstr[k] = rstr[k + varnumlen];
			}
			rstr[len - varnumlen + 1] = 0;
			len = len - varnumlen + 1;
		}
	}

	return rstr;
}

int findstr_first(int start, char* bigstr, char* formstr) {

}

#endif