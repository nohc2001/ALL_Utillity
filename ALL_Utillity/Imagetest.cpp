#include "Utill_ImageFormating.h"

using namespace imgform;

imgform_init

int main() {
	PixelImageObject pio = PixelImageObject(16, 16);
	for (int i = 0; i < 16; ++i) {
		for (int k = 0; k < 16; ++k) {
			pio.SetData(i, k, { (unsigned char)(((i + k) % 2) * 255), 0, 0, (unsigned char)(((i + k) % 2) * 255) });
		}
	}

	pio.rawDataToPNG("testpng.png");
	pio.rawDataToBMP("testpng.bmp");
	return 0;
}