#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // Initialize BMP280 for temperature reading

float lastTemperature = 0;
bool spikeDetected = false;
const float tempSpikeThreshold = 2.0; // Threshold for temperature spike (adjust as needed)

// Initialize the BMP280 temperature sensor
void initializeTemperatureSensor() {
    if (!bmp.begin(0x76)) {
        Serial.println("BMP280 initialization failed!");
        while (1);
    }
}

// Check for temperature spike
bool checkTemperatureSpike() {
    float currentTemperature = bmp.readTemperature();
    
    if (abs(currentTemperature - lastTemperature) > tempSpikeThreshold) {
        spikeDetected = true;
    } else {
        spikeDetected = false;
    }

    lastTemperature = currentTemperature;
    return spikeDetected;
}

// Function to reset the spike status after handling
void resetSpikeDetection() {
    spikeDetected = false;
}