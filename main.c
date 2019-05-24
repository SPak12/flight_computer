#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "altimeter.h"

FILE *flight_data;
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
	flight_data = fopen("flight_data.txt", "w");
	fprintf(flight_data, "%.1f\t%.1f\n", data.time, data.altitude);
	fclose(flight_data);
	return 0;
}

// Returns monotonic time since start in seconds
double getTime(uint64_t start_time) {
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
	return (double) (time.tv_nsec - start_time) / (double) 10e9;
}

// Updates data for telemetry and datalogging
void updateData(double time, double temperature, double pressure, double altitude) {
	data.time = time;
	data.temperature = temperature;
	data.pressure = pressure;
	data.altitude = altitude;
}

int main(int argc, char *argv[]) {
	running = 1;
	wiringPiSetup();
	int psens = wiringPiI2CSetup(BMP180_ADDR);
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	uint64_t start_time = start.tv_nsec;
	while (running) {
		double time = getTime(start_time);

		// BMP180
		calibrate(psens);
		double T = getTemperature(psens);
		double P = getPressure(psens);
		double a = getAltitude(psens);
		
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
