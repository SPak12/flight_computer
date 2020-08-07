CC = gcc
CFLAGS = -I /usr/local/include -I ./BMP280_driver -I ./BNO055_driver -L /usr/local/lib -lm -lwiringPi -lpthread
DEPS = bmp280.h bno055.h altimeter.h imu.h
OBJ = main.o altimeter.o ./BMP280_driver/bmp280.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f ./*o ./BMP280_driver/bmp280.o
