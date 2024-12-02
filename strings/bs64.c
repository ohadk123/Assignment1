//bs64.c

#include <stdio.h>
#include <string.h>
#include <immintrin.h>

char* clean(char* str) {
    // Define valid character ranges
    __m128i ranges = _mm_setr_epi8(
        '0', '9',              // digits
        'A', 'Z',             // uppercase
        'a', 'z',            // lowercase
        '+', '+',           // plus sign
        '/', '/',          // forward slash
        0, 0, 0, 0, 0, 0  // padding
    );

    char* write = str;  // Position to write next valid character
    const char* read = str;  // Position to read from

    while (*read) {
        __m128i chars = _mm_loadu_si128((__m128i*)read);
        int res = _mm_cmpistri(ranges, chars, 0x2C);
        
        // Process only valid characters
        for (int i = 0; i < res && read[i]; i++) {
            char c = read[i];
            // Only copy if it's a valid character
            if ((c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                c == '+' || c == '/') {
                *write = c;
                write++;
            }
        }

        read += (res < 16) ? res + 1 : 16;
    }

    *write = '\0';  // Terminate the string
    printf("After cleaning: '%s'\n", str);
    return str;
}

int get_b64_value(char c) {
    // Load the character into all elements of a 128-bit vector
    __m128i char_vec = _mm_set1_epi8(c);
    
    // Create comparison vectors for the ranges
    __m128i upper_range = _mm_and_si128(
        _mm_cmpgt_epi8(char_vec, _mm_set1_epi8('A' - 1)),
        _mm_cmpgt_epi8(_mm_set1_epi8('Z' + 1), char_vec)
    );
    
    __m128i lower_range = _mm_and_si128(
        _mm_cmpgt_epi8(char_vec, _mm_set1_epi8('a' - 1)),
        _mm_cmpgt_epi8(_mm_set1_epi8('z' + 1), char_vec)
    );
    
    __m128i digit_range = _mm_and_si128(
        _mm_cmpgt_epi8(char_vec, _mm_set1_epi8('0' - 1)),
        _mm_cmpgt_epi8(_mm_set1_epi8('9' + 1), char_vec)
    );
    
    __m128i plus_match = _mm_cmpeq_epi8(char_vec, _mm_set1_epi8('+'));
    __m128i slash_match = _mm_cmpeq_epi8(char_vec, _mm_set1_epi8('/'));
    
    // Calculate the base values
    __m128i upper_val = _mm_and_si128(upper_range, _mm_sub_epi8(char_vec, _mm_set1_epi8('A')));
    __m128i lower_val = _mm_and_si128(lower_range, _mm_add_epi8(_mm_sub_epi8(char_vec, _mm_set1_epi8('a')), _mm_set1_epi8(26)));
    __m128i digit_val = _mm_and_si128(digit_range, _mm_add_epi8(_mm_sub_epi8(char_vec, _mm_set1_epi8('0')), _mm_set1_epi8(52)));
    __m128i plus_val = _mm_and_si128(plus_match, _mm_set1_epi8(62));
    __m128i slash_val = _mm_and_si128(slash_match, _mm_set1_epi8(63));
    
    // Combine all values
    __m128i result = _mm_or_si128(
        _mm_or_si128(
            _mm_or_si128(upper_val, lower_val),
            _mm_or_si128(digit_val, plus_val)
        ),
        slash_val
    );
    
    // Extract the first byte which contains our result
    return _mm_extract_epi8(result, 0);
}

int distance(char* str1, char* str2) {
    char* clean1 = clean(str1);
    char* clean2 = clean(str2);
    
    // Process 4 characters at a time using SSE2 instead of 8
    __m128i powers = _mm_setr_epi32(
        1,
        64,
        64*64,
        64*64*64
    );
    
    long long acc1 = 0;
    long long acc2 = 0;

    // Process first string
    char* ptr = clean1;
    while (*ptr) {
        // Load up to 4 characters
        __m128i vals = _mm_setzero_si128();
        int count = 0;
        
        // Convert next 4 chars to base64 values and load them into an array first
        int values[4] = {0};
        for(int i = 0; i < 4 && ptr[i]; i++) {
            values[i] = get_b64_value(ptr[i]);
            count++;
            printf("char '%c' -> value %d\n", ptr[i], values[i]);
        }

        // Load the array into SIMD vector
        vals = _mm_loadu_si128((__m128i*)values);

        // Multiply each value by its corresponding power of 64
        __m128i products = _mm_mullo_epi32(vals, powers);
        
        // Horizontal sum of products
        long long chunk_sum = 0;
        int* sums = (int*)&products;
        for(int i = 0; i < count; i++) {
            chunk_sum += sums[i];
        }
        
        // Update accumulator
        acc1 = acc1 * (1LL << (count * 6)) + chunk_sum;
        
        // Advance pointer
        ptr += count;
    }
    printf("First accumulator: %lld\n", acc1);

    // Process second string
    ptr = clean2;
    while (*ptr) {
        // Load up to 4 characters
        __m128i vals = _mm_setzero_si128();
        int count = 0;
        
        // Convert next 4 chars to base64 values and load them into an array first
        int values[4] = {0};
        for(int i = 0; i < 4 && ptr[i]; i++) {
            values[i] = get_b64_value(ptr[i]);
            count++;
            printf("char '%c' -> value %d\n", ptr[i], values[i]);
        }

        // Load the array into SIMD vector
        vals = _mm_loadu_si128((__m128i*)values);

        // Multiply each value by its corresponding power of 64
        __m128i products = _mm_mullo_epi32(vals, powers);
        
        // Horizontal sum of products
        long long chunk_sum = 0;
        int* sums = (int*)&products;
        for(int i = 0; i < count; i++) {
            chunk_sum += sums[i];
        }
        
        // Update accumulator
        acc2 = acc2 * (1LL << (count * 6)) + chunk_sum;
        
        // Advance pointer
        ptr += count;
    }
    printf("Second accumulator: %lld\n", acc2);

    return (int)(acc2 - acc1);
}

int main() {
    char str1[256];
    char str2[256];  
    
    printf("Enter first number:\n");
    scanf("%255s", str1);
    printf("Enter second number:\n");
    scanf("%255s", str2);
        
    printf("Original strings: '%s' and '%s'\n", str1, str2);
    printf("The distance is: %d\n", distance(str1, str2));
    
    return 0;
}