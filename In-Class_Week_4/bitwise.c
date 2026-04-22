#include <stdio.h>

/*
 * Expected output:
 *
 * 1111000000000000
 * 0000111111111111
 * 1010110011001100
 * 0101001100110011
 *
 * 1111
 * 0000
 * 1010
 * 0101
 *
 * 000000000000
 * 111111111111
 * 110011001100
 * 001100110011
 */

// What to Implement
// All example numbers in the following descriptions are binary.

// merge_nums(): This takes a 4-bit number and prepends it to a 12-bit number.

// For example:

// 1010 prepended to 111111111111 gives 1010111111111111.

// get_high_4(): Returns the high 4 bits of a number.

// For example,

// 1010111111111111 should return 1010.

// get_low_12(): Returns the low 12 bits of a number.

// For example,

// 1010111111111111 should return 111111111111.

// Expected Output
// 1111000000000000
// 0000111111111111
// 1010110011001100
// 0101001100110011

// 1111
// 0000
// 1010
// 0101

// 000000000000
// 111111111111
// 110011001100
// 001100110011

void print_bin(int width, unsigned v)
{
    for (int i = width - 1; i >= 0; i--)
        putchar(((v >> i) & 1) + '0');
    putchar('\n');
}

void merge_nums(unsigned a, unsigned b)
{
    unsigned int merged;

    // append b to a
    merged = (a << 12) | b;

    print_bin(16, merged);
}

void get_high_4(unsigned a)
{
    unsigned int high_4;

    // leftmost four bits of a number
    high_4 = a >> 12;

    print_bin(4, high_4);
}

void get_low_12(unsigned a)
{
    unsigned int low_12;

    // rightmost twelve bits of a number
    low_12 = a & 0xFFF;

    print_bin(12, low_12);
}

int main(void)
{
    merge_nums(0b1111, 0b000000000000);
    merge_nums(0b0000, 0b111111111111);
    merge_nums(0b1010, 0b110011001100);
    merge_nums(0b0101, 0b001100110011);

    putchar('\n');

    get_high_4(0b1111000000000000);
    get_high_4(0b0000111111111111);
    get_high_4(0b1010110011001100);
    get_high_4(0b0101001100110011);

    putchar('\n');

    get_low_12(0b1111000000000000);
    get_low_12(0b0000111111111111);
    get_low_12(0b1010110011001100);
    get_low_12(0b0101001100110011);
}


