#include <string.h>
#include <unistd.h>

#include "block.h"
#include "ctest.h"
#include "image.h"

void test_image_open_close(void)
{
    int result = image_open("test.img", 1);

    CTEST_ASSERT(result >= 0, "image_open() should open or create the image file");

    result = image_close();

    CTEST_ASSERT(result == 0, "image_close() should close the image file");
}

void test_block_write_and_read(void)
{
    unsigned char write_block[BLOCK_SIZE];
    unsigned char read_block[BLOCK_SIZE];

    memset(write_block, 0, BLOCK_SIZE);
    memset(read_block, 0, BLOCK_SIZE);

    write_block[0] = 42;
    write_block[100] = 99;
    write_block[BLOCK_SIZE - 1] = 123;

    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for block test");

    bwrite(0, write_block);
    bread(0, read_block);

    CTEST_ASSERT(read_block[0] == 42, "first byte should match after bread()");
    CTEST_ASSERT(read_block[100] == 99, "middle byte should match after bread()");
    CTEST_ASSERT(read_block[BLOCK_SIZE - 1] == 123, "last byte should match after bread()");

    image_close();
}

void test_multiple_blocks(void)
{
    unsigned char block_one[BLOCK_SIZE];
    unsigned char block_two[BLOCK_SIZE];
    unsigned char read_block[BLOCK_SIZE];

    memset(block_one, 0, BLOCK_SIZE);
    memset(block_two, 0, BLOCK_SIZE);
    memset(read_block, 0, BLOCK_SIZE);

    block_one[0] = 11;
    block_two[0] = 22;

    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for multiple block test");

    bwrite(1, block_one);
    bwrite(2, block_two);

    bread(1, read_block);
    CTEST_ASSERT(read_block[0] == 11, "block 1 should contain its own data");

    bread(2, read_block);
    CTEST_ASSERT(read_block[0] == 22, "block 2 should contain its own data");

    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open_close();
    test_block_write_and_read();
    test_multiple_blocks();

    CTEST_RESULTS();
    CTEST_EXIT();
}