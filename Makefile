CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = memory_manager.o bitmap.o
LIB = memory_manager.a

all: 
	$(CC) $(CFLAGS) -c memory_manager.c -o memory_manager.o
	$(CC) $(CFLAGS) -c bitmap.c -o bitmap.o
	ar rcs $(LIB) $(OBJ)

clean:
	rm -f $(OBJ) $(LIB)
