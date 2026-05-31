#include <string.h>

#include "block.h"
#include "dir.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"

#define ROOT_DIR_ENTRY_COUNT 2
#define ROOT_DIR_SIZE (ROOT_DIR_ENTRY_COUNT * DIRECTORY_ENTRY_SIZE)
#define DOT_ENTRY_OFFSET 0
#define DOTDOT_ENTRY_OFFSET DIRECTORY_ENTRY_SIZE
#define DIRENT_NAME_OFFSET 2

void mkfs(void)
{
    unsigned char block[BLOCK_SIZE];
    struct inode *root_inode;
    int root_block;

    memset(block, 0, BLOCK_SIZE);

    bwrite(0, block);
    bwrite(1, block);
    bwrite(3, block);
    bwrite(4, block);
    bwrite(5, block);
    bwrite(6, block);

    memset(block, 0, BLOCK_SIZE);
    block[0] = 0x7f;
    bwrite(2, block);

    root_inode = ialloc();
    root_block = alloc();

    root_inode->flags = DIRECTORY_FLAG;
    root_inode->size = ROOT_DIR_SIZE;
    root_inode->block_ptr[0] = root_block;

    memset(block, 0, BLOCK_SIZE);

    write_u16(block + DOT_ENTRY_OFFSET, root_inode->inode_num);
    strcpy((char *)(block + DOT_ENTRY_OFFSET + DIRENT_NAME_OFFSET), ".");

    write_u16(block + DOTDOT_ENTRY_OFFSET, root_inode->inode_num);
    strcpy((char *)(block + DOTDOT_ENTRY_OFFSET + DIRENT_NAME_OFFSET), "..");

    bwrite(root_block, block);

    iput(root_inode);
}