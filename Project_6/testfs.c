#include <string.h>
#include <unistd.h>

#include "block.h"
#include "ctest.h"
#include "image.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"
#include "dir.h"
#include "ls.h"

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

// void test_ialloc_allocates_first_free_inode(void)
// {
//     CTEST_ASSERT(image_open("test.img", 1) >= 0, "image file should open for ialloc test");

//     mkfs();

//     int inode_num = ialloc();
//     CTEST_ASSERT(inode_num == 0, "ialloc() should allocate the first free inode (0)");

//     inode_num = ialloc();
//     CTEST_ASSERT(inode_num == 1, "ialloc() should allocate the next free inode (1)");

//     image_close();
// }

void test_ialloc_returns_initialized_inode(void)
{
    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image should open for ialloc test");

    mkfs();
    incore_free_all();

    struct inode *in = ialloc();

    CTEST_ASSERT(in != NULL, "ialloc should return an in-core inode");
    CTEST_ASSERT(in->inode_num == 0, "first ialloc should allocate inode 0");
    CTEST_ASSERT(in->ref_count == 1, "allocated inode should have ref_count 1");
    CTEST_ASSERT(in->size == 0, "allocated inode size should be initialized to 0");
    CTEST_ASSERT(in->owner == 0, "allocated inode owner should be initialized to 0");
    CTEST_ASSERT(in->permissions == 0, "allocated inode permissions should be initialized to 0");
    CTEST_ASSERT(in->flags == 0, "allocated inode flags should be initialized to 0");

    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        CTEST_ASSERT(in->block_ptr[i] == 0, "allocated inode block pointers should be 0");
    }

    iput(in);

    image_close();
}

void test_alloc_allocates_first_free_block(void)
{
    int block_num;

    CTEST_ASSERT(image_open("test.img", 1) >= 0,
        "image file should open for alloc test");

    mkfs();

    block_num = alloc();
    CTEST_ASSERT(block_num == 8,
        "alloc() should allocate the first free block after root directory");

    block_num = alloc();
    CTEST_ASSERT(block_num == 9,
        "alloc() should allocate the next free block");

    image_close();
}

void test_mkfs_initializes_image(void)
{
    unsigned char block[BLOCK_SIZE];

    CTEST_ASSERT(image_open("test.img", 1) >= 0,
        "image file should open for mkfs test");

    mkfs();

    bread(1, block);
    CTEST_ASSERT(block[0] == 0x01,
        "inode map should have inode 0 allocated for root directory");

    bread(2, block);
    CTEST_ASSERT(block[0] == 0xff,
        "block map should mark blocks 0 through 7 used");

    CTEST_ASSERT(block[1] == 0x00,
        "block map second byte should still be zero");

    image_close();
}

void test_pack(void)
{
    unsigned char buf[8];

    write_u32(buf, 0x12345678);
    CTEST_ASSERT(read_u32(buf) == 0x12345678, "read_u32() should return the value written by write_u32()");

    write_u16(buf, 0xabcd);
    CTEST_ASSERT(read_u16(buf) == 0xabcd, "read_u16() should return the value written by write_u16()");

    write_u8(buf, 0xef);
    CTEST_ASSERT(read_u8(buf) == 0xef, "read_u8() should return the value written by write_u8()");
}

void test_incore_find_free_and_find(void) {
    incore_free_all();
    struct inode *in = incore_find_free();
    CTEST_ASSERT(in != NULL, "incore_find_free() should return a free inode");
    in->ref_count = 1;
    in->inode_num = 12;
    in->size = 555;
    struct inode *found = incore_find(12);
    CTEST_ASSERT(found == in, "incore_find() should find the inode by its number");
    CTEST_ASSERT(found->size == 555, "incore_find() should return the correct inode with the correct size");
}

