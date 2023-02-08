#pragma once
#include "Utill_ConditionDefines.h"
#include "Utill_FreeMemory.h"

namespace virtu_unit {
#define index(n) ((n)/8)
#define loc(n) ((n)%8)

	typedef struct BitRange {
		byte8* arr;
		int start = 0;
		int end = 0;
	};

	inline string GetStrBitRange(BitRange br) {
		string str = "";
		if (br.start > br.end) return str;
		int i = br.start;
		while (1) {
			if (_GetByte(br.arr[index(i)], loc(i))) {
				str.push_back('1');
			}
			else {
				str.push_back('0');
			}

			if (i == br.end) {
				break;
			}
			
			++i;
		}

		return str;
	}

	typedef enum class UnitTypes{
		NOT = 1,
		OR = 2,
		AND = 3,
		XOR = 4,
		CIRCUTUNIT = 5
	};

	class Unit {
	public:
		int inputNum = 0;
		int outputNum = 0;
		int materialNum = 0;
		UnitTypes t;

		Unit() :
			inputNum(0), outputNum(0), materialNum(0)
		{
		}

		virtual ~Unit() {}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {

		}

		virtual int GetMaxWire() {
			return inputNum + outputNum;
		}

		
	};

	typedef freemem::InfiniteArray<int> BitConnect;

	static freemem::InfiniteArray<Unit*> UnitArray;

	constexpr unsigned char byte_enable_stair[8] = {
		0B00000000,
		0B10000000,
		0B11000000,
		0B11100000,
		0B11110000,
		0B11111000,
		0B11111100,
		0B11111110
	};

	constexpr unsigned char byte_disable_stair[8] = {
		0B11111111,
		0B01111111,
		0B00111111,
		0B00011111,
		0B00001111,
		0B00000111,
		0B00000011,
		0B00000001
	};

	constexpr unsigned char byte_dot1_stair[8] = {
		0B10000000,
		0B01000000,
		0B00100000,
		0B00010000,
		0B00001000,
		0B00000100,
		0B00000010,
		0B00000001
	};

	constexpr unsigned char byte_dot0_stair[8] = {
		0B01111111,
		0B10111111,
		0B11011111,
		0B11101111,
		0B11110111,
		0B11111011,
		0B11111101,
		0B11111110
	};

	class NOT_Gate : Unit {
	public:
		NOT_Gate()
		{
			inputNum = 1;
			outputNum = 1;
			materialNum = 0;
			t = UnitTypes::NOT;
		}

		virtual ~NOT_Gate() {

		}

		NOT_Gate* Init() {
			inputNum = 1;
			outputNum = 1;
			materialNum = 0;
			t = UnitTypes::NOT;
			return this;
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			int inindex = input.start / 8;
			int inloc = input.start % 8;
			int outindex = output.start / 8;
			int outloc = output.start % 8;
			byte8 b0 = ((input.arr[inindex] ^ byte_dot1_stair[inloc]) >> (outloc - inloc)) & byte_dot1_stair[outloc];
			byte8 b1 = output.arr[outindex] & byte_dot0_stair[outloc];
			output.arr[outindex] = b0 | b1;
		}

		virtual int GetMaxWire() {
			return inputNum + outputNum;
		}
	};

	class OR_Gate : Unit {
	public:
		OR_Gate() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
		}

		virtual ~OR_Gate() {

		}

