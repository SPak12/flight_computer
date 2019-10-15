test: test.c
	gcc -Wall -o test test.c -I/usr/local/include -L/usr/local/lib -lm -lwiringPi -lpthread
