CC = gcc
CFLAGS = -I /usr/local/include -I ./BMP280_driver -L /usr/local/lib -lm -lwiringPi -lpthread
DEPS = bmp280.h
OBJ = main.o ./BMP280_driver/bmp280.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
