CC = gcc
CFLAGS = -I /usr/local/include -I ./BMP280_driver -L /usr/local/lib -lm -lwiringPi -lpthread
DEPS = bmp280.h
OBJ = test.o ./BMP280_driver/bmp280.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
