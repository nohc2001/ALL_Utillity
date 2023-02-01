#pragma once
#include <string>
#include <vector>
#include <math.h>
#include "Utill_ConditionDefines.h"
#include "Utill_StrArr.h"

#define 분수로변환 tr_Ration
#define 최대약분 tr_ReductionOfFraction
#define 소인수분해 tr_PrimeFactorization
#define 거듭제곱분해 tr_SpreadingPower
#define 곱셈분배 tr_SpreadingDistributionLaw
#define 정리 tr_Clean
using namespace std;

inline void oper_plus(float* stackArray, bool* ActiveArray, int k) {
	if (k > 0) {
		float f1 = stackArray[k];
		float f2 = stackArray[k - 1];
		float re = f1 + f2;
		stackArray[k - 1] = re;
		ActiveArray[k] = false;
		stackArray[k] = 0;
	}
}

inline void oper_minus(float* stackArray, bool* ActiveArray, int k) {
	if (k > 0) {
		float f1 = stackArray[k];
		float f2 = stackArray[k - 1];
		float re = f1 - f2;
		stackArray[k - 1] = re;
		ActiveArray[k] = false;
		stackArray[k] = 0;
	}
}

inline void oper_multi(float* stackArray, bool* ActiveArray, int k) {
	if (k > 0) {
		float f1 = stackArray[k];
		float f2 = stackArray[k - 1];
		float re = f1 * f2;
		stackArray[k - 1] = re;
		ActiveArray[k] = false;
		stackArray[k] = 0;
	}
}

inline void oper_div(float* stackArray, bool* ActiveArray, int k) {
	if (k > 0) {
		float f1 = stackArray[k];
		float f2 = stackArray[k - 1];
		float re = f1 / f2;
		stackArray[k - 1] = re;
		ActiveArray[k] = false;
		stackArray[k] = 0;
	}
}

inline void oper_pow(float* stackArray, bool* ActiveArray, int k) {
	if (k > 0) {
		float f1 = stackArray[k];
		float f2 = stackArray[k - 1];
		float re = powf(f1, f2);
		stackArray[k - 1] = re;
		ActiveArray[k] = false;
		stackArray[k] = 0;
	}
}

namespace math_expr {
	typedef struct StringStack {
		string str;
		int stack = 0;
	};
	
	class Operator {
	public:
		Operator() :
			Operfunc(nullptr)
		{
		}
		Operator(string o, int nan, bool front, void (*f)(float* stackArray, bool* ActiveArray, int k)) {
			oper_string = o;
			need_argument_num = nan;
			front_apear = front;
			Operfunc = f;
		}
		virtual ~Operator() {}

		string oper_string;
		int need_argument_num = 2;
		bool front_apear = false;

		void (*Operfunc)(float* stackArray, bool* ActiveArray, int k);
	};

	class Expression {
	public:
		static constexpr int NumStackMax = 100;

		Expression() {
			if (OperatorPriority.empty() == 0) {
				OperatorPriority = MakeVector();
			}

			for (int i = 0; i < NumStackMax; i++) {
				numStack[i] = 0;
				numActive[i] = false;
			}
		}
		virtual ~Expression() {}

		void AddBlocks(string exp)
		{
			string b;
			for (int i = 0; i < (int)exp.size(); i++) {
				if (exp.at(i) == ' ') {
					block.push_back(b);
					b.clear();
				}
				else {
					b.push_back(exp.at(i));
				}
			}
			block.push_back(b);
			b.clear();
		}

		bool isConstant(string str) {
			bool b = '0' <= str.at(0) && str.at(0) <= '9';
			b = b || ((str.size() >= 2 && str[0] == '-') && ('0' <= str.at(1) && str.at(1) <= '9'));
			return b;
		}

		bool isVariable(string str) {
			bool b = str[0] == 'x';
			return b;
		}

		bool isNumber(string str) {
			bool b = ('0' <= str.at(0) && str.at(0) <= '9') || str.at(0) == 'x';
			b = b || ((str.size() >= 2 && str[0] == '-') && ('0' <= str.at(1) && str.at(1) <= '9'));
			return b;
		}

