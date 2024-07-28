#include <Wire.h>

void I2C_init() {
    Wire.begin();          // Start I2C communication
}

float I2C_read(uint8_t address) {
    Wire.beginTransmission(address);
    Wire.requestFrom(address, 2);
    float data = (Wire.read() << 8) | Wire.read();
    return data;
}