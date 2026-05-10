CC = gcc
CFLAGS = -Wall -Wextra -Werror -DCTEST_ENABLE
AR = ar
ARFLAGS = rcs

LIB = libvvsfs.a
TEST = testfs

LIB_OBJS = image.o block.o
TEST_OBJS = testfs.o

all: $(LIB) $(TEST)

$(LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $(LIB) $(LIB_OBJS)

$(TEST): $(TEST_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $(TEST) $(TEST_OBJS) $(LIB)

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c

block.o: block.c block.h image.h
	$(CC) $(CFLAGS) -c block.c

testfs.o: testfs.c image.h block.h ctest.h
	$(CC) $(CFLAGS) -c testfs.c

test: $(TEST)
	./$(TEST)

clean:
	rm -f *.o

pristine: clean
	rm -f $(TEST) *.a test.img

.PHONY: all test clean pristine