CC = gcc
CFLAGS = -g -Wall -pedantic -Wno-strict-prototypes
TARGET = main
SRC = main.c banking.c common_utils.c input.c validation.c user_repository.c account_service.c insurance_service.c ui_render.c ui_menu.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
