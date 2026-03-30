CC = gcc
CFLAGS = -g -Wall -pedantic -Wno-strict-prototypes
TARGET = main
SRC = main.c banking.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
