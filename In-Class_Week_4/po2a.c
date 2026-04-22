#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <page> <offset>\n", argv[0]);
        return 1;
    }

    int page = atoi(argv[1]);
    int offset = atoi(argv[2]);

    int address = (page << 8) | offset;

    printf("Page %d, offset %d = Address %d\n", page, offset, address);

    return 0;
}