#include <unistd.h>
#include "block.h"
#include "image.h"
#include "free.h"

unsigned char *bread(int block_num, unsigned char *block)
{
    off_t offset = block_num * BLOCK_SIZE;

    lseek(image_fd, offset, SEEK_SET);
    read(image_fd, block, BLOCK_SIZE);

    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    off_t offset = block_num * BLOCK_SIZE;

    lseek(image_fd, offset, SEEK_SET);
    write(image_fd, block, BLOCK_SIZE);
}

int alloc(void)
{
    unsigned char block[BLOCK_SIZE];
    bread(BLOCK_MAP_BLOCK, block);

    int free_block = find_free(block);
    if (free_block == -1) {
        return -1;
    }

    set_free(block, free_block, 1);
    bwrite(BLOCK_MAP_BLOCK, block);

    return free_block;
}