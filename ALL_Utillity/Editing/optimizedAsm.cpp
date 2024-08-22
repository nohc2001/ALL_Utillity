#include <iostream>
using namespace std;
typedef unsigned int ui32;

//asm procedure
extern "C" {
	int __stdcall set_ret(void* ret);
	ui32 __stdcall p_ui32_rev(ui32 x);
}

void Shell() {
	cout << "Hello! ASM!!" << endl;
}

__forceinline ui32 ui32_rev_hd(ui32 x) {
	x = (x & 0x55555555) << 1 | (x >> 1) & 0x55555555;
	x = (x & 0x33333333) << 2 | (x >> 2) & 0x33333333;
	x = (x & 0x0F0F0F0F) << 4 | (x >> 4) & 0x0F0F0F0F;
	x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

int main() {
	ui32 x;
	cin >> x;
	x = ui32_rev_hd(x);
	x = p_ui32_rev(x);
	cout << x << endl;
	return 0;
}