#include <dif.h>

int main(int argc, char *argv[]) {
	DifImage * img = DifImage::open("test.dif", 2, 2, DifImage::f16Bit);
}
