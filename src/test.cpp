#include <dif.h>

int main(int argc, char *argv[]) {
	DifDeepPixel *px = DifDeepPixel::create(2,2, 4,3);

	DifPixel p1(4.8, 1.3, 45.1, 1.0, 2.0);
	DifPixel p2(4.8, 1.3, 45.1, 1.0, 91.1);
	DifPixel p3(4.8, 1.3, 45.1, 1.0, 82.1);
	DifPixel p4(4.8, 1.3, 45.1, 1.0, 1.2);

	printf("depth=%f samples=%d\n", px->depth(), px->samples());

	px->pixel(p1);

	printf("depth=%f samples=%d\n", px->depth(), px->samples());

	px->pixel(p2);

	printf("depth=%f samples=%d\n", px->depth(), px->samples());

	px->pixel(p3);

	printf("depth=%f samples=%d\n", px->depth(), px->samples());

	px->pixel(p4);

	printf("depth=%f samples=%d\n", px->depth(), px->samples());

	return 0;
}