		void BlockBake()
		{
			for (int k = 0; k < (int)Expression::OperatorPriority.size(); k++) {
				Operator opera = Expression::OperatorPriority.at(k);
				string oper = opera.oper_string;

				if (opera.front_apear == false && opera.need_argument_num == 2) {
					//이항 연산자의 경우
					if ('a' <= oper.at(0) && oper.at(0) <= 'Z') {
						//함수일 경우 나간다.
					}
					else {
						for (int i = 0; i < (int)block.size(); i++) {
							string s = block.at(i);
							if (s == oper) {
								int startI = 0, endI = 0;
								string s1;
								s1 = block.at(i - 1);
								if (isNumber(s1)) {
									startI = i - 1;
								}
								else if (s1 == ")") {
									int si = i - 2;
									int stacking = 1;
									string te = s1;
									for (si; stacking != 0; si--) {
										te = block.at(si);
										if (te == ")") {
											stacking++;
										}
										else if (te == "(") {
											stacking--;
										}
									}

									++si;

									startI = si;
								}

								string s2;
								s2 = block.at(i + 1);
								if (isNumber(s2)) {
									endI = i + 2;
								}
								else if (s2 == "(") {
									int si = i + 3;
									int stacking = 1;
									string te = s2;
									for (si; stacking != 0; si++) {
										te = block.at(si);
										if (te == "(") {
											stacking++;
										}
										else if (te == ")") {
											stacking--;
										}
									}
									si--;

									endI = si;
								}
								
								if ((startI - 1 >= 0 && endI < block.size()) && block[endI] == ")" && block[startI-1] == "(") {
									block.erase(block.begin() + i);
									block.insert(block.begin() + startI, oper);
								}
								else {
									block.insert(block.begin() + endI, ")");
									block.erase(block.begin() + i);
									block.insert(block.begin() + startI, oper);
									block.insert(block.begin() + startI, "(");
								}
							}
						}
					}
				}
				else if (opera.front_apear == true) {
					//함수일 경우
				}

			}

			for (int i = 0; i < (int)block.size(); i++) {
				if (block.at(i) == ")" || block.at(i) == "(") {
					block.erase(block.begin() + i);
					i--;
					continue;
				}

				if (block.at(i) == ",") {
					block.erase(block.begin() + i);
					i--;
					continue;
				}
			}
		}

		vector<string> block;
		vector<float> argument; // 매개변수 입력 변수
		float Operate()
		{
			for (int i = 0; i < (int)block.size(); i++) {
				int n = block.size() - i - 1; // 역순
				string s = block.at(n);
				if ('0' <= s.at(0) && s.at(0) <= '9') {
					//숫자일 경우
					UpdateNum(s);
				}
				else if (s.size() >= 2 && (s.at(0) == '-' && '0' <= s.at(1) && s.at(0) <= '1')) {
					//숫자일 경우 (음수)
					UpdateNum(s);
				}
				else if ('x' == s.at(0)) {
					// 매개변수일 경우
					s.erase(s.begin());
					int n = stoi(s);
					if ((int)argument.size() - 1 <= n) {
						//매개변수 입력이 되었을 경우
						int k = 0;
						for (int i = 0; i < NumStackMax; i++) {
							if (numActive[i] == false) {
								k = i;
								break;
							}
						}

						numStack[k] = argument[n];
						numActive[k] = true;
					}
					else {
						// 매개변수 입력이 없을 경우
						int k = 0;
						for (int i = 0; i < NumStackMax; i++) {
							if (numActive[i] == false) {
								k = i;
								break;
							}
						}

						numStack[k] = 0;
						numActive[k] = true;
					}
				}
				else {
					//연산자일 경우
					Operation(s);
				}
			}

			float v = numStack[0];
			numStack[0] = 0;
			numActive[0] = false;
			return v;
		}

		static std::vector<Operator> MakeVector()
		{
			std::vector<Operator> v;
			v.push_back(Operator("^", 2, false, oper_pow)); // 0
			v.push_back(Operator("/", 2, false, oper_div)); // 1
			v.push_back(Operator("*", 2, false, oper_multi)); // 2
			v.push_back(Operator("-", 2, false, oper_minus)); // 3
			v.push_back(Operator("+", 2, false, oper_plus)); // 4
			return v;
		}

		static std::vector<Operator> OperatorPriority;

