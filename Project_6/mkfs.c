#include <string.h>
#include "block.h"
#include "free.h"
#include "mkfs.h"

void mkfs(void)
{
    unsigned char block[BLOCK_SIZE];

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
}