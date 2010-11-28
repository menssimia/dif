#include <dif.h>

#include <cstring>
#include <cstdio>

#define DIM 40

void write() {
	DifImage * img = DifImage::open("test.dif", DIM, DIM, 6);

	unsigned int idx = 0;
	if(img->addChannel("P", DifImage::f64Bit, idx) == false) {
		printf("Error creating channel\n");
	}


	if(img->addChannel("U", DifImage::f8Bit, idx) == false) {
		printf("Error creating channel\n");
	}

	char c[DIM][DIM];

	for(int j=0; j < DIM; j++) {
		for(int k=0; k < DIM; k++) {
			c[j][k] = 0;
		}
	}

	img->dataWrite(idx, 12, c);

	img->sync();
	img->release();
}

void read() {
	DifImage * img = DifImage::open("test.dif");

	if(!img) {
		printf("Error while loading file\n");
		return;
	} 


	printf("I have %d channels:\n", img->channels());

	for(unsigned int i = 0; i < img->channels(); i++) {
		printf("\t%d: %s with type %s of size %d bytes\n", i, img->channelName(i), DifImage::formatToString(img->channelFormat(i)), img->channelSize(i));

		if(strcmp(img->channelName(i), "U") == 0) {
			char c[DIM][DIM];

			img->dataRead(i, 12, c);
		
			for(int j=0; j < DIM; j++) {
				for(int k=0; k < DIM; k++) {
					printf("%d ", c[j][k]);
				}
				printf("\n");
			}
		}
	
	}

	
}

int main(int argc, char *argv[]) {
	if(argc == 2) write();

	read();
}
