#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C interface for BMP280

// Initialize BMP280 with specified oversampling and filtering settings
void initializeBMP280() {
    if (!bmp.begin(0x76)) { // Default I2C address for BMP280 is 0x76
        Serial.println("BMP280 initialization failed! Check wiring.");
        while (1);
    }

    // Set BMP280 sampling and filtering
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X16,    // Temperature oversampling
                    Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                    Adafruit_BMP280::FILTER_X16,      // Filtering
                    Adafruit_BMP280::STANDBY_MS_500); // Standby time for smooth data

    Serial.println("BMP280 initialized successfully.");
}