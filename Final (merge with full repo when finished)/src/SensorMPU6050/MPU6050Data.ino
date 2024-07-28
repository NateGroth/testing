#include "MPU6050Setup.ino"
#include "MPU6050Compensation.ino"

struct MPUData {
    int16_t accel[3];
    int16_t gyro[3];
};

// Buffers for storing recent sensor data
MPUData lastGoodData = {0, 0, 0, 0, 0, 0};
MPUData currentData;

// Function to read and combine data from both sensors, applying offsets and handling spikes
void readAndCompensateMPUData() {
    // Apply saved offsets to the sensors
    applyOffsets();

    MPUData mpu1Data, mpu2Data;
    int16_t ax, ay, az, gx, gy, gz;

    // Read data from MPU1 with offsets applied
    mpu1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    mpu1Data.accel[0] = ax - mpu1Offsets.accelOffset[0];
    mpu1Data.accel[1] = ay - mpu1Offsets.accelOffset[1];
    mpu1Data.accel[2] = az - mpu1Offsets.accelOffset[2];
    mpu1Data.gyro[0] = gx - mpu1Offsets.gyroOffset[0];
    mpu1Data.gyro[1] = gy - mpu1Offsets.gyroOffset[1];
    mpu1Data.gyro[2] = gz - mpu1Offsets.gyroOffset[2];

    // Read data from MPU2 with offsets applied, if connected
    if (mpu2.testConnection()) {
        mpu2.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        mpu2Data.accel[0] = ax - mpu2Offsets.accelOffset[0];
        mpu2Data.accel[1] = ay - mpu2Offsets.accelOffset[1];
        mpu2Data.accel[2] = az - mpu2Offsets.accelOffset[2];
        mpu2Data.gyro[0] = gx - mpu2Offsets.gyroOffset[0];
        mpu2Data.gyro[1] = gy - mpu2Offsets.gyroOffset[1];
        mpu2Data.gyro[2] = gz - mpu2Offsets.gyroOffset[2];
    } else {
        mpu2Data = mpu1Data; // If only one MPU is connected, duplicate MPU1 data for averaging
    }

    // Average data from both sensors
    for (int i = 0; i < 3; i++) {
        currentData.accel[i] = (mpu1Data.accel[i] + mpu2Data.accel[i]) / 2;
        currentData.gyro[i] = (mpu1Data.gyro[i] + mpu2Data.gyro[i]) / 2;
    }

    // If a spike is detected, use the last known good data
    if (checkTemperatureSpike()) {
        currentData = lastGoodData;
    } else {
        lastGoodData = currentData;
    }
}

// Expose a function to retrieve the current MPU data for the master parser
MPUData getMPUData() {
    return currentData;
}