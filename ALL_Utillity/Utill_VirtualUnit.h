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
			if (GetByte(br.arr[index(i)], loc(i))) {
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

	class Unit {
	public:
		int inputNum = 0;
		int outputNum = 0;
		int materialNum = 0;

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
		}

		virtual ~NOT_Gate() {

		}

		NOT_Gate* Init() {
			inputNum = 1;
			outputNum = 1;
			materialNum = 0;
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
			SetByte(output.arr[index(output.start)], loc(output.start), (GetByte(input.arr[index(input.start)], loc(input.start)) || GetByte(input.arr[index(input.start + 1)], loc(input.start + 1))))
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
		}

		virtual ~AND_Gate() {

		}

		AND_Gate* Init() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			return this;
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			SetByte(output.arr[index(output.start)], loc(output.start), GetByte(input.arr[index(input.start)], loc(input.start)) && GetByte(input.arr[index(input.start + 1)], loc(input.start + 1)));
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
		}

		virtual ~XOR_Gate() {

		}

		XOR_Gate* Init() {
			inputNum = 2;
			outputNum = 1;
			materialNum = 0;
			return this;
		}

		virtual void GetResult(BitRange input, BitRange output, BitRange material) {
			SetByte(output.arr[index(output.start)], loc(output.start), (GetByte(input.arr[index(input.start)], loc(input.start)) != GetByte(input.arr[index(input.start + 1)], loc(input.start + 1))))
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
		}

		virtual ~CircutUnit() {

		}

		freemem::FM_Model* FM = nullptr;
		freemem::InfiniteArray<Unit*> units; // 재료가 되는 유닛들
		freemem::InfiniteArray<BitConnect> connecting; // 각 wire들이 어떤 유닛들의 wire들로 input 되는지

		void Init() {
			inputNum = 0;
			outputNum = 0;
			materialNum = 0;
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
			int ion = inputNum + outputNum;

			string dbg_input = GetStrBitRange(input);
			string dbg_output = GetStrBitRange(output);
			string dbg_material = GetStrBitRange(material);
			for (int i = 0; i < inputNum; ++i) {
				bool wv = GetByte(input.arr[index(i)], loc(i));
				for (int k = 0; k < connecting[i].size(); ++k) {
					int wadd = connecting[i][k];
					SetByte(input.arr[index(wadd)], loc(wadd), wv);
				}
			}
			dbg_input = GetStrBitRange(input);
			dbg_output = GetStrBitRange(output);
			dbg_material = GetStrBitRange(material);

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
				u->GetResult(in, out, mater);

				ion = ion + u->inputNum + u->outputNum;
				ion += u->materialNum;
			}

			dbg_input = GetStrBitRange(input);
			dbg_output = GetStrBitRange(output);
			dbg_material = GetStrBitRange(material);
			for (int i = 0; i < connecting.size(); ++i) {
				bool wv = GetByte(input.arr[index(i)], loc(i));
				for (int k = 0; k < connecting[i].size(); ++k) {
					int wadd = connecting[i][k];
					SetByte(input.arr[index(wadd)], loc(wadd), wv);
				}
			}
			dbg_input = GetStrBitRange(input);
			dbg_output = GetStrBitRange(output);
			dbg_material = GetStrBitRange(material);
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
			}
		}

		void SetFM(freemem::FM_Model* fm) {
			FM = fm;
		}

		void Input(int wirenum, bool enable) {
			SetByte(wire.Arr[index(wirenum)], loc(wirenum), enable);
		}

		BitRange Execute() {
			CircutUnit* origin = dynamic_cast<CircutUnit*>(Original);
			BitRange in, out, material;
			in.arr = &wire.Arr[0];
			in.start = 0;
			in.end = Original->inputNum - 1;
			out.arr = &wire.Arr[index(in.end + 1)];
			out.start = in.end + 1;
			out.end = in.end + Original->outputNum;
			material.arr = &wire.Arr[index(out.end + 1)];
			material.start = out.end + 1;
			material.end = out.end + Original->materialNum;

			Original->GetResult(in, out, material);
			return out;
		}
	};
}