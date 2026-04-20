#include <stdio.h>
#include <stdlib.h>

int global_var = 42;

int main() {
    int stack_var = 42;
    int *heap_var = malloc(sizeof(int));
    *heap_var = 42;

    printf("Stack variable address: %p\n", (void*)&stack_var);
    printf("Heap variable address: %p\n", (void*)heap_var);
    printf("Global variable address: %p\n", (void*)&global_var);

    free(heap_var);
    return 0;
}

// Run your program multiple times. Are the addresses always the same? If not, investigate why not.

// No, the addresses are not always the same. This is because of a security feature called Address Space Layout Randomization (ASLR) that randomizes the memory addresses used by a program each time it runs. This makes it more difficult for attackers to predict where certain data is located in memory, which can help prevent certain types of attacks.