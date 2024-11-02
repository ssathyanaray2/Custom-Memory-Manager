CC = gcc
OBJ = memory_manager.o bitmap.o
LIB = memory_manager.a

all: 
	$(CC) -c memory_manager.c -o memory_manager.o
	$(CC) -c bitmap.c -o bitmap.o
	ar rcs $(LIB) $(OBJ)

clean:
	rm -f $(OBJ) $(LIB)
