#include <stdio.h>
#include <stdlib.h>

void grow_stack(int depth) {
    int local_var = depth;
    printf("Plumbing: %d: %p\n", depth, (void*)&local_var);
    
    if (depth < 5) {
        grow_stack(depth + 1);
    }
}

int main() {
    printf("Stack growth:\n");
    grow_stack(1);

    printf("\nHeap growth:\n");
    for (int i = 1; i <= 5; i++) {
        int *heap_var = malloc(sizeof(int));
        *heap_var = i;
        printf("Heap variable %d: %p\n", i, (void*)heap_var);
    }

    return 0;
}