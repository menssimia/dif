#include <dif.h>

#include <cstring>
#include <cstdio>

void write() {
	DifImage * img = DifImage::open("test.dif", 2, 2, DifImage::f16Bit);

	img->writeMeta("MyKey", "MyValue");
	img->writeMeta("MyKey1", "MyValue4");
	img->writeMeta("MyKey2", "MyValue5");

	img->sync();
	img->release();
}

void read() {
	DifImage * img = DifImage::open("test.dif");

	if(!img) {
		printf("Error while loading file\n");
		return;
	}

	printf("attribute \"MyKey\" is %s\n", img->meta("MyKey"));
}

int main(int argc, char *argv[]) {
	if(argc == 2) write();

	read();
}
