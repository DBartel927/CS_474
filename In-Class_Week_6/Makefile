CC = gcc
CFLAGS = -Wall -Wextra
TARGET = main

.PHONY: clean pristine

$(TARGET): main.o adder.o
	$(CC) -o $(TARGET) main.o adder.o

main.o: main.c adder.h
	$(CC) $(CFLAGS) -c main.c

adder.o: adder.c adder.h
	$(CC) $(CFLAGS) -c adder.c

clean:
	rm -f *.o

pristine: clean
	rm -f $(TARGET)