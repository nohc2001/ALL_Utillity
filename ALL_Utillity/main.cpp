#include <iostream>
#include <time.h>
#include "Utill_VirtualUnit.h"

using namespace virtu_unit;

//지금 4~40 바이트 사이즈의 메모리 할당이 참 느린데 이건 FM_Model1에서 lifecheck가 체크하는 바이트를 늘려서 극복가능할 것 같음.
//어딘가에 비용함수가 있었음.

int main() {
	constexpr int fmsize = 100000; //100 KB
	freemem::FM_Model0 fm;
	fm.SetHeapData(new byte8[fmsize], fmsize);
	
	UnitArray.NULLState();
	UnitArray.SetFM((freemem::FM_Model*)&fm);
	UnitArray.Init(100);
	
	NOT_Gate* u_not = vins_New(fm, NOT_Gate, u_not);
	OR_Gate* u_or = vins_New(fm, OR_Gate, u_or);
	AND_Gate* u_and = vins_New(fm, AND_Gate, u_and);
	XOR_Gate* u_xor = vins_New(fm, XOR_Gate, u_xor);

	UnitArray.push_back((Unit*)u_not);
	UnitArray.push_back((Unit*)u_or);
	UnitArray.push_back((Unit*)u_and);
	UnitArray.push_back((Unit*)u_xor);
	
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
	UnitArray.push_back((Unit*)u_HalfAdder);

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
	u_FullAdder->RequireSimul(2);
	UnitArray.push_back((Unit*)u_FullAdder);

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
	for (int i = 0; i < 8; ++i) {
		u_8bitAdder->Connect(i, 26 + 27 * i);
		u_8bitAdder->Connect(i+8, 27 + 27 * i);
	}
	u_8bitAdder->Connect(16, 28 + 27);
	for (int i = 0; i < 7; ++i) {
		u_8bitAdder->Connect(30 + 27 * i, 28 + 27 * i);
	}
	u_8bitAdder->Connect(30 + 27 * 7, 25);
	for (int i = 0; i < 8; ++i) {
		u_8bitAdder->Connect(29 + 27 * i, 17 + i);
	}
	u_8bitAdder->RequireSimul(2);
	UnitArray.push_back((Unit*)u_8bitAdder);

	UnitInstance instance = UnitInstance();
	instance.SetFM((freemem::FM_Model*)&fm);
	instance.Init((Unit*)u_8bitAdder);
	byte8 data[2] = { 0B00000001, 0B00000011 };
	instance.Input(0, &data[0], 8); instance.Input(8, &data[1], 8); instance.Input(16, false);
	cout << GetStrBitRange(instance.Execute()) << endl;
	//UnitArray.push_back()
	return 0;
}