#include <stdlib.h>
#include <string.h>

#include "block.h"
#include "dir.h"
#include "inode.h"
#include "pack.h"

static char *get_dirname(const char *path, char *dirname)
{
    strcpy(dirname, path);

    char *p = strrchr(dirname, '/');

    if (p == NULL) {
        strcpy(dirname, ".");
        return dirname;
    }

    if (p == dirname) {
        *(p + 1) = '\0';
    } else {
        *p = '\0';
    }

    return dirname;
}

static char *get_basename(const char *path, char *basename)
{
    if (strcmp(path, "/") == 0) {
        strcpy(basename, path);
        return basename;
    }

    const char *p = strrchr(path, '/');

    if (p == NULL) {
        p = path;
    } else {
        p++;
    }

    strcpy(basename, p);

    return basename;
}

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

    ent->inode_num = read_u16(block + offset_in_block + DIRECTORY_INODE_OFFSET);
    strcpy(ent->name, (char *)(block + offset_in_block + DIRECTORY_NAME_OFFSET));

    dir->offset += DIRECTORY_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *dir)
{
    iput(dir->inode);
    free(dir);
}

struct inode *namei(char *path)
{
    struct directory *dir;
    struct directory_entry ent;
    char dirname[1024];
    char basename[1024];

    if (path == NULL || path[0] != '/') {
        return NULL;
    }

    if (strcmp(path, "/") == 0) {
        return iget(ROOT_INODE_NUM);
    }

    get_dirname(path, dirname);
    get_basename(path, basename);

    if (strcmp(dirname, "/") != 0) {
        return NULL;
    }

    dir = directory_open(ROOT_INODE_NUM);

    if (dir == NULL) {
        return NULL;
    }

    while (directory_get(dir, &ent) != -1) {
        if (strcmp(ent.name, basename) == 0) {
            unsigned int inode_num = ent.inode_num;

            directory_close(dir);

            return iget(inode_num);
        }
    }

    directory_close(dir);

    return NULL;
}

int directory_make(char *path)
{
    unsigned char block[BLOCK_SIZE];
    char dirname[1024];
    char basename[1024];
    struct inode *parent;
    struct inode *new_dir;
    struct inode *already_exists;
    int new_block;
    unsigned int parent_offset;
    unsigned int parent_block_index;
    unsigned int parent_block_num;
    unsigned int offset_in_block;

    if (path == NULL || path[0] != '/') {
        return -1;
    }

    get_dirname(path, dirname);
    get_basename(path, basename);

    if (strcmp(dirname, "/") != 0 ||
        strcmp(basename, "/") == 0 ||
        strcmp(basename, "") == 0) {
        return -1;
    }

    already_exists = namei(path);

    if (already_exists != NULL) {
        iput(already_exists);
        return -1;
    }

    parent = namei(dirname);

    if (parent == NULL) {
        return -1;
    }

    if (parent->size >= BLOCK_SIZE) {
        iput(parent);
        return -1;
    }

    new_dir = ialloc();

    if (new_dir == NULL) {
        iput(parent);
        return -1;
    }

    new_block = alloc();

    if (new_block == -1) {
        iput(new_dir);
        iput(parent);
        return -1;
    }

    new_dir->flags = DIRECTORY_FLAG;
    new_dir->size = DIRECTORY_ENTRY_SIZE * 2;
    new_dir->block_ptr[0] = new_block;

    memset(block, 0, BLOCK_SIZE);

    write_u16(block, new_dir->inode_num);
    strcpy((char *)(block + DIRECTORY_NAME_OFFSET), ".");

    write_u16(block + DIRECTORY_ENTRY_SIZE, parent->inode_num);
    strcpy((char *)(block + DIRECTORY_ENTRY_SIZE + DIRECTORY_NAME_OFFSET), "..");

    bwrite(new_block, block);

    parent_offset = parent->size;
    parent_block_index = parent_offset / BLOCK_SIZE;
    parent_block_num = parent->block_ptr[parent_block_index];
    offset_in_block = parent_offset % BLOCK_SIZE;

    bread(parent_block_num, block);

    write_u16(block + offset_in_block, new_dir->inode_num);
    strcpy((char *)(block + offset_in_block + DIRECTORY_NAME_OFFSET), basename);

    bwrite(parent_block_num, block);

    parent->size += DIRECTORY_ENTRY_SIZE;

    iput(new_dir);
    iput(parent);

    return 0;
}