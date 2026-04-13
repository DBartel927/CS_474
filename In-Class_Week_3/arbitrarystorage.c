// Introduction
// In this challenge you'll be given a buffer allocated from the OS (with mmap()) and number of different data types that you are meant to store in different places in that buffer.

// What You'll Gain
// Better understanding of low-level memory operations in C.
// The ability to write arbitrary data at arbitrary points in memory.
// More practice with casting.
// The Plan
// We're going to take our chunk of 1024 bytes, and treat the addresses like pointers of different types.

// We can do this with a certain specific set of skills that we'll outline next.

// First, here's a picture showing how we're going to partition up our 1024 bytes (byte offsets are shown at the top. Empty space is unused).

//  0   4?           48        64?              512         1023
// ┌┴──┬┴────┬───⋯───┬┴───────┬┴───────┬───⋯───┬┴────┬───⋯──┴┐
// │int│float│       │struct  │struct  │       │short│       │
// │   │     │       │firework│firework│       │     │       │
// └───┴─────┴───⋯───┴────────┴────────┴───⋯───┴─────┴───⋯───┘
// [The above diagram shows an integer stored at offset 0 in our allocated space. Then a float at offset 4 (depending on the sizeof(int) on your system). Then some unused space. Then a struct firework at offset 48, then another struct firework at offset 64 (depending on sizeof(struct firework)), some more unused space, a short at offset 512, then the remaining empty space.]

// Tools
// Here are a variety of tools you have at your disposal to solve this challenge.

// Casting
// You can turn a pointer into any other pointer type if you cast.

// int a[128];  // array of 128 ints
// char *p;

// // This gives an error. The pointer on the left is `char*`
// // and the pointer on the right is `int*`

// p = &(a[12]); // error, pointer mismatch
// p = a + 12;   // shorter, equivalent error

// // Force the right side to match left side type with a cast:

// p = (char *)(a + 12);
// Warning about alignment: We haven't talked about this yet, but you often run into trouble just stamping non-char data at arbitrary offsets in memory. We're going to carefully do this a addresses that are multiples of 16 to prevent these problems.

// You can cast any pointer type to any other pointer type. This is you telling the compiler, "I know what I'm doing, so don't complain." And you'd better actually know what you're doing!

// You do not need to cast to or from void*. A void* can be assigned into any other pointer type without a cast, and vice-versa.

// Size-Of
// You can tell how big a data structure is in bytes with the sizeof operator.

// Use parentheses with a raw type name:
// sizeof(int);
// sizeof(short);
// sizeof(struct firework);
// You can also get the type of an expression; this just gives you the size of the type underlying the expression. Parens are redundant in this case.
// char c;
// int *p;

// sizeof c;   // sizeof(char)
// sizeof p;   // sizeof(int*)
// sizeof *p;  // sizeof(int)

// sizeof 32 + 3490.0;  // sizeof(double)
// The sizeof(char) is always 1 byte!
// Pointer Arithmetic
// For pointers with a type (i.e. non-void*), you can index ahead in memory by a certain number of elements of that type. (Some compiler extensions allow you to do pointer arithmetic on void*s, but it's not portable.)

// int *p = (int[]){0,1,2,3,4};  // point to anon array

// printf("%d\n", *p);     // 0
// printf("%d\n", *(p+0)); // 0
// printf("%d\n", *(p+2)); // 2
// If you want to get to the second byte of an integer, you can get tricky by casting to a char*.

// int x = 3490;
// //char *p = &x;      // ERROR: pointer mismatch
// char *p = (char*)&x; // OK

// printf("%02X\n", *(p+1)); // 0D (hex), system-dependent
// The output there depends on the byte ordering on your system. On this little-endian system, 3490 (0x0DA2) is stored in memory as A2 0D 00 00, so byte 0D is at offset 1.

// Lastly, if you have a big array of memory, you can jump to a particular byte offset first by casting to char*, then adding, then casting back to the type you want.

