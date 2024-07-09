#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; 

const unsigned long runDuration = 300000; 
const unsigned long sampleInterval = 1000; 

void setup() {
    Serial.begin(9600);
    if (!bmp.begin(0x76)) { 
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1);
    }

    // CHANGE THESE VALUES FOR DIFFERENT OVERSAMPLING RATES
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X16,    // Temperature oversampling
                    Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                    Adafruit_BMP280::FILTER_X16,      // Filter coefficient
                    Adafruit_BMP280::STANDBY_MS_500); 

    Serial.println("BMP280 Temperature and Pressure Logging Started");
}

void loop() {
    unsigned long startTime = millis();

    while (millis() - startTime < runDuration) {
        float temperature = bmp.readTemperature();
        float pressure = bmp.readPressure() / 100.0F; 

        
        Serial.print("Temperature (C): ");
        Serial.print(temperature);
        Serial.print(", Pressure (hPa): ");
        Serial.println(pressure);

        delay(sampleInterval); 
    }

    Serial.println("BMP280 Temperature and Pressure Logging Complete");
    while (1); 
}
