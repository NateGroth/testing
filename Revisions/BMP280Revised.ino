#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; /

const int bufferSize = 10;
float barometerBuffer[bufferSize];
int bufferIndex = 0;

// Temperature spike threshold
float lastTemperature = 0;
float tempSpikeThreshold = 5.0; 

void setup() {
    Serial.begin(9600);

    if (!bmp.begin(0x76)) { 
        Serial.println("BMP280 initialization failed! Check wiring.");
        while (1);
    }


    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X16,   
                    Adafruit_BMP280::SAMPLING_X16,    
                    Adafruit_BMP280::FILTER_X16,    
                    Adafruit_BMP280::STANDBY_MS_500); 

    Serial.println("BMP280 Temperature, Barometer, and Altitude Monitoring Started");
}

float pressureToAltitude(float pressure, float seaLevelPressure = 1013.25) {
    return 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

void addToBarometerBuffer(float pressure) {
    barometerBuffer[bufferIndex] = pressure;
    bufferIndex = (bufferIndex + 1) % bufferSize;
}

void clearBarometerBuffer() {
    for (int i = 0; i < bufferSize; i++) {
        barometerBuffer[i] = 0;
    }
    Serial.println("Temperature spike detected! Clearing barometer buffer.");
}

void loop() {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F; // Convert pressure from Pa to hPa
    if (abs(temperature - lastTemperature) > tempSpikeThreshold) {
        clearBarometerBuffer();
    }
    lastTemperature = temperature;
    addToBarometerBuffer(pressure);
    float altitude = pressureToAltitude(pressure);


    Serial.print("Temperature (°C): ");
    Serial.print(temperature);
    Serial.print(" | Pressure (hPa): ");
    Serial.print(pressure);
    Serial.print(" | Altitude (m): ");
    Serial.println(altitude);

    delay(1000);
}