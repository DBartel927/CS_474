#include "block.h"
#include "free.h"
#include "inode.h"

int ialloc(void)
{
    unsigned char block[BLOCK_SIZE];

    bread(INODE_MAP_BLOCK, block);

    int free_inode = find_free(block);

    if (free_inode == -1) {
        return -1;
    }

    set_free(block, free_inode, 1);
    bwrite(INODE_MAP_BLOCK, block);

    return free_inode;
}