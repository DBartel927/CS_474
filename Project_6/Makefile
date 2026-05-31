CC = gcc
CFLAGS = -Wall -Wextra -Werror -DCTEST_ENABLE
AR = ar
ARFLAGS = rcs

LIB = libvvsfs.a
TEST = testfs

LIB_OBJS = image.o block.o free.o inode.o mkfs.o pack.o dir.o ls.o
TEST_OBJS = testfs.o

all: $(LIB) $(TEST)

$(LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $(LIB) $(LIB_OBJS)

$(TEST): $(TEST_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $(TEST) $(TEST_OBJS) $(LIB)

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c

block.o: block.c block.h image.h free.h
	$(CC) $(CFLAGS) -c block.c

free.o: free.c free.h block.h
	$(CC) $(CFLAGS) -c free.c

inode.o: inode.c inode.h block.h free.h pack.h
	$(CC) $(CFLAGS) -c inode.c

mkfs.o: mkfs.c mkfs.h block.h free.h
	$(CC) $(CFLAGS) -c mkfs.c

pack.o: pack.c pack.h
	$(CC) $(CFLAGS) -c pack.c

dir.o: dir.c dir.h block.h inode.h pack.h
	$(CC) $(CFLAGS) -c dir.c

ls.o: ls.c ls.h dir.h
	$(CC) $(CFLAGS) -c ls.c

testfs.o: testfs.c image.h block.h free.h inode.h mkfs.h pack.h dir.h ls.h ctest.h
	$(CC) $(CFLAGS) -c testfs.c

test: $(TEST)
	./$(TEST)

clean:
	rm -f *.o

pristine: clean
	rm -f $(TEST) *.a test.img

.PHONY: all test clean pristine