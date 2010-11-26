#include <dif.h>

int main(int argc, char *argv[]) {
	DifImage * img = DifImage::open("test.dif", 2, 2, DifImage::f16Bit);

	img->writeAttribute("MyKey", "MyValue");
	img->writeAttribute("MyKey1", "MyValue4");
	img->writeAttribute("MyKey2", "MyValue5");

	img->sync();
}
