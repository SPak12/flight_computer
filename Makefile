main: main.c
	gcc -Wall -o main main.c -I/usr/local/include -L/usr/local/lib -lm -lwiringPi -lpthread
