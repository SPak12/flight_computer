CC = gcc
CFLAGS = -I /usr/local/include ./BMP280_driver ./BNO055_driver -L /usr/local/lib -lm -lwiringPi -lpthread
DEPS = bmp280.h
OBJ = main.o altimeter.o imu.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