		int GetPriorityIndexOfOperation(string oper) {
			for (int i = 0; i < (int)Expression::OperatorPriority.size(); ++i) {
				if (Expression::OperatorPriority[i].oper_string == oper) {
					return i;
				}
			}
			return -1;
		}

		// 모든 유리수를 정수 분자 분모로 나누는 변환.
		void tr_Ration()
		{
			char denomStr[128] = {};
			char numerStr[128] = {};
			int nindex = 0;

			for (int i = 0; i < (int)block.size(); ++i) {
				denomStr[0] = '1';
				bool isration = false;
				bool isnumd = false;
				int dindex = 0;
				nindex = 0;
				int ten = 0; // 10의 거듭제곱
				for (int k = 0; k < (int)block[i].size(); ++k) {
					char c = block[i][k];
					if ('0' <= c && c <= '9') {
						numerStr[nindex] = c;
						numerStr[nindex + 1] = '\0';
						++nindex;

						if (isration) {
							dindex += 1;
							if (c != '0') {
								isnumd = true;
							}
							++ten;
							denomStr[ten] = '0';
							denomStr[ten + 1] = '\0';
						}
					}
					else if (c == '.') {
						isration = true;
					}
				}

				if (isration && isnumd) {
					block.erase(block.begin() + i);
					block.insert(block.begin() + i, "(");
					block.insert(block.begin() + i + 1, numerStr);
					block.insert(block.begin() + i + 2, "/");
					block.insert(block.begin() + i + 3, denomStr);
					block.insert(block.begin() + i + 4, ")");
				}
				else if(isration && isnumd == false) {
					for (int k = 0; k < dindex + 1;++k) {
						block[i].pop_back();
					}
				}
			}
		}

		// 모든 분수를 최대한 약분한다.
		void tr_ReductionOfFraction()
		{
			for (int i = 0; i < (int)block.size(); ++i) {
				if (block[i] == "/") {
					bool num0b = true;
					bool num1b = true;

					int numerator = 0;
					int denominator = 0;

					for (int k0 = 0; k0 < (int)block[i - 1].size(); ++k0) {
						char c = block[i - 1][k0];
						if (('0' <= c && c <= '9') == false) {
							num0b = false;
						}
					}

					for (int k0 = 0; k0 < (int)block[i + 1].size(); ++k0) {
						char c = block[i + 1][k0];
						if (('0' <= c && c <= '9') == false) {
							num1b = false;
						}
					}

					numerator = atoi(block[i - 1].c_str());
					denominator = atoi(block[i + 1].c_str());
					int minn = min(denominator, numerator);
					int div = 1;
					int tmp = 2;
					for (int i = 2; i < minn / div + 1; ++i) {
						if (((denominator % i) == 0) && ((numerator % i) == 0)) {
							tmp = i;
							numerator /= tmp;
							denominator /= tmp;
							div *= tmp;
							tmp = 2;
							i = 1;
						}
					}

					block.erase(block.begin() + i - 1);
					block.insert(block.begin() + i - 1, to_string(numerator).c_str());
					block.erase(block.begin() + i + 1);
					block.insert(block.begin() + i + 1, to_string(denominator).c_str());
				}
			}
		}

		// 모든 정수를 소인수분해 한다.
		void tr_PrimeFactorization()
		{
			block.insert(block.begin(), "(");
			block.push_back(")");
			for (int i = 0; i < (int)block.size(); ++i) {
				bool isnumber = true;
				for (int k = 0; k < (int)block[i].size(); ++k) {
					char c = block[i][k];
					if (('0' <= c && c <= '9') == false) {
						isnumber = false;
						break;
					}
				}

				if (isnumber) {
					int num = atoi(block[i].c_str());
					int snum = num;
					int div = 1;
					int factorSiz = 0;
					block.erase(block.begin() + i);
					block.insert(block.begin() + i, "(");
					for (int tmp = 2; tmp < num; ++tmp) {
						if (num % tmp == 0) {
							num /= tmp;
							++factorSiz;
							block.insert(block.begin() + i + factorSiz * 2 - 1, to_string(tmp).c_str());
							block.insert(block.begin() + i + factorSiz * 2, "*");
							tmp = 1;
						}
					}

					if (factorSiz > 0) {
						++factorSiz;
						block.insert(block.begin() + i + factorSiz * 2 - 1, to_string(num).c_str());
						block.insert(block.begin() + i + factorSiz * 2, ")");
					}

					if (factorSiz < 2) {
						block.insert(block.begin() + i + factorSiz * 2 + 1, ")");
						block.erase(block.begin() + i);
						block.erase(block.begin() + i);
						block.insert(block.begin() + i, to_string(snum).c_str());
					}
					else {
						i = i + factorSiz * 2 + 2;
					}
				}
			}
			block.erase(block.begin());
			block.pop_back();
		}

