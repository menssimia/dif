#include <dif.h>

#include <cstring>
#include <cstdio>

void write() {
	DifImage * img = DifImage::open("test.dif", 2, 2, 6);

	img->writeMeta("MyKey", "MyValue");
	img->writeMeta("MyKey1", "MyValue4");
	img->writeMeta("MyKey2", "MyValue5");

	img->writeMeta("emptyKey", "");

	unsigned int idx = 0;
	if(img->addChannel("P", DifImage::f64Bit, idx) == false) {
		printf("Error creating channel\n");
	}

	printf("Ps Index is %d\n", idx);

	if(img->addChannel("U", DifImage::fDReal, idx) == false) {
		printf("Error creating channel\n");
	}

	printf("Us Index is %d\n", idx);


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
	printf("attribute \"emptyKey\" is %s\n", img->meta("emptyKey", "I am really empty"));

	printf("compression is %d\n", img->compression());

	printf("I have %d channels:\n", img->channels());

	for(unsigned int i = 0; i < img->channels(); i++) {
	printf("\t%d: %s with type %s of size %d bytes\n", i, img->channelName(i), DifImage::formatToString(img->channelFormat(i)), img->channelSize(i));		
	} 
}

int main(int argc, char *argv[]) {
	if(argc == 2) write();

	read();
}
