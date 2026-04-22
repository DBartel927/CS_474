#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <address>\n", argv[0]);
        return 1;
    }

    int address = atoi(argv[1]);

    int page = (address >> 8);
    int offset = (address & 0xFF);

    printf("Address %d = Page %d, offset %d\n", address, page, offset);

    return 0;
}