		// 거듭제곱분해
		void tr_SpreadingPower()
		{
			for (int i = 1; i < (int)block.size(); ++i) {
				if (block[i] == "^" && block[i - 1] == ")") {
					int stack = 1;
					int start = 0;
					int end = 0;
					for (int k = i - 2; k >= 0; --k) {
						if (block[k] == ")") {
							stack += 1;
						}

						if (block[k] == "(") {
							stack -= 1;
						}

						if (stack == 0) {
							start = k;
							end = i - 1;
							break;
						}
					}

					int quotient = atoi(block[i + 1].c_str());
					for (int k = 0; k < quotient - 1; ++k) {
						for (int k2 = end; k2 >= start; --k2) {
							block.insert(block.begin() + i, block[k2].c_str());
						}
						block.insert(block.begin() + i, "*");
					}

					block.erase(block.begin() + i + (quotient - 1) * (end - start + 2));
					block.erase(block.begin() + i + (quotient - 1) * (end - start + 2));
				}
			}
		}

		// 곱셈의 모든 분배법칙 구조를 풀어해친다.
		void tr_SpreadingDistributionLaw()
		{
			int operindex = GetPriorityIndexOfOperation("*");

			for (int i = 0; i < (int)block.size(); ++i) {
				printf("%d\n", i);
				if (block[i] == "*") {
					bool leftoff = false;
					bool rightoff = false;
					if (block[i + 1] == "(") {
						strarr::Strarr Right = strarr::GetSegmentStrArrFromStrArr(block, i + 1, "(", ")");
						for (int k = 0; k < Right.size(); ++k) {
							for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
								if (Expression::OperatorPriority[o].oper_string == Right[k]) {
									rightoff = true;
									break;
								}
							}

							if (rightoff) {
								break;
							}
						}

						if (block[i - 1] == ")") {
							if (rightoff == false) {
								strarr::Strarr Left = strarr::GetInvSegmentStrArrFromStrArr(block, i - 1, "(", ")");
								for (int k = 0; k < Left.size(); ++k) {
									for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
										if (Expression::OperatorPriority[o].oper_string == Left[k]) {
											leftoff = true;
											break;
										}
									}

									if (leftoff) {
										break;
									}
								}
								if (leftoff == false) {
									continue;
								}
							}
						}
						else {
							if (rightoff == false) {
								continue;
							}
						}
					}
					else if (block[i - 1] == ")") {
						strarr::Strarr Left = strarr::GetInvSegmentStrArrFromStrArr(block, i - 1, "(", ")");
						for (int k = 0; k < Left.size(); ++k) {
							for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
								if (Expression::OperatorPriority[o].oper_string == Left[k]) {
									leftoff = true;
									break;
								}
							}

							if (leftoff) {
								break;
							}
						}

						if (leftoff == false) {
							continue;
						}
					}
					
					if (rightoff) {
						int s = 0;
						int e = 0;

						strarr::Strarr B = strarr::GetSegmentStrArrFromStrArr(block, i + 1, "(", ")");
						vector<strarr::Strarr> Barr;
						strarr::Strarr A;
						if (block[i - 1] == ")") {
							A = strarr::GetInvSegmentStrArrFromStrArr(block, i - 1, "(", ")");
						}
						else {
							for (int k = i - 1; k >= 0; --k) {
								bool b = false;
								for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
									if (Expression::OperatorPriority[o].oper_string == block[k]) {
										b = true;
										break;
									}
								}

								if (block[k] == "(") {
									b = true;
								}

								if (block[k] == ")") {
									strarr::Strarr oc = strarr::GetInvSegmentStrArrFromStrArr(block, k, "(", ")");
									strarr::InsertStrArr(&A, A.size(), oc);
									k -= oc.size() - 1;
									continue;
								}

								if (b == false) {
									A.push_back(block[k].c_str());
								}
								else {
									break;
								}
							}
						}
						s = i - A.size();
						e = i + B.size();

