#include <stdlib.h>
#include <string.h>

#include "block.h"
#include "dir.h"
#include "inode.h"
#include "pack.h"

#define DIRENT_INODE_OFFSET 0
#define DIRENT_NAME_OFFSET 2

struct directory *directory_open(int inode_num)
{
    struct directory *dir;
    struct inode *in;

    in = iget(inode_num);

    if (in == NULL) {
        return NULL;
    }

    dir = malloc(sizeof *dir);

    if (dir == NULL) {
        iput(in);
        return NULL;
    }

    dir->inode = in;
    dir->offset = 0;

    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    unsigned char block[BLOCK_SIZE];
    unsigned int block_index;
    unsigned int block_num;
    unsigned int offset_in_block;

    if (dir->offset >= dir->inode->size) {
        return -1;
    }

    block_index = dir->offset / BLOCK_SIZE;
    block_num = dir->inode->block_ptr[block_index];

    bread(block_num, block);

    offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block + DIRENT_INODE_OFFSET);
    strcpy(ent->name, (char *)(block + offset_in_block + DIRENT_NAME_OFFSET));

    dir->offset += DIRECTORY_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *dir)
{
    iput(dir->inode);
    free(dir);
}