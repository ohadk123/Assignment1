#include <immintrin.h>

float formula2(float *x, float *y, unsigned int length) {
	__m128 prod = _mm_set1_ps(1.0f);
	for (int i = 0; i < length; i += 4) {
		__m128 x_vec = _mm_load_ps(x + i);
		__m128 y_vec = _mm_load_ps(y + i);

		__m128 x_y = _mm_sub_ps(x_vec, y_vec);
		prod = _mm_mul_ps(prod, _mm_add_ps(_mm_mul_ps(x_y, x_y), _mm_set1_ps(1.0f)));	// prod((x - y)^2 + 1)
	}

	__m128 shuf_prod;
	shuf_prod = _mm_shuffle_ps(prod, prod, 0b01001110);			// x1, x0, x3, x2
	prod = _mm_mul_ps(prod, shuf_prod);							// x1*x3, x0*x2, x3*x1, x2*x0
	shuf_prod = _mm_shuffle_ps(prod, prod, 0b10110001);			// x0*x2, x1*x3, x2*x0, x3*x1
	prod = _mm_mul_ps(prod, shuf_prod);							// x0*x2*x1*x3, x1*x3*x0*x2, x2*x0*x3*x1, x3*x1*x2*x0

	__m128 sum = _mm_setzero_ps();
	for (int i = 0; i < length; i += 4) {
		__m128 x_vec = _mm_load_ps(x + i);
		__m128 y_vec = _mm_load_ps(y + i);

		__m128 xy = _mm_mul_ps(x_vec, y_vec);
		sum = _mm_add_ps(sum, _mm_div_ps(xy, prod));
	}

	sum = _mm_hadd_ps(sum, sum);		// x3+x2, x1+x0, x3+x2, x1+x0
	sum = _mm_hadd_ps(sum, sum);		// x3+x2+x1+x0, x3+x2+x1+x0, x3+x2+x1+x0, x3+x2+x1+x0

	float sum_f = _mm_cvtss_f32(sum);
	return sum_f;
}
