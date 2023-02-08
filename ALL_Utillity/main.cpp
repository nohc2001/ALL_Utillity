#include <iostream>
#include <time.h>
#include "Utill_VirtualUnit.h"

using namespace virtu_unit;

//지금 4~40 바이트 사이즈의 메모리 할당이 참 느린데 이건 FM_Model1에서 lifecheck가 체크하는 바이트를 늘려서 극복가능할 것 같음.
//어딘가에 비용함수가 있었음.

constexpr int fmsize = 100000; //100 KB
freemem::FM_Model0 fm;
void CircuitInit() {
	fm.SetHeapData(new byte8[fmsize], fmsize);

	UnitArray.NULLState();
	UnitArray.SetFM((freemem::FM_Model*)&fm);
	UnitArray.Init(100);

	NOT_Gate* u_not = vins_New(fm, NOT_Gate, u_not);
	OR_Gate* u_or = vins_New(fm, OR_Gate, u_or);
	AND_Gate* u_and = vins_New(fm, AND_Gate, u_and);
	XOR_Gate* u_xor = vins_New(fm, XOR_Gate, u_xor);

	UnitArray.push_back((Unit*)u_not); // 0
	UnitArray.push_back((Unit*)u_or); // 1
	UnitArray.push_back((Unit*)u_and); // 2
	UnitArray.push_back((Unit*)u_xor); // 3

	CircutUnit* u_HalfAdder = vins_New(fm, CircutUnit, u_HalfAdder);
	u_HalfAdder->SetFM((freemem::FM_Model*)&fm);
	u_HalfAdder->SetInputOutputNum(2, 2, 6); // 10 (2byte)
	u_HalfAdder->AddUnit((Unit*)u_xor);
	u_HalfAdder->AddUnit((Unit*)u_and);
	u_HalfAdder->ConnectSetting();
	u_HalfAdder->Connect(0, 4);
	u_HalfAdder->Connect(0, 7);
	u_HalfAdder->Connect(1, 5);
	u_HalfAdder->Connect(1, 8);
	u_HalfAdder->Connect(6, 2);
	u_HalfAdder->Connect(9, 3);
	u_HalfAdder->RequireSimul(1);
	UnitArray.push_back((Unit*)u_HalfAdder); // 4

	CircutUnit* u_FullAdder = vins_New(fm, CircutUnit, u_FullAdder);
	u_FullAdder->SetFM((freemem::FM_Model*)&fm);
	u_FullAdder->SetInputOutputNum(3, 2, 22); // 27 (4byte)
	u_FullAdder->AddUnit((Unit*)u_HalfAdder);
	u_FullAdder->AddUnit((Unit*)u_HalfAdder);
	u_FullAdder->AddUnit((Unit*)u_or);
	u_FullAdder->ConnectSetting();
	u_FullAdder->Connect(0, 5);
	u_FullAdder->Connect(1, 6);
	u_FullAdder->Connect(2, 16);
	u_FullAdder->Connect(7, 15);
	u_FullAdder->Connect(8, 26);
	u_FullAdder->Connect(18, 25);
	u_FullAdder->Connect(17, 3);
	u_FullAdder->Connect(27, 4);
	u_FullAdder->RequireSimul(1);
	UnitArray.push_back((Unit*)u_FullAdder); //5

	CircutUnit* u_8bitAdder = vins_New(fm, CircutUnit, u_8bitAdder);
	u_8bitAdder->SetFM((freemem::FM_Model*)&fm);
	u_8bitAdder->SetInputOutputNum(17, 9, 216); // 242 (31byte)
	u_8bitAdder->AddUnit((Unit*)u_FullAdder); //in : 26+27i 27+27i 28+27i > out : 29+27i 30+27i
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->AddUnit((Unit*)u_FullAdder);
	u_8bitAdder->ConnectSetting();
	int mul = u_FullAdder->GetMaxWire();
	for (int i = 0; i < 8; ++i) {
		u_8bitAdder->Connect(i, 26 + mul * i);
		u_8bitAdder->Connect(i + 8, 27 + mul * i);
	}
	u_8bitAdder->Connect(16, 28 + mul);
	for (int i = 0; i < 7; ++i) {
		u_8bitAdder->Connect(30 + mul * i, 28 + mul * (i + 1));
	}
	u_8bitAdder->Connect(30 + mul * 7, 25);
	for (int i = 0; i < 8; ++i) {
		u_8bitAdder->Connect(29 + mul * i, 17 + i);
	}
	u_8bitAdder->RequireSimul(1);
	UnitArray.push_back((Unit*)u_8bitAdder); // 6

	mul = u_8bitAdder->GetMaxWire();
	CircutUnit* u_16bitAdder = vins_New(fm, CircutUnit, u_16bitAdder);
	u_16bitAdder->SetFM((freemem::FM_Model*)&fm);
	u_16bitAdder->SetInputOutputNum(33, 17, mul * 2); // 242 (31byte)
	u_16bitAdder->AddUnit((Unit*)u_8bitAdder); //in : 50+muli ~ 65+muli, 66+muli(carry in)  > out : 67+muli ~ 74+muli 75+muli
	u_16bitAdder->AddUnit((Unit*)u_8bitAdder);
	u_16bitAdder->ConnectSetting();

	u_16bitAdder->Connect(32, u_16bitAdder->GetUnitIndex(0, 'i', 16));
	u_16bitAdder->Connect(u_16bitAdder->GetUnitIndex(0, 'o', 8), u_16bitAdder->GetUnitIndex(1, 'i', 16));
	u_16bitAdder->Connect(u_16bitAdder->GetUnitIndex(1, 'o', 8), 49);
	
	for (int i = 0; i < 8; ++i) {
		u_16bitAdder->Connect(i, u_16bitAdder->GetUnitIndex(0, 'i', i));
		u_16bitAdder->Connect(i+16, u_16bitAdder->GetUnitIndex(0, 'i', i+8));
	}
	for (int i = 8; i < 16; ++i) {
		u_16bitAdder->Connect(i, u_16bitAdder->GetUnitIndex(1, 'i', i-8));
		u_16bitAdder->Connect(i + 16, u_16bitAdder->GetUnitIndex(1, 'i', i));
	}
	for (int i = 0; i < 8; ++i) {
		u_16bitAdder->Connect(u_16bitAdder->GetUnitIndex(0, 'o', i), 33 + i);
		u_16bitAdder->Connect(u_16bitAdder->GetUnitIndex(1, 'o', i), 33 + i + 8);
	}

	u_16bitAdder->RequireSimul(1);
	UnitArray.push_back((Unit*)u_16bitAdder); // 7
}

int main() {
	CircuitInit();
	UnitInstance instance = UnitInstance();
	instance.SetFM((freemem::FM_Model*)&fm);
	instance.Init((Unit*)UnitArray[6]);
	byte8 data[2] = { 0B00000110, 0B00000011 };
	instance.Input(0, &data[0], 8); instance.Input(8, &data[1], 8); instance.Input(16, false);
	cout << GetStrBitRange(instance.Execute()) << endl;
	//UnitArray.push_back()
	return 0;
}