#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "bmp180.h"

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
	fprintf(flight_data, "%.2f\t%.2f\n", data.time, data.altitude);
	fclose(flight_data);
	return 0;
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
	unsigned int start_time = millis();
	while (running) {
		double time = (double) (millis() - start_time) / (double) 1e3;

		// BMP180
		calibrate(psens);
		double T = getTemperature(psens);
		double P = getPressure(psens);
		double a = getAltitude(psens);
		printf("Time: %.2f\tAltitude: %.2f\n", time, a);

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
