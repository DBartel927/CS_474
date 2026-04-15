# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -pthread

# All source files
SRCS = arbitrarystorage.c withloopsthistime.c split.c coalesce.c

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