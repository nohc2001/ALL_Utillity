#pragma once
#include "Utill_Math_Expression.h"
#include "Utill_Reform.h"

namespace math_interp {
	typedef struct xyPair {
		float x = 0;
		float y = 0;
	}XYPAIR;

	class LagrangeInterpolation {
	public:
		vector<xyPair> input_param_returns;
		math_expr::Expression e;

		LagrangeInterpolation() {
		}

		virtual ~LagrangeInterpolation() {

		}

		void axy(float x, float y) {
			xyPair p = { x, y };
			input_param_returns.push_back(p);
		}

		string GetExpressionStr() {
			e.block.clear();
			string exp;
			for (int i = 0; i < (int)input_param_returns.size(); ++i) {
				for (int k = 0; k < (int)input_param_returns.size(); ++k) {
					if (i == k) continue;

					float f = input_param_returns[k].x;
					exp.push_back('(');
					exp.push_back(' ');
					exp.push_back('x');
					exp.push_back('0');
					exp.push_back(' ');

					if (f > 0) exp.push_back('-');
					else exp.push_back('+');
					exp.push_back(' ');

					string v = to_string(f);
					for (int i2 = 0; i2 < (int)v.size(); ++i2) {
						exp.push_back(v[i2]);
					}

					exp.push_back(' ');
					exp.push_back(')');

					exp.push_back(' ');
					exp.push_back('*');
					exp.push_back(' ');
				}

				float bj = input_param_returns[i].y;
				float bm = 1;
				float bx = input_param_returns[i].x;
				for (int k = 0; k < (int)input_param_returns.size(); ++k) {
					if (i == k) continue;
					float ff = bx - input_param_returns[k].x;
					bm *= ff;
				}

				exp.push_back('(');
				exp.push_back(' ');

				string strbj = to_string(bj);
				for (int i2 = 0; i2 < (int)strbj.size(); ++i2) {
					exp.push_back(strbj[i2]);
				}

				exp.push_back(' ');
				exp.push_back('/');
				exp.push_back(' ');

				string strbm = to_string(bm);
				for (int i2 = 0; i2 < (int)strbm.size(); ++i2) {
					exp.push_back(strbm[i2]);
				}

				exp.push_back(' ');
				exp.push_back(')');

				if (i < (int)input_param_returns.size() - 1) {
					exp.push_back(' ');
					exp.push_back('+');
					exp.push_back(' ');
				}
			}
			e.AddBlocks(exp);
			e.�м��κ�ȯ();
			e.�ִ���();
			e.�����й�();
			e.����();
			return e.GetExpressionString();
		}

		void Bake() {
			e.BlockBake();
		}

		float GetReturn(float x) {
			if (e.argument.size() != 0) {
				e.argument.clear();
			}
			e.argument.push_back(x);
			return e.Operate();
		}
	};

	inline float Factorial(int n) {
		float v = 1;
		for (int i = 1; i < n + 1; ++i) {
			v *= i;
		}

		return v;
	}

	inline float Combination(int n, int r) {
		return Factorial(n) / Factorial(r) * Factorial(n - r);
	}

	//����Ÿ�� ���׽�
	//B(n, i, t) = nCi * t^i * (1-t)^(n-i)
	//������ �Լ�
	//p(t) = sum(n, i=0) { B(n, i, t) * Pi } // Pi = i��° pair
	//px(t) = sum(n, i=0) { B(n, i, t) * Xi }
	//py(t) = sum(n, i=0) { B(n, i, t) * Yi }
	//px^-1(x) = t; - x�� ������������ ������ �����ϴ�,
	//t�� ����Ž���� �Ͽ� x�� ���ϰ� ���ϴ� �� �ݺ�, ������� ����� x�� ������ �� ������ t�� �����̶� ġ��
	//py(t) �� ������ ���Ѵ�.
	class BezierInterpolation {
	public:
		vector<xyPair> ipr;
		float startx = 0;
		float endx = 0;

		BezierInterpolation() {}
		virtual ~BezierInterpolation() {}

		void axy(float x, float y) {
			xyPair p = { x, y };
			ipr.push_back(p);
		}

		float BezierF(float t, bool isX) {
			int n = ipr.size();
			if (isX) {
				float r = 0;
				for (int i = 0; i < n; ++i) {
					r += Combination(n, i) * powf(t, i) * powf(1.0f - t, n - i) * ipr[i].x;
				}
				return r;
			}
			else {
				float r = 0;
				for (int i = 0; i < n; ++i) {
					r += Combination(n, i) * powf(t, i) * powf(1.0f - t, n - i) * ipr[i].y;
				}
				return r;
			}
		}

		void Bake() {
			for (int i = 0; i < (int)ipr.size(); ++i) {
				for (int k = i + 1; k < (int)ipr.size(); ++k) {
					if (ipr[i].x > ipr[k].x) {
						xyPair p = ipr[i];
						ipr[i] = ipr[k];
						ipr[k] = p;
					}
				}
			}
		}

		float GetReturn(float x) {
			float pd = 0.5f;
			float pivot = 0.5f;
			constexpr float delta = 0.001f;
			while (true) {
				float fx = BezierF(pivot, true);
				if (fabsf(fx - x) <= delta) {
					return BezierF(pivot, false);
				}
				else if (x > fx) {
					pd /= 2;
					pivot += pd;
				}
				else {
					pd /= 2;
					pivot -= pd;
				}
			}
		}
	};

	class FFTInterpolation {
	public:
		FFTInterpolation() {}
		virtual ~FFTInterpolation() {}
	};
}