#include "BMP280Setup.ino"

float lastTemperature = 0;
bool spikeDetected = false;
const float tempSpikeThreshold = 2.0; // Temperature change threshold for detecting spikes

// Check for a temperature spike
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

// Reset the spike detection flag after handling the spike
void resetSpikeDetection() {
    spikeDetected = false;
}