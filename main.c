#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "altimeter.h"

int running; // Flag

// Struct for storing data
struct dataStruct {
	double time;
	double temperature;
	double pressure;
	double altitude;
} data;

void *telemetry(void *input) {
	printf("Doing telemetry...\t");
	return 0;
}

void *datalogging(void *input) {
	FILE *flight_data = fopen("flight_data.txt", "a");
	if (flight_data == NULL) {
		printf("\nError opening file!\n");
		while(1); // Wait forever
	}
	fprintf(flight_data, "%.1f\t%.1f\n", data.time, data.altitude);
	fclose(flight_data);
	return 0;
}

// Returns monotonic time since start in seconds
uint64_t us_since_start(struct timespec start_time) {
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC_RAW, &time);
	return (time.tv_sec - start_time.tv_sec) * 1e6 + (time.tv_nsec - start_time.tv_nsec) / 1e3;
}

// Updates data for telemetry and datalogging
void updateData(double t, double T, double P, double a) {
	data.time = t;
	data.temperature = T;
	data.pressure = P;
	data.altitude = a;
}

int main(int argc, char *argv[]) {
	running = 1;
	wiringPiSetup();
	int psens = wiringPiI2CSetup(BMP180_ADDR);
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
	while (running) {
		double time = (double) us_since_start(start_time) / (double) 1e6;

		// BMP180
		calibrate(psens);
		double T = getTemperature(psens);
		double P = getPressure(psens);
		double a = getAltitude(psens);
		printf("Time: %.1f\tTemperature: %.1f\tPressure: %.0f\tAltitude: %.1f\n", time, T, P, a);

		updateData(time, T, P, a); // Update data for this time step

		// Telemetry and Datalogging (Multithreading)

		pthread_t telemetry_t;
		pthread_t datalogging_t;
		
		pthread_create(&telemetry_t, NULL, telemetry, NULL);
		pthread_create(&datalogging_t, NULL, datalogging, NULL);

		pthread_join(telemetry_t, NULL);
		pthread_join(datalogging_t, NULL);
	}
	return 0;
}
