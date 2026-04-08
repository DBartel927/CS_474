#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

struct node {
    int data;
    struct node *next;
};

struct stack {
    _Atomic(struct node *) top;
};

/**
 * Construct a new stack
 */
struct stack *stack_new(void)
{
    struct stack *s = malloc(sizeof *s);
    s->top = NULL;

    return s;
}

/**
 * Construct a new node
 */
struct node *node_new(int data)
{
    struct node *node = malloc(sizeof *node);
    node->data = data;
    node->next = NULL;

    return node;
}

/**
 * Push onto the stack
 */
void push(struct stack *s, int val)
{
    struct node *new_node = node_new(val);
    struct node *current_top;

    do {
        new_node->next = current_top;
    } while (!atomic_compare_exchange_weak(&s->top, &current_top, new_node));
}

/**
 * Pop off stack
 *
 * Returns false on failure, true on success.
 */
int pop(struct stack *s, int *value)
{
    struct node *current_top;

    do {
        if (current_top == NULL) {
            return 0;
        }
    } while (!atomic_compare_exchange_weak(&s->top, &current_top, current_top->next));

    *value = current_top->data;
    free(current_top);
    return 1;
}

/**
 * Print the stack (not threadsafe)
 */
void print_stack(struct stack *s)
{
    struct node *p = s->top;

    printf("Stack (top):%s", p == NULL? " [empty]": "");

    while (p != NULL) {
        printf(" %d", p->data);
        p = p->next;
    }

    putchar('\n');
}

int main(void)
{
    struct stack *s = stack_new();

    print_stack(s);

    for (int i = 0; i < 4; i++) {
        push(s, i);
        print_stack(s);
  }

    int val;

    while (pop(s, &val)) {
        printf("Popped %d\n", val);
        print_stack(s);
    }
}

// Research: What are some solutions to the ABA problem?
// The pointers need to be updated in a way that they can't be confused. One way we might do this is to add a counter to the pointer, so that we can tell if it's been changed since we last looked at it. It seems that this is called a "tagged pointer", and we haven't messed with them yet, but I now see it coming.

// Research: What are some solutions to the Hazard Pointers Problem?
// One solution is to have a "retire list" of nodes that have been popped but not yet freed. When we pop a node, we add it to the retire list instead of freeing it immediately. Then, when we want to free a node, we check if it's still in the retire list. If it is, we know that it's safe to free it. If it's not, we know that it's still being used by another thread and we shouldn't free it yet. But of course, what happens when two different threads pop the same node at the same time? We might end up with two threads trying to add the same node to the retire list, which could cause a race condition. My thought would be to use a lock to protect the retire list, but that seems to defeat the purpose of having a lock-free stack in the first place. This needs some more thought to be sure.