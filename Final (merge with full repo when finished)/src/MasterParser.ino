#include "SensorMPU6050/MPU6050Data.ino"
#include "SensorBMP280/BMP280Data.ino"
#include "SensorMPXV7002/MPXV7002Data.ino"

void parseData() {
    // Retrieve data from each sensor
    float mpuData = MPU6050_readData();
    float bmpData = BMP280_readData();
    float mpvData = MPXV7002_readData();

    // Apply compensations
    mpuData = MPU6050_applyCompensation(mpuData);
    bmpData = BMP280_applyCompensation(bmpData);
    mpvData = MPXV7002_applyCompensation(mpvData);

    // Final parsed data output (modify as needed)
    Serial.print("MPU6050: "); Serial.println(mpuData);
    Serial.print("BMP280: "); Serial.println(bmpData);
    Serial.print("MPXV7002: "); Serial.println(mpvData);
}