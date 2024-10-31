all:
	gcc -c memory_manager.c
	gcc -c test_main.c
	gcc -c bitmap.c
	gcc memory_manager.o bitmap.o test_main.o -o myexec