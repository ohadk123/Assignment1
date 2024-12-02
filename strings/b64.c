#include <immintrin.h>
#include <stdio.h>
#include "libstr.h"

int clear_string() {
	char ranges[] = {'A', 'Z', 'a', 'z', '0', '9', '+','+', '/', '/', 0, 0, 0, 0, 0, 0};
	__m128i b64_filter = _mm_lddqu_si128((const __m128i *) ranges);
	char str[] = "123456789?bcdef";
	char dst[16] = {0};
	__m128i xstr = _mm_lddqu_si128((const __m128i *) str);
	__m128i mask = _mm_cmpistrm(b64_filter, xstr, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK | _SIDD_MASKED_NEGATIVE_POLARITY);
	_mm_maskmoveu_si128(xstr, mask, dst);
	puts(dst);
}

int main() {
	clear_string();
	return 0;
}