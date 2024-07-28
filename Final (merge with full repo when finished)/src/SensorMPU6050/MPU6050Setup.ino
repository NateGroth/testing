#include <MPU6050.h>
#include <Wire.h>

#define MPU1_ADDRESS 0x68
#define MPU2_ADDRESS 0x69

MPU6050 mpu1(MPU1_ADDRESS);
MPU6050 mpu2(MPU2_ADDRESS);

struct MPUOffsets {
    int16_t accelOffset[3];
    int16_t gyroOffset[3];
};

MPUOffsets mpu1Offsets, mpu2Offsets;

// Initialize and calibrate the MPU sensors, storing offsets
void initializeAndCalibrateMPUs() {
    Wire.begin();
    mpu1.initialize();
    mpu2.initialize();
    
    if (mpu1.testConnection()) {
        calibrateMPU(mpu1, mpu1Offsets);
    }
    if (mpu2.testConnection()) {
        calibrateMPU(mpu2, mpu2Offsets);
    }
}

// Calibrate an MPU sensor and save offsets
void calibrateMPU(MPU6050 &mpu, MPUOffsets &offsets) {
    mpu.CalibrateAccel(6); // Adjust sensitivity as needed
    mpu.CalibrateGyro(6);
    
    offsets.accelOffset[0] = mpu.getXAccelOffset();
    offsets.accelOffset[1] = mpu.getYAccelOffset();
    offsets.accelOffset[2] = mpu.getZAccelOffset();
    offsets.gyroOffset[0] = mpu.getXGyroOffset();
    offsets.gyroOffset[1] = mpu.getYGyroOffset();
    offsets.gyroOffset[2] = mpu.getZGyroOffset();
}

// Apply saved offsets to the MPU sensors
void applyOffsets() {
    mpu1.setXAccelOffset(mpu1Offsets.accelOffset[0]);
    mpu1.setYAccelOffset(mpu1Offsets.accelOffset[1]);
    mpu1.setZAccelOffset(mpu1Offsets.accelOffset[2]);
    mpu1.setXGyroOffset(mpu1Offsets.gyroOffset[0]);
    mpu1.setYGyroOffset(mpu1Offsets.gyroOffset[1]);
    mpu1.setZGyroOffset(mpu1Offsets.gyroOffset[2]);

    if (mpu2.testConnection()) {
        mpu2.setXAccelOffset(mpu2Offsets.accelOffset[0]);
        mpu2.setYAccelOffset(mpu2Offsets.accelOffset[1]);
        mpu2.setZAccelOffset(mpu2Offsets.gyroOffset[2]);
        mpu2.setXGyroOffset(mpu2Offsets.gyroOffset[0]);
        mpu2.setYGyroOffset(mpu2Offsets.gyroOffset[1]);
        mpu2.setZGyroOffset(mpu2Offsets.gyroOffset[2]);
    }
}