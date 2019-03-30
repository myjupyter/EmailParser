GCC = gcc
CFLAGS = -Wall 
TARGET = main.out
SRC = main.c \

GCC_LINK = -I/usr/include -L/usr/local/lib -L/usr/lib -lpcre

all:
	$(GCC) $(CFLAGS) $(GCC_LINK) $(SRC) -o $(TARGET)

clean:
	rm -rf $(TARGET)
