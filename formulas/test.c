#include <stdio.h>

extern float formula2(float *x, float *y, unsigned int length);

int main() {
	float x[] = {1.0f, 2.0f, 3.0f, 4.0f};
	float y[] = {0.1, 0.2, 0.3, 0.4};

	float res = formula2(x, y, 4);
	printf("%f\n", res);
}