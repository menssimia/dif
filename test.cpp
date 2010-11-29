#include <dif.h>

#include <cstring>
#include <cstdio>

#include <cassert>

#define DIM 4

void write() {
	DifImage * img = DifImage::open("test.dif", DIM, DIM, 6);

	unsigned int idx = 0;
	if(img->addChannel("P", DifImage::f64Bit, idx) == false) {
		printf("Error creating channel\n");
	}


	if(img->addChannel("U", DifImage::f32Bit, idx) == false) {
		printf("Error creating channel\n");
	}

	int c[DIM][DIM];

	for(int j=0; j < DIM; j++) {
		for(int k=0; k < DIM; k++) {
			c[j][k] = 0;
		}
	}

	assert(img->dataWrite(idx, 0, c));

	c[1][1] = 71;

	assert(img->dataWrite(idx, 12, c));

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
			int c[DIM][DIM];

			for(int j=0; j < DIM; j++) {
				for(int k=0; k < DIM; k++) {
					c[j][k] = 0;
				}
			}

			assert(img->dataRead(i, 12, c));
		
			for(int j=0; j < DIM; j++) {
				for(int k=0; k < DIM; k++) {
					printf("%d ", c[j][k]);
				}
				printf("\n");
			}
		}
	
	}

	
}

void datatest() {
#define RES 4

DifImage * img = DifImage::open("fourXfourExample.dif", RES, RES, 0);

unsigned int r, g, b;

img->addChannel("R", DifImage::f16Bit, r);
img->addChannel("G", DifImage::f16Bit, g);
img->addChannel("B", DifImage::f16Bit, b);

short somedata[RES][RES] = {
{1,2,3,4},
{5,6,7,8},
{9,10,11,12},
{13,14,15, 16}
};

// Write some data to depth 0.0
img->dataWrite(r, 0.0, somedata);

somedata[2][3] = 82;

// Write some data to depth 2.0
img->dataWrite(r, 2.0, somedata);
}

int main(int argc, char *argv[]) {
	if(argc == 2) write();

	read();
}