// Let's get an int* at byte offset 64:

// void *v = malloc(1024);

// char *c = v;
// char *c_offset = c + 64;
// int *i = (int*)c_offset;

// // Or, more compactly:

// int *i = (int*)((char*)v + 32);
// That last line reads, "Cast v to a char*, then add 32 bytes, then cast the result to int*."

// Remember the only reason this adds 32 bytes is because a char is exactly 1 byte.

// If we did this:

// int *i = malloc(1024);
// int *j = i + 32;
// j would point 32 ints beyond i. If each int was 4 bytes, then j would point 32×4 (128) bytes beyond i!

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const int SIZE=1024;  // bytes

struct firework {
    int shell_caliber;
    char color;
    float burst_charge;
    int fuse_delay;
};

int main(void)
{
    // Allocate some space from the OS
    void *heap = mmap(NULL, SIZE, PROT_READ|PROT_WRITE,
                      MAP_ANON|MAP_PRIVATE, -1, 0);

    // Challenge: Store your favorite letter at the byte `heap` points
    // to, then print it with `%c` and also with `%d`.
    // TODO
    char *letter_ptr = (char *)heap;
    *letter_ptr = 'P';
    printf("Letter: %c\n", *letter_ptr);
    printf("Letter as int: %d\n", *letter_ptr);

    // Challenge: Store a string at the byte `heap` points to, then
    // print it.
    // TODO
    char *string_ptr = (char *)heap;
    strcpy(string_ptr, "Hello, world!");
    printf("String: %s\n", string_ptr);

    // Challenge: Store an integer at the byte `heap` points to, then
    // print it.
    // TODO
    int *int_ptr = (int *)heap;
    *int_ptr = 3490;
    printf("Integer: %d\n", *int_ptr);

    // Challenge: Store a float right *after* the int you just stored.
    // Print them both.
    // TODO
    float *float_ptr = (float *)(int_ptr + 1);
    *float_ptr = 2.457298f;
    printf("Integer: %d\n", *int_ptr);
    printf("Float: %f\n", *float_ptr);

    // Challenge: Store a short (a smallish integer) at byte offset 512
    // in the `heap` area. Then print it with `%d`, along with the int
    // and float you already stored.
    // TODO
    short *short_ptr = (short *)((char *)heap + 512);
    *short_ptr = 12345;
    printf("Integer: %d\n", *int_ptr);
    printf("Float: %f\n", *float_ptr);
    printf("Short: %d\n", *short_ptr);

    // Challenge: Store a struct firework at byte offset 48 in the
    // `heap` area. Make a `struct firework *` initialized to the right
    // position, then fill it with data (with the `->` operator). Then
    // print out the data, and also print out the int, float, and short
    // from above.
    // TODO
    struct firework *fw = (struct firework *)((char *)heap + 48);
    fw->shell_caliber = 5;
    fw->color = 'g';
    fw->burst_charge = 2.0f;
    fw->fuse_delay = 1000;
    printf("Firework: caliber=%d, color=%c, burst_charge=%f, fuse_delay=%d\n",
           fw->shell_caliber, fw->color, fw->burst_charge, fw->fuse_delay);
    printf("Integer: %d\n", *int_ptr);
    printf("Float: %f\n", *float_ptr);
    printf("Short: %d\n", *short_ptr);

    // Challenge: Store a second struct firework directly after the
    // first one. Initialize and print.
    // TODO
    struct firework *fw2 = fw + 1;
    fw2->shell_caliber = 10;
    fw2->color = 'r';
    fw2->burst_charge = 4.5f;
    fw2->fuse_delay = 2000;
    printf("Firework 2: caliber=%d, color=%c, burst_charge=%f, fuse_delay=%d\n",
           fw2->shell_caliber, fw2->color, fw2->burst_charge, fw2->fuse_delay);
    printf("Integer: %d\n", *int_ptr);
    printf("Float: %f\n", *float_ptr);
    printf("Short: %d\n", *short_ptr);
}