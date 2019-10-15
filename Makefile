main: main.c
	gcc -Wall -o main main.c -I/usr/local/include -L/usr/local/lib -lm -lwiringPi -lpthread

test: test.c
	gcc -Wall -o test test.c -I/usr/local/include -L/usr/local/lib -lm -lwiringPi -lpthread
