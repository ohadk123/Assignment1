#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include "libstr.h"

int b64_distance(char str1[MAX_STR], char str2[MAX_STR]) {
	char ranges[] = {'A', 'Z', 'a', 'z', '0', '9', '+','+', '/', '/', 0, 0, 0, 0, 0, 0};
	__m128i b64_filter = _mm_lddqu_si128((const __m128i *) ranges);
	
}