void test_write_and_read_inode(void)
{
    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image should open for inode read/write test");

    mkfs();

    struct inode in;
    struct inode out;

    in.size = 12345;
    in.owner = 77;
    in.permissions = 6;
    in.flags = 2;
    in.link_count = 3;
    in.inode_num = 5;
    in.ref_count = 99;

    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in.block_ptr[i] = i + 100;
    }

    write_inode(&in);
    read_inode(&out, 5);

    CTEST_ASSERT(out.size == 12345, "inode size should survive write/read");
    CTEST_ASSERT(out.owner == 77, "inode owner should survive write/read");
    CTEST_ASSERT(out.permissions == 6, "inode permissions should survive write/read");
    CTEST_ASSERT(out.flags == 2, "inode flags should survive write/read");
    CTEST_ASSERT(out.link_count == 3, "inode link count should survive write/read");
    CTEST_ASSERT(out.block_ptr[0] == 100, "block pointer 0 should survive write/read");
    CTEST_ASSERT(out.block_ptr[15] == 115, "block pointer 15 should survive write/read");
    CTEST_ASSERT(out.inode_num == 5, "read_inode should set inode_num");

    image_close();
}

void test_iget_and_iput(void)
{
    CTEST_ASSERT(image_open("test.img", 1) >= 0, "image should open for iget/iput test");

    mkfs();
    incore_free_all();

    struct inode original;

    original.size = 777;
    original.owner = 42;
    original.permissions = 7;
    original.flags = 1;
    original.link_count = 2;
    original.inode_num = 9;
    original.ref_count = 0;

    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        original.block_ptr[i] = i + 20;
    }

    write_inode(&original);

    struct inode *in1 = iget(9);
    CTEST_ASSERT(in1 != NULL, "iget should load inode into core");
    CTEST_ASSERT(in1->size == 777, "iget should load inode data");
    CTEST_ASSERT(in1->ref_count == 1, "iget should set ref_count to 1");

    struct inode *in2 = iget(9);
    CTEST_ASSERT(in2 == in1, "iget should return existing in-core inode");
    CTEST_ASSERT(in1->ref_count == 2, "second iget should increment ref_count");

    iput(in1);
    CTEST_ASSERT(in1->ref_count == 1, "first iput should decrement ref_count");

    in1->size = 888;
    iput(in1);
    CTEST_ASSERT(in1->ref_count == 0, "second iput should drop ref_count to 0");

    struct inode check;
    read_inode(&check, 9);
    CTEST_ASSERT(check.size == 888, "iput should write inode when ref_count reaches 0");

    image_close();
}

void test_mkfs_creates_root_directory(void)
{
    struct inode in;

    CTEST_ASSERT(image_open("test.img", 1) >= 0,
        "image should open for root directory test");

    mkfs();

    read_inode(&in, 0);

    CTEST_ASSERT(in.flags == DIRECTORY_FLAG,
        "root inode should be a directory");

    CTEST_ASSERT(in.size == 64,
        "root directory should have two directory entries");

    CTEST_ASSERT(in.block_ptr[0] != 0,
        "root directory should have a data block");

    image_close();
}

void test_directory_open(void)
{
    struct directory *dir;

    CTEST_ASSERT(image_open("test.img", 1) >= 0,
        "image should open for directory open test");

    mkfs();

    dir = directory_open(0);

    CTEST_ASSERT(dir != NULL,
        "directory_open should return a directory pointer");

    CTEST_ASSERT(dir->offset == 0,
        "directory offset should start at 0");

    directory_close(dir);

    image_close();
}

void test_directory_get(void)
{
    struct directory *dir;
    struct directory_entry ent;

    CTEST_ASSERT(image_open("test.img", 1) >= 0,
        "image should open for directory get test");

    mkfs();

    dir = directory_open(0);

    CTEST_ASSERT(directory_get(dir, &ent) == 0,
        "directory_get should read the first entry");

    CTEST_ASSERT(ent.inode_num == 0,
        "first entry should point to inode 0");

    CTEST_ASSERT(strcmp(ent.name, ".") == 0,
        "first entry should be .");

    CTEST_ASSERT(directory_get(dir, &ent) == 0,
        "directory_get should read the second entry");

    CTEST_ASSERT(ent.inode_num == 0,
        "second entry should point to inode 0");

    CTEST_ASSERT(strcmp(ent.name, "..") == 0,
        "second entry should be ..");

    CTEST_ASSERT(directory_get(dir, &ent) == -1,
        "directory_get should fail at the end of the directory");

    directory_close(dir);

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
    test_alloc_allocates_first_free_block();
    test_mkfs_initializes_image();
    test_pack();
    test_incore_find_free_and_find();
    test_write_and_read_inode();
    test_iget_and_iput();
    test_mkfs_creates_root_directory();
    test_directory_open();
    test_directory_get();

    CTEST_RESULTS();
    CTEST_EXIT();
}