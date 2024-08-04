#include "BMP280Setup.ino"
#include "BMP280Compensation.ino"

struct BMP280Data {
    float temperature;
    float pressure;
    float altitude;
};

BMP280Data lastGoodData = {0, 0, 0};
BMP280Data currentData;

// Convert barometric pressure to altitude using the barometric formula
float pressureToAltitude(float pressure, float seaLevelPressure = 1013.25) {
    return 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

// Read data from BMP280 sensor and handle temperature spikes
void readBMP280Data() {
    // Read current data from BMP280
    currentData.temperature = bmp.readTemperature();
    currentData.pressure = bmp.readPressure() / 100.0F; // Convert from Pa to hPa
    currentData.altitude = pressureToAltitude(currentData.pressure);

    // Check for temperature spike
    if (checkTemperatureSpike()) {
        // If a spike is detected, use the last known good data
        currentData = lastGoodData;
    } else {
        // Update last good data with current readings
        lastGoodData = currentData;
    }
}

// Expose a function to retrieve BMP280 data for the master parser
BMP280Data getBMP280Data() {
    return currentData;
}