		OR_Gate* Init() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			return this;
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			bool b0 = _GetByte(input.arr[index(input.start)], loc(input.start));
			bool b1 = _GetByte(input.arr[index(input.start + 1)], loc(input.start + 1));
			_SetByte(output.arr[index(output.start)], loc(output.start), b0 || b1);
			//SetByte(output.arr[index(output.start)], loc(output.start), (GetByte(input.arr[index(input.start)], loc(input.start)) || GetByte(input.arr[index(input.start + 1)], loc(input.start + 1))))
		}

		virtual int GetMaxWire() {
			return inputNum + outputNum;
		}
	};

	class AND_Gate : Unit {
	public:
		AND_Gate() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			t = UnitTypes::AND;
		}

		virtual ~AND_Gate() {

		}

		AND_Gate* Init() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			t = UnitTypes::AND;
			return this;
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			bool b0 = _GetByte(input.arr[index(input.start)], loc(input.start));
			bool b1 = _GetByte(input.arr[index(input.start + 1)], loc(input.start + 1));
			_SetByte(output.arr[index(output.start)], loc(output.start), b0 && b1);
		}

		virtual int GetMaxWire() {
			return inputNum + outputNum;
		}
	};

	class XOR_Gate : Unit {
	public:
		XOR_Gate() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			t = UnitTypes::XOR;
		}

		virtual ~XOR_Gate() {

		}

		XOR_Gate* Init() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			t = UnitTypes::XOR;
			return this;
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			bool b0 = _GetByte(input.arr[index(input.start)], loc(input.start));
			bool b1 = _GetByte(input.arr[index(input.start + 1)], loc(input.start + 1));
			_SetByte(output.arr[index(output.start)], loc(output.start), b0 != b1);
			//SetByte(output.arr[index(output.start)], loc(output.start), (GetByte(input.arr[index(input.start)], loc(input.start)) != GetByte(input.arr[index(input.start + 1)], loc(input.start + 1))))
		}

		virtual int GetMaxWire() {
			return inputNum + outputNum;
		}
	};

	class CircutUnit : Unit {
	public:
		CircutUnit() {
			inputNum = 0;
			outputNum = 0;
			materialNum = 0;
			t = UnitTypes::CIRCUTUNIT;
		}

		virtual ~CircutUnit() {
		}

		freemem::FM_Model* FM = nullptr;
		freemem::InfiniteArray<Unit*> units; // 재료가 되는 유닛들
		freemem::InfiniteArray<BitConnect> connecting; // 각 wire들이 어떤 유닛들의 wire들로 input 되는지
		int require_simulation_time = 1;

		CircutUnit* Init() {
			inputNum = 0;
			outputNum = 0;
			materialNum = 0;
			t = UnitTypes::CIRCUTUNIT;
			return this;
		}

		void RequireSimul(int n) {
			require_simulation_time = n;
		}

		void SetFM(freemem::FM_Model* fm) {
			FM = fm;
			units.NULLState();
			units.SetFM(fm);
			units.Init(2);
			connecting.NULLState();
			connecting.SetFM(fm);
			connecting.Init(2);
		}

		virtual int GetMaxWire() {
			materialNum = 0;
			int m = inputNum + outputNum;
			for (int i = 0; i < units.size(); ++i) {
				materialNum += units[i]->GetMaxWire();
			}
			m += materialNum;
			return m;
		}

		// 이 함수들은 새로운 부품을 만들때 차례대로 호출하라.
		// 
		//먼저 인풋, 아웃풋을 몇비트 받을 건지 결정한다.
		void SetInputOutputNum(int input, int output, int material) {
			inputNum = input;
			outputNum = output;
			materialNum = material;
		}

		//재료가 되는 유닛들을 모두 담는다.
		void AddUnit(Unit* sampleUnit) {
			units.push_back(sampleUnit);
		}

		// 커낵터 준비를 한다.
		void ConnectSetting() {
			connecting.NULLState();
			connecting.SetFM(FM);
			connecting.SetVPTR();
			int n = GetMaxWire();
			connecting.Init(n);
			for (int i = 0; i < n; ++i) {
				BitConnect bc;
				bc.NULLState();
				bc.SetFM(FM);
				bc.SetVPTR();
				bc.Init(1);
				connecting.push_back(bc);
			}
		}

		// 연결한다.
		void Connect(int ind, int connectingI) {
			if (Cisin_same(0, ind, connecting.size()) && Cisin_same(0, connectingI, connecting.size())) {
				connecting[ind].push_back(connectingI);
			}
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			for (int simul = 0; simul < require_simulation_time; ++simul) {
				int ion = inputNum + outputNum + input.start;

				string dbg = GetStringData(input.arr, input.start);
				for (int i = 0; i < inputNum; ++i) {
					bool wv = _GetByte(input.arr[index(i + input.start)], loc(i + input.start));
					for (int k = 0; k < connecting[i].size(); ++k) {
						int wadd = connecting[i][k] + input.start;
						input.arr[index(wadd)] = freemem::SetByte8(input.arr[index(wadd)], loc(wadd), wv);
					}
				}
				dbg = GetStringData(input.arr, input.start);

				for (int i = 0; i < units.size(); ++i) {
					Unit* u = units[i];
					BitRange in, out, mater;
					in.arr = &input.arr[index(ion)];
					in.start = ion % 8;
					in.end = ion % 8 + u->inputNum - 1;
					out.arr = &input.arr[index(ion + u->inputNum)];
					out.start = (ion + u->inputNum) % 8;
					out.end = (ion + u->inputNum) % 8 + u->outputNum - 1;
					mater.arr = &input.arr[index(ion + u->inputNum + u->outputNum)];
					mater.start = (ion + u->inputNum + u->outputNum) % 8;
					mater.end = (ion + u->inputNum + u->outputNum) % 8 + u->materialNum - 1;

					if (u->t == UnitTypes::CIRCUTUNIT) {
						dbg = ((CircutUnit*)u)->GetStringData(in.arr, in.start);
					}

					u->GetResult(in, out, mater);

					if (u->t == UnitTypes::CIRCUTUNIT) {
						dbg = ((CircutUnit*)u)->GetStringData(in.arr, in.start);
					}

					int j = ion + u->inputNum;
					for (int j1 = 0; j1 < u->outputNum; ++j1) {
						bool wv = _GetByte(input.arr[index(j)], loc(j));
						for (int k = 0; k < connecting[j - input.start].size(); ++k) {
							int wadd = connecting[j - input.start][k] + input.start;
							input.arr[index(wadd)] = freemem::SetByte8(input.arr[index(wadd)], loc(wadd), wv);
						}
						j += 1;
					}
					
					dbg = GetStringData(input.arr, input.start);

					ion = ion + u->inputNum + u->outputNum;
					ion += u->materialNum;
				}

				dbg = GetStringData(input.arr, input.start);

				for (int i = 0; i < connecting.size(); ++i) {
					bool wv = _GetByte(input.arr[index(i + input.start)], loc(i + input.start));
					for (int k = 0; k < connecting[i].size(); ++k) {
						int wadd = connecting[i][k] + input.start;
						input.arr[index(wadd)] = freemem::SetByte8(input.arr[index(wadd)], loc(wadd), wv);
					}
				}

				dbg = GetStringData(input.arr, input.start);
			}
		}

		string GetStringData(byte8* start, int loc) {
			string str;
			str.append("[i");
			int ion = loc;
			BitRange in, out, mater;
			in.arr = &start[index(ion)];
			in.start = ion % 8;
			in.end = ion % 8 + inputNum - 1;
			out.arr = &start[index(ion + inputNum)];
			out.start = (ion + inputNum) % 8;
			out.end = (ion + inputNum) % 8 + outputNum - 1;
			mater.arr = &start[index(ion + inputNum + outputNum)];
			mater.start = (ion + inputNum + outputNum) % 8;
			mater.end = (ion + inputNum + outputNum) % 8 + materialNum - 1;
			str.append(GetStrBitRange(in));
			str.append(" o");
			str.append(GetStrBitRange(out));
			str.append(" m[");
			byte8* st = mater.arr;
			int stloc = mater.start;
			for (int i = 0; i < units.size(); ++i) {
				str.append("u");
				char n[20] = {};
				_itoa_s(i, n, 10);
				str.append(n);
				str.append(":[");
				if (units[i]->t == UnitTypes::CIRCUTUNIT) {
					CircutUnit* cu = (CircutUnit*)units[i];
					str.append(cu->GetStringData(st, stloc));
					stloc = stloc + cu->GetMaxWire();
					st += stloc / 8;
					stloc = stloc % 8;
				}
				else {
					Unit* u = units[i];
					str.append("[i");
					BitRange in, out;
					int l = stloc;
					in.arr = &st[index(l)];
					in.start = l % 8;
					in.end = l % 8 + u->inputNum - 1;
					out.arr = &st[index(l + u->inputNum)];
					out.start = (l + u->inputNum) % 8;
					out.end = (l + u->inputNum) % 8 + u->outputNum - 1;
					str.append(GetStrBitRange(in));
					str.append(" o");
					str.append(GetStrBitRange(out));
					str.append("]");
					stloc = stloc + u->inputNum + u->outputNum;
					st += stloc / 8;
					stloc = stloc % 8;
				}
				str.append("]] ");
			}
			return str;
		}

		// 이 부품 내부의 유닛의 io 인덱스 번호를 봔환한다.
		int GetUnitIndex(int unitNum, char io, int i) {
			int re = inputNum + outputNum;

			for (int i = 0; i < unitNum; ++i) {
				re += units[i]->GetMaxWire();
			}

			if (io == 'i') {
				return re + i;
			}
			else if (io == 'o') {
				return re + units[unitNum]->inputNum + i;
			}

			return re;
		}

		int GetIOIndex(char io, int i) {
			if (io == 'i') {
				return i;
			}
			else if (io == 'o') {
				return inputNum + i;
			}
		}
	};

	class UnitInstance {
	public:
		freemem::FM_Model* FM;
		freemem::BitArray wire;
		Unit* Original = nullptr;


		UnitInstance() :
			Original(nullptr)
		{}

		virtual ~UnitInstance() {

		}

		void Init(Unit* origin) {
			Original = origin;
			if (Original != nullptr) {
				int m = Original->GetMaxWire();
				wire = freemem::BitArray(FM, m);
				wire.SetUp(m);
				for (int i = 0; i < wire.bit_arr_size; ++i) {
					wire.Arr[index(i)] = freemem::SetByte8(wire.Arr[index(i)], loc(i), false);
				}
			}
		}

		void SetFM(freemem::FM_Model* fm) {
			FM = fm;
		}

		void Input(int wirenum, bool enable) {
			_SetByte(wire.Arr[index(wirenum)], loc(wirenum), enable);
		}

		void Input(int wirenum, byte8* data, int length) {
			for (int i = wirenum; i < wirenum + length; ++i) {
				if (_GetByte(data[index(i-wirenum)], loc(i-wirenum))) {
					_SetByte(wire.Arr[index(i)], loc(i), true);
				}
				else {
					_SetByte(wire.Arr[index(i)], loc(i), false);
				}
			}
		}

		BitRange Execute() {
			CircutUnit* origin = dynamic_cast<CircutUnit*>(Original);
			BitRange in, out, material;
			in.arr = &wire.Arr[0];
			in.start = 0;
			in.end = Original->inputNum - 1;
			out.arr = &wire.Arr[index(Original->inputNum)];
			out.start = loc(Original->inputNum);
			out.end = loc(Original->inputNum) + Original->outputNum - 1;
			material.arr = &wire.Arr[index(Original->inputNum + Original->outputNum)];
			material.start = loc(out.end + 1);
			material.end = loc(out.end + 1) + Original->materialNum - 1;

			Original->GetResult(in, out, material);
			return out;
		}
	};
}