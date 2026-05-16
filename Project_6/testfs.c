#include <string.h>
#include <unistd.h>

#include "block.h"
#include "ctest.h"
#include "image.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

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

// void test_multiple_blocks(void)
// {
//     unsigned char block_one[BLOCK_SIZE];
//     unsigned char block_two[BLOCK_SIZE];
//     unsigned char read_block[BLOCK_SIZE];

//     memset(block_one, 0, BLOCK_SIZE);
//     memset(block_two, 0, BLOCK_SIZE);
//     memset(read_block, 0, BLOCK_SIZE);

//     block_one[0] = 11;
//     block_two[0] = 22;

//     CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for multiple block test");

//     bwrite(1, block_one);
//     bwrite(2, block_two);

//     bread(1, read_block);
//     CTEST_ASSERT(read_block[0] == 11, "block 1 should contain its own data");

//     bread(2, read_block);
//     CTEST_ASSERT(read_block[0] == 22, "block 2 should contain its own data");

//     image_close();
// }

void test_set_free_sets_and_clears_bits(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    set_free(block, 0, 1);
    CTEST_ASSERT(block[0] == 0x01, "setting bit 0 should set the first bit of the first byte");

    set_free(block, 7, 1);
    CTEST_ASSERT(block[0] == 0x81, "setting bit 7 should set the last bit of the first byte");

    set_free(block, 8, 1);
    CTEST_ASSERT(block[1] == 0x01, "setting bit 8 should set the first bit of the second byte");

    set_free(block, 0, 0);
    CTEST_ASSERT(block[0] == 0x80, "clearing bit 0 should clear the first bit of the first byte");

    set_free(block, 7, 0);
    CTEST_ASSERT(block[0] == 0x00, "clearing bit 7 should clear the last bit of the first byte");

    set_free(block, 8, 0);
    CTEST_ASSERT(block[1] == 0x00, "clearing bit 8 should clear the first bit of the second byte");
}

void test_find_free_finds_first_clear_bit(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    CTEST_ASSERT(find_free(block) == 0, "find_free() should return 0 when all bits are clear");

    block[0] = 0xff;
    CTEST_ASSERT(find_free(block) == 8, "find_free() should return 8 when the first byte is full");

    block[1] = 0x7f;
    CTEST_ASSERT(find_free(block) == 15, "find_free() should return 15 when bits 0-7 are set and bit 8 is clear");
}

void test_ialloc_allocates_first_free_inode(void)
{
    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for ialloc test");

    mkfs();

    int inode_num = ialloc();
    CTEST_ASSERT(inode_num == 0, "ialloc() should allocate the first free inode (0)");

    inode_num = ialloc();
    CTEST_ASSERT(inode_num == 1, "ialloc() should allocate the next free inode (1)");

    image_close();
}

void test_alloc_allocates_first_free_block(void)
{
    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for alloc test");

    mkfs();

    int block_num = alloc();
    CTEST_ASSERT(block_num == 7, "alloc() should allocate the first free block (7)");

    block_num = alloc();
    CTEST_ASSERT(block_num == 8, "alloc() should allocate the next free block (8)");

    image_close();
}

void test_mkfs_initializes_image(void)
{
    unsigned char block[BLOCK_SIZE];
    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for mkfs test");

    mkfs();

    bread(1, block);
    CTEST_ASSERT(block[0] == 0x00, "block 1 should be initialized to all zeros");

    bread(2, block);
    CTEST_ASSERT(block[0] == 0x7f, "block 2 should have the first 7 bits set and the last bit clear");
    CTEST_ASSERT(block[1] == 0x00, "block 2 should have the second byte set to all zeros");

    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open_close();
    test_block_write_and_read();
    // test_multiple_blocks();
    test_set_free_sets_and_clears_bits();
    test_find_free_finds_first_clear_bit();
    test_ialloc_allocates_first_free_inode();
    test_alloc_allocates_first_free_block();
    test_mkfs_initializes_image();

    CTEST_RESULTS();
    CTEST_EXIT();
}