#include <stddef.h>

#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"

#define INODE_SIZE_OFFSET 0
#define INODE_OWNER_OFFSET 4
#define INODE_PERMISSIONS_OFFSET 6
#define INODE_FLAGS_OFFSET 7
#define INODE_LINK_COUNT_OFFSET 8
#define INODE_BLOCK_PTR_OFFSET 9
#define INODE_BLOCK_PTR_SIZE 2

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

static int inode_block_num(int inode_num)
{
    return inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
}

static int inode_block_offset(int inode_num)
{
    int block_offset = inode_num % INODES_PER_BLOCK;

    return block_offset * INODE_SIZE;
}

struct inode *incore_find_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count == 0) {
            return incore + i;
        }
    }

    return NULL;
}

struct inode *incore_find(unsigned int inode_num)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num) {
            return incore + i;
        }
    }

    return NULL;
}

void incore_free_all(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        incore[i].ref_count = 0;
    }
}

void read_inode(struct inode *in, int inode_num)
{
    unsigned char block[BLOCK_SIZE];

    int block_num = inode_block_num(inode_num);
    int offset = inode_block_offset(inode_num);

    bread(block_num, block);

    in->size = read_u32(block + offset + INODE_SIZE_OFFSET);
    in->owner = read_u16(block + offset + INODE_OWNER_OFFSET);
    in->permissions = read_u8(block + offset + INODE_PERMISSIONS_OFFSET);
    in->flags = read_u8(block + offset + INODE_FLAGS_OFFSET);
    in->link_count = read_u8(block + offset + INODE_LINK_COUNT_OFFSET);

    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        int ptr_offset = INODE_BLOCK_PTR_OFFSET + i * INODE_BLOCK_PTR_SIZE;

        in->block_ptr[i] = read_u16(block + offset + ptr_offset);
    }

    in->inode_num = inode_num;
}

void write_inode(struct inode *in)
{
    unsigned char block[BLOCK_SIZE];

    int block_num = inode_block_num(in->inode_num);
    int offset = inode_block_offset(in->inode_num);

    bread(block_num, block);

    write_u32(block + offset + INODE_SIZE_OFFSET, in->size);
    write_u16(block + offset + INODE_OWNER_OFFSET, in->owner);
    write_u8(block + offset + INODE_PERMISSIONS_OFFSET, in->permissions);
    write_u8(block + offset + INODE_FLAGS_OFFSET, in->flags);
    write_u8(block + offset + INODE_LINK_COUNT_OFFSET, in->link_count);

    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        int ptr_offset = INODE_BLOCK_PTR_OFFSET + i * INODE_BLOCK_PTR_SIZE;

        write_u16(block + offset + ptr_offset, in->block_ptr[i]);
    }

    bwrite(block_num, block);
}

struct inode *iget(int inode_num)
{
    struct inode *in = incore_find(inode_num);

    if (in != NULL) {
        in->ref_count++;
        return in;
    }

    in = incore_find_free();

    if (in == NULL) {
        return NULL;
    }

    read_inode(in, inode_num);

    in->ref_count = 1;
    in->inode_num = inode_num;

    return in;
}

void iput(struct inode *in)
{
    if (in->ref_count == 0) {
        return;
    }

    in->ref_count--;

    if (in->ref_count == 0) {
        write_inode(in);
    }
}

struct inode *ialloc(void)
{
    unsigned char block[BLOCK_SIZE];

    bread(INODE_MAP_BLOCK, block);

    int inode_num = find_free(block);

    if (inode_num == -1) {
        return NULL;
    }

    struct inode *in = iget(inode_num);

    if (in == NULL) {
        return NULL;
    }

    set_free(block, inode_num, 1);
    bwrite(INODE_MAP_BLOCK, block);

    in->size = 0;
    in->owner = 0;
    in->permissions = 0;
    in->flags = 0;
    in->link_count = 0;
    in->inode_num = inode_num;

    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }

    write_inode(in);

    return in;
}