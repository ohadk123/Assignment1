#include <immintrin.h>
#include <stdio.h>
#include <string.h>
#include "libstr.h"

// extern int str_len(char *str);

int clear_string(char *str, char *dst, int len) {
	char ranges[] = {'A', 'Z', 'a', 'z', '0', '9', '+','+', '/', '/', 0, 0, 0, 0, 0, 0};
	__m128i b64_filter = _mm_lddqu_si128((const __m128i *) ranges);
	__m128i xstr = _mm_lddqu_si128((const __m128i *) str);
	__m128i mask = _mm_cmpistrm(b64_filter, xstr, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);
	_mm_maskmoveu_si128(xstr, mask, dst);

	for (int w = 0, r = 0; w < len; w++) {
		while (dst[w++]);
		r = w;
		while (!(dst[r]));
		dst[w] = dst[r];
	}
	puts(dst);
}

int main() {
	char str[] = "123456789?bcdef";
	int len = strlen(str);
	char dst[len];
	memset((void *) dst, 0, len);
	clear_string(str, dst, len);

	puts(dst);

	return 0;
}