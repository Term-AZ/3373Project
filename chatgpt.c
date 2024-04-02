#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define SENSOR_ADDRESS 0x48 // Address of the pressure sensor
#define DISPLAY_ADDRESS 0x20 // Address of the 7 segment display

// Function to read data from the pressure sensor
uint16_t readPressureSensor(int file) {
    // Command to read data from pressure sensor
    uint8_t command = 0x00;
    uint8_t data[2];

    // Write command to pressure sensor
    if (write(file, &command, 1) != 1) {
        perror("Failed to write to the sensor.\n");
        exit(1);
    }

    // Read data from pressure sensor
    if (read(file, data, 2) != 2) {
        perror("Failed to read from the sensor.\n");
        exit(1);
    }

    uint16_t pressure = (data[0] << 8) | data[1];
    return pressure;
}

// Function to display data on 7 segment display
void displayOn7Segment(int file, uint16_t data) {
    // Convert data to string
    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%d", data);

    // Write data to 7 segment display
    if (write(file, buffer, sizeof(buffer)) != sizeof(buffer)) {
        perror("Failed to write to the display.\n");
        exit(1);
    }
}

int main() {
    // Open I2C bus
    int file;
    if ((file = open("/dev/i2c-0", O_RDWR)) < 0) {
        perror("Failed to open the I2C bus.\n");
        exit(1);
    }

    // Set I2C slave addresses
    if (ioctl(file, I2C_SLAVE, SENSOR_ADDRESS) < 0) {
        perror("Failed to set I2C sensor address.\n");
        exit(1);
    }
    if (ioctl(file, I2C_SLAVE, DISPLAY_ADDRESS) < 0) {
        perror("Failed to set I2C display address.\n");
        exit(1);
    }

    // Loop to continuously read and display data
    while (1) {
        // Read data from pressure sensor
        uint16_t pressureData = readPressureSensor(file);

        // Display data on 7 segment display
        displayOn7Segment(file, pressureData);

        // Delay for some time before reading again
        usleep(500000); // 500ms delay
    }

    // Close I2C bus
    close(file);

    return 0;
}