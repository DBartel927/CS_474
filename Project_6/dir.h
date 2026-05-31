#ifndef DIR_H
#define DIR_H

#include "block.h"
#include "inode.h"

#define DIRECTORY_ENTRY_SIZE 32
#define DIRECTORY_NAME_SIZE 16
#define DIRECTORY_FLAG 2

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[DIRECTORY_NAME_SIZE];
};

struct directory *directory_open(int inode_num);
int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *dir);

#endif