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

	NOT_Gate* u_not = ((NOT_Gate*)fm._New(sizeof(NOT_Gate)))->Init(); Init_VPTR<NOT_Gate>(u_not);
	OR_Gate* u_or = ((OR_Gate*)fm._New(sizeof(OR_Gate)))->Init(); Init_VPTR<OR_Gate>(u_or);
	AND_Gate* u_and = ((AND_Gate*)fm._New(sizeof(AND_Gate)))->Init(); Init_VPTR<AND_Gate>(u_and);
	XOR_Gate* u_xor = ((XOR_Gate*)fm._New(sizeof(XOR_Gate)))->Init(); Init_VPTR<XOR_Gate>(u_xor);

	UnitArray.push_back((Unit*)u_not);
	UnitArray.push_back((Unit*)u_or);
	UnitArray.push_back((Unit*)u_and);
	UnitArray.push_back((Unit*)u_xor);
	
	CircutUnit* u_HalfAdder = (CircutUnit*)fm._New(sizeof(CircutUnit)); Init_VPTR<CircutUnit>(u_HalfAdder);
	u_HalfAdder->SetFM((freemem::FM_Model*)&fm);
	u_HalfAdder->SetInputOutputNum(2, 2, 6); // 10
	u_HalfAdder->AddUnit((Unit*)u_xor);
	u_HalfAdder->AddUnit((Unit*)u_and);
	u_HalfAdder->ConnectSetting();
	u_HalfAdder->Connect(0, 4);
	u_HalfAdder->Connect(0, 7);
	u_HalfAdder->Connect(1, 5);
	u_HalfAdder->Connect(1, 8);
	u_HalfAdder->Connect(6, 2);
	u_HalfAdder->Connect(9, 3);
	UnitArray.push_back((Unit*)u_HalfAdder);

	CircutUnit* u_FullAdder = (CircutUnit*)fm._New(sizeof(CircutUnit)); Init_VPTR<CircutUnit>(u_FullAdder);
	u_FullAdder->SetFM((freemem::FM_Model*)&fm);
	u_FullAdder->SetInputOutputNum(3, 2, 22);
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
	UnitArray.push_back((Unit*)u_FullAdder);

	UnitInstance instance = UnitInstance();
	instance.SetFM((freemem::FM_Model*)&fm);
	instance.Init((Unit*)u_FullAdder);
	instance.Input(0, true); instance.Input(1, true); instance.Input(2, false);
	cout << GetStrBitRange(instance.Execute()) << endl;
	cout << GetStrBitRange(instance.Execute()) << endl;
	cout << GetStrBitRange(instance.Execute()) << endl;
	//UnitArray.push_back()
	return 0;
}