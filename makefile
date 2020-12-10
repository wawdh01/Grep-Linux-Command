grep: main_grep.o list.o
	gcc -o grep getotp.o list.o
getotp.o: main_grep.c
	gcc -c -o main_grep.o main_grep.c
list.o: list.c
	gcc -c -o list.o list.c
clean:
	rm *.o 
