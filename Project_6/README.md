# VVSFS Project 6

This project implements the beginning of a simple file system.

## What it does

The program can:

- Open and create a simulated disk image file
- Close the image file
- Write a 4096-byte block to the image
- Read a 4096-byte block from the image
- Run tests using `ctest.h`

## Files

- `image.c` / `image.h`: image file open and close functions
- `block.c` / `block.h`: block read and write functions
- `testfs.c`: test program
- `Makefile`: builds the project and test executable
- `ctest.h`: C testing, provided by the magnanimous Beej Jorgenson