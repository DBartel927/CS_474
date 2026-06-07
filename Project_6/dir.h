#ifndef DIR_H
#define DIR_H

#include "block.h"
#include "inode.h"

#define ROOT_INODE_NUM 0

#define DIRECTORY_ENTRY_SIZE 32
#define DIRECTORY_NAME_SIZE 16
#define DIRECTORY_FLAG 2
#define DIRECTORY_INODE_OFFSET 0
#define DIRECTORY_NAME_OFFSET 2

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

struct inode *namei(char *path);
int directory_make(char *path);

#endif