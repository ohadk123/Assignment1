#include <immintrin.h>
#include <math.h>

float formula1(float *x, unsigned int length) {
	__m128 sqrt_x_sum = _mm_setzero_ps();
	__m128 x2_prod = _mm_set1_ps(1.0f);

	for (int i = 0; i < length; i += 4) {
		__m128 x_vec = _mm_load_ps(x + i);

		sqrt_x_sum = _mm_add_ps(sqrt_x_sum, _mm_sqrt_ps(x_vec));		// sum(sqrt(x))
		x2_prod = _mm_mul_ps(x2_prod, _mm_add_ps(_mm_mul_ps(x_vec, x_vec), _mm_set1_ps(1.0f)));		// prod(x^2+1)
	}

	sqrt_x_sum = _mm_hadd_ps(sqrt_x_sum, sqrt_x_sum);		// x3+x2, x1+x0, x3+x2, x1+x0
	sqrt_x_sum = _mm_hadd_ps(sqrt_x_sum, sqrt_x_sum);		// x3+x2+x1+x0, x3+x2+x1+x0, x3+x2+x1+x0, x3+x2+x1+x0

	__m128 shuf_x2;
	shuf_x2 = _mm_shuffle_ps(x2_prod, x2_prod, 0b01001110);			// x1, x0, x3, x2
	x2_prod = _mm_mul_ps(x2_prod, shuf_x2);							// x1*x3, x0*x2, x3*x1, x2*x0
	shuf_x2 = _mm_shuffle_ps(x2_prod, x2_prod, 0b10110001);			// x0*x2, x1*x3, x2*x0, x3*x1
	x2_prod = _mm_mul_ps(x2_prod, shuf_x2);							// x0*x2*x1*x3, x1*x3*x0*x2, x2*x0*x3*x1, x3*x1*x2*x0

	float sqrt_x_sum_f = _mm_cvtss_f32(sqrt_x_sum);
	float x2_prod_f = _mm_cvtss_f32(x2_prod);

	float total = sqrtf(1.0f + (cbrtf(sqrt_x_sum_f) / x2_prod_f));
	return total;
}
