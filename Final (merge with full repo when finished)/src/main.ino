#include "../lib/I2CCommunication.ino"
#include "../lib/MasterParser.ino"

void setup() {
    I2C_init();               // Initialize I2C
    MPU6050_setup();          // Setup MPU6050
    BMP280_setup();           // Setup BMP280
    MPXV7002_setup();         // Setup MPXV7002
}

void loop() {
    parseData();              // Main data parsing and compensation
    delay(100);               // Adjust delay as needed
}