						int start = 0;
						int end = 0;

						B.erase(B.begin());
						B.pop_back();

						strarr::Strarr operlist;
						operlist.push_back("+");

						for (int k = 0; k < (int)B.size(); ++k) {
							bool isoper = false;
							for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
								if (Expression::OperatorPriority[o].oper_string == B[k]) {
									operlist.push_back(B[k].c_str());
									isoper = true;
									break;
								}
							}

							if (isoper == false) {
								if (B[k] == "(") {
									strarr::Strarr oc = strarr::GetSegmentStrArrFromStrArr(B, k, "(", ")");
									k += oc.size() - 1;
								}
							}

							if (isoper) {
								end = k - 1;
								Barr.push_back(strarr::GetStrArrFromStrArr(B, start, end));
								start = k + 1;
							}
						}

						end = B.size() - 1;
						Barr.push_back(strarr::GetStrArrFromStrArr(B, start, end));
						if (Barr.size() == 1) {
							if (Barr[0][0] != "(" || Barr[0][end] != ")") {
								Barr[0].insert(Barr[0].begin(), "(");
								Barr[0].push_back(")");
							}
						}

						strarr::Strarr addexpr;
						for (int k = 0; k < (int)Barr.size(); ++k) {
							addexpr.push_back(operlist[k]);
							strarr::InsertStrArr(&addexpr, addexpr.size(), Barr[k]);
							addexpr.push_back("*");
							strarr::InsertStrArr(&addexpr, addexpr.size(), A);
						}
						addexpr.erase(addexpr.begin());

