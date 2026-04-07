# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -pthread

# All source files
SRCS = spinlock.c compareswap.c

# Convert .c -> executable names
TARGETS = $(SRCS:.c=)

# Default target
all: $(TARGETS)

# Rule to build each program
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean build files
clean:
	rm -f $(TARGETS)