						if (Barr.size() > 1) {
							addexpr.insert(addexpr.begin(), "(");
							addexpr.push_back(")");
						}
						strarr::DeleteRangeFromStrArr(&block, s, e);
						strarr::InsertStrArr(&block, s, addexpr);
						i = 0;
					}
					else if (leftoff) {
						vector<string> A = strarr::GetInvSegmentStrArrFromStrArr(block, i - 1, "(", ")");
						vector<string> B;
						vector<strarr::Strarr> Aarr;

						for (int k = i + 1; k < (int)block.size(); ++k) {
							bool b = false;
							for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
								if (Expression::OperatorPriority[o].oper_string == block[k]) {
									b = true;
									break;
								}
							}

							if (block[k] == ")") {
								b = true;
							}

							if (block[k] == "(") {
								strarr::Strarr oc = strarr::GetSegmentStrArrFromStrArr(block, k, "(", ")");
								strarr::InsertStrArr(&B, B.size(), oc);
								k += oc.size() - 1;
								continue;
							}

							if (b == false) {
								B.push_back(block[k].c_str());
							}
							else {
								break;
							}
						}

						int s = 0;
						int e = 0;

						s = i - A.size();
						e = i + B.size();

						int start = 0;
						int end = 0;

						A.erase(A.begin());
						A.pop_back();

						strarr::Strarr operlist;
						operlist.push_back("+");
						for (int k = 0; k < (int)A.size(); ++k) {
							bool isoper = false;
							for (int o = operindex + 1; o < (int)Expression::OperatorPriority.size(); ++o) {
								if (Expression::OperatorPriority[o].oper_string == A[k]) {
									operlist.push_back(A[k].c_str());
									isoper = true;
									break;
								}
							}

							if (isoper == false) {
								if (A[k] == "(") {
									strarr::Strarr oc = strarr::GetSegmentStrArrFromStrArr(A, k, "(", ")");
									k += oc.size() - 1;
								}
							}

							if (isoper) {
								end = k - 1;
								Aarr.push_back(strarr::GetStrArrFromStrArr(A, start, end));
								start = k + 1;
							}
						}

						end = A.size() - 1;
						Aarr.push_back(strarr::GetStrArrFromStrArr(A, start, end));
						if (Aarr.size() == 1) {
							if (Aarr[0][0] != "(" || Aarr[0][end] != ")") {
								Aarr[0].insert(Aarr[0].begin(), "(");
								Aarr[0].push_back(")");
							}
						}

						strarr::Strarr addexpr;
						for (int k = 0; k < (int)Aarr.size(); ++k) {
							addexpr.push_back(operlist[k]);
							strarr::InsertStrArr(&addexpr, addexpr.size(), Aarr[k]);
							addexpr.push_back("*");
							strarr::InsertStrArr(&addexpr, addexpr.size(), B);
						}
						addexpr.erase(addexpr.begin());
						if (Aarr.size() > 1) {
							addexpr.insert(addexpr.begin(), "(");
							addexpr.push_back(")");
						}

						strarr::DeleteRangeFromStrArr(&block, s, e);
						strarr::InsertStrArr(&block, s, addexpr);
						i = 0;
					}
				}
			}

			tr_Clean();
		}

		// 불필요한 요소를 모두 삭제한다.
		void tr_Clean()
		{
			//불필요한 괄호정리
			for (int i = 0; i < (int)block.size(); ++i) {
				if (block[i] == "(") {
					bool isdelete = true;
					if (i > 2) {
						string oper = block[i - 1];
						bool b = IsOperationPriorityBig("+", false, oper);
						if (b) {
							isdelete = false;
						}
					}

					strarr::Strarr sa = strarr::GetSegmentStrArrFromStrArr(block, i, "(", ")");
					int end = i + sa.size() - 1;
					if (end < (int)block.size() - 3) {
						string oper = block[end + 1];
						bool b = IsOperationPriorityBig("-", false, oper);
						if (b) {
							isdelete = false;
						}
					}

					if (isdelete) {
						block.erase(block.begin() + end);
						block.erase(block.begin() + i);
						i -= 1;
					}
				}
			}

			//0 * (?) 정리
			while (1) {
				bool allclean_zero = true;
				for (int i = 0; i < (int)block.size(); ++i) {
					if (block[i] == "0") {
						if (i + 1 < block.size() && Cisor3(block[i + 1], "*", "/", "^")) {
							allclean_zero = false;
							break;
						}

						if (i - 1 >= 0 && Cisor3(block[i - 1], "*", "/", "^")) {
							allclean_zero = false;
							break;
						}

						if ((i + 1 < block.size() && i - 1 >= 0) && (block[i - 1] == "(" && block[i + 1] == ")")) {
							block.erase(block.begin() + i + 1);
							block.erase(block.begin() + i - 1);
							allclean_zero = false;
							break;
						}
					}
				}
				if (allclean_zero == false) {
					for (int i = 0; i < (int)block.size(); ++i) {
						if (block[i] == "0") {
							if (i + 2 < block.size()) {
								if (Cisor3(block[i + 1], "*", "/", "^")) {
									if (block[i + 2] == "(") {
										strarr::Strarr A;
										A = strarr::GetSegmentStrArrFromStrArr(block, i + 1, "(", ")");
										int end = i + 1 + A.size();
										strarr::DeleteRangeFromStrArr(&block, i + 1, end);
									}
									else {
										strarr::DeleteRangeFromStrArr(&block, i + 1, i + 2);
									}
								}
							}

							if (i - 2 >= 0) {
								if (block[i - 1] == "*") {
									if (block[i - 2] == ")") {
										strarr::Strarr A;
										A = strarr::GetInvSegmentStrArrFromStrArr(block, i - 1, "(", ")");
										int start = i - 1 - A.size();
										strarr::DeleteRangeFromStrArr(&block, start, i - 1);
									}
									else {
										strarr::DeleteRangeFromStrArr(&block, i - 2, i - 1);
									}
								}
								else if (block[i - 1] == "/") {
									block.erase(block.begin() + i);
									block.insert(block.begin() + i, "err"); // 수식상 에러표시.
								}
								else if (block[i - 1] == "^") {
									if (block[i - 2] == ")") {
										strarr::Strarr A;
										A = strarr::GetInvSegmentStrArrFromStrArr(block, i - 1, "(", ")");
										int start = i - 1 - A.size();
										strarr::DeleteRangeFromStrArr(&block, start, i);
										block.insert(block.begin() + start, "1");
									}
									else {
										strarr::DeleteRangeFromStrArr(&block, i - 2, i);
										block.insert(block.begin() + i - 2, "1");
									}
								}
							}
						}
					}
				}
				else {
					break;
				}
			}

			//불필요한 괄호정리
			for (int i = 0; i < (int)block.size(); ++i) {
				if (block[i] == "(") {
					bool isdelete = true;

					bool allz = true;
					strarr::Strarr sa = strarr::GetSegmentStrArrFromStrArr(block, i, "(", ")");
					for (int k = 0; k < sa.size(); ++k) {
						if (isNumber(sa[k]) && sa[k] != "0") {
							allz = false;
						}

						if (sa[k] == "^") {
							allz = false;
						}
					}

					if (i > 2) {
						string oper = block[i - 1];
						bool b = IsOperationPriorityBig("+", false, oper);
						if (b) {
							isdelete = false;
						}
					}

					
					int end = i + sa.size() - 1;
					if (end < (int)block.size() - 3) {
						string oper = block[end + 1];
						bool b = IsOperationPriorityBig("-", false, oper);
						if (b) {
							isdelete = false;
						}
					}

					if (isdelete || allz) {
						block.erase(block.begin() + end);
						block.erase(block.begin() + i);
						i -= 1;
					}
				}
			}

			//+ 0 + 정리
			for (int i = 0; i < (int)block.size(); ++i) {
				if (block[i] == "0") {
					int c = 0;

					if (i + 2 < block.size()) {
						if (block[i + 1] == "+" || block[i + 1] == "-") {
							++c;
						}
					}

					if (i - 2 >= 0) {
						if (block[i - 1] == "+" || block[i - 1] == "-") {
							++c;
						}
					}

					if (c > 0) {
						block.erase(block.begin() + i);
					}
				}
			}
			for (int i = 0; i < (int)block.size(); ++i) {
				if (block[i] == "+" || block[i] == "-") {
					if (i + 1 >= block.size()) {
						block.pop_back();
						break;
					}
					
					if (isNumber(block[i + 1]) == false && block[i + 1] != "(") {
						block.erase(block.begin() + i);
						--i;
						continue;
					}

					if (i + 1 >= block.size() || ((isNumber(block[i + 1]) == false && block[i + 1] != "(")) 
						|| (i == 0 || (isNumber(block[i - 1]) == false && block[i - 1] != ")"))) {
						block.erase(block.begin() + i);
						--i;
						continue;
					}
				}
			}

			//식 속 모든 항들의 인수들을 찾아 정리 40*x0*2*x0*x1 -> 80*x0^2*x1
			strarr::Strarr PresentSection;
			int ps_start = 0;
			int ps_end = 0;
			for (int i = 0; i < (int)block.size(); ++i) {
				if ((block[i] == "*" || block[i] == "/") || isNumber(block[i])) {
					if (PresentSection.size() == 0) {
						ps_start = i;
					}
					PresentSection.push_back(block[i]);
				}

				if (block[i] == "(") {
					if (PresentSection.size() == 0) {
						ps_start = i;
					}

					strarr::Strarr sarr = strarr::GetSegmentStrArrFromStrArr(block, i, "(", ")");
					strarr::InsertStrArr(&PresentSection, PresentSection.size(), sarr);
				}

				// 덧셈, 뺄셈등이 나오거나 마지막이라 항이 끝날때
				if (IsOperationPriorityBig("*", true, block[i]) || i + 1 >= block.size()) {
					ps_end = i - 1;
					//항 처리

					//차수배열
					vector<StringStack> degree;
					degree.push_back({ "Constant", 1 }); // 상수 부분의 stack은 차수가 아닌 계수를 계산.

					for (int k = 0; k < PresentSection.size(); ++k) {
						if (isNumber(PresentSection[i])) {
							if (isConstant(PresentSection[i])) {
								degree[0].stack *= stoi(PresentSection[i]);
							}
							else if (isVariable(PresentSection[i])) {
								bool already_exist = false;
								for (int k1 = 0; k1 < degree.size(); ++k1) {
									if (degree[k1].str == PresentSection[i]) {
										already_exist = true;
										degree[k1].stack += 1;
									}
								}

								if (already_exist == false) {
									degree.push_back({ PresentSection[i] , 1 });
								}
							}
						}
					}

					strarr::Strarr Result;
					Result.push_back(to_string(degree[0].stack)); Result.push_back("*");
					for (int k = 1; k < degree.size(); ++k) {
						Result.push_back(degree[k].str); Result.push_back("^"); 
						Result.push_back(to_string(degree[k].stack)); Result.push_back("*");
					}
					Result.pop_back();

					strarr::DeleteRangeFromStrArr(&block, ps_start, ps_end);
					strarr::InsertStrArr(&block, ps_start, Result);
				}
			}

			//같은 차수를 가진 항끼리 묶기
		}

		// 곱셈의 모든 분배법칙 구조를 압축한다.
		void tr_CompressDistributionLaw()
		{
		}

		// 인수분해
		void tr_Factorization()
		{
		}

		// 확정된 계수들의 압축 (상수 포함)
		void tr_CoefficientCompression()
		{
		}

		string GetExpressionString()
		{
			string rstr;
			for (int i = 0; i < (int)block.size(); ++i) {
				bool spaceb = false;
				if (block[i] == "+" || block[i] == "-") {
					spaceb = true;
				}

				if (spaceb) {
					rstr.push_back(' ');
				}
				for (int k = 0; k < (int)block[i].size(); ++k) {
					rstr.push_back(block[i][k]);
				}
				if (spaceb) {
					rstr.push_back(' ');
				}
			}
			return rstr;
		}

		bool IsOperationPriorityBig(string oper, bool up, string obj)
		{
			int a = 0, b = 0;
			for (int i = 0; i < (int)Expression::OperatorPriority.size(); ++i) {
				if (Expression::OperatorPriority[i].oper_string == oper) {
					a = i;
				}

				if (Expression::OperatorPriority[i].oper_string == obj) {
					b = i;
				}
			}

			if (up) {
				if (b > a) return true;
				else return false;
			}
			else {
				if (a > b) return true;
				else return false;
			}
		}

		static void Init()
		{
			Expression::OperatorPriority = {};
			Expression::OperatorPriority.push_back(Operator("^", 2, false, oper_pow)); // 0
			Expression::OperatorPriority.push_back(Operator("/", 2, false, oper_div)); // 1
			Expression::OperatorPriority.push_back(Operator("*", 2, false, oper_multi)); // 2
			Expression::OperatorPriority.push_back(Operator("-", 2, false, oper_minus)); // 3
			Expression::OperatorPriority.push_back(Operator("+", 2, false, oper_plus)); // 4
		}

	private:
		float numStack[NumStackMax];
		bool numActive[NumStackMax];

		void UpdateNum(string num)
		{
			float f = stof(num);
			int k = 0;
			for (int i = 0; i < NumStackMax; i++) {
				if (numActive[i] == false) {
					k = i;
					break;
				}
			}

			numStack[k] = f;
			numActive[k] = true;
		}

		void Operation(string operation)
		{
			int k = 0;
			for (int i = 0; i < NumStackMax; i++) {
				if (numActive[i] == false) {
					k = i;
					break;
				}
			}
			k = k - 1;

			for (int i = 0; i < (int)OperatorPriority.size(); i++) {
				if (OperatorPriority.at(i).oper_string == operation) {
					OperatorPriority.at(i).Operfunc(numStack, numActive, k);
				}
			}
		}
	};

	inline int GetPriorityIndexOfOperation(string oper);
};

int math_expr::GetPriorityIndexOfOperation(string oper) {
	for (int i = 0; i < (int)Expression::OperatorPriority.size(); ++i) {
		if (Expression::OperatorPriority[i].oper_string == oper) {
			return i;
		}
	}
	return -1;
}

#define init_mathexpr std::vector<math_expr::Operator> math_expr::Expression::OperatorPriority = math_expr::Expression::MakeVector();

// 유리수 -> 정수 분자 / 정수 분모 로 모두 바꾸는 식연산 (rationTransform)
// 식 안의 모든 분수를 약분하는 식연산
// 식 안의 모든 정수를 소인수분해 하는 식 연산
// 식 안의 모든 곱셈을 분배법칙으로 푸는 식 연산
// 인수분해의 식 연산
// 방정식의 식 연산
// 부등식의 식 연산
// 각 연산에 맞는 성질들
// 덧셈 - 교환법칙
// 곱셈 - 교환법칙, 분배법칙
// 상수 정리