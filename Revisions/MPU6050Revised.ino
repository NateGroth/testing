#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_BMP280.h>

MPU6050 mpu1(0x68);       // MPU6050 at address 0x68
MPU6050 mpu2(0x69);       // MPU6050 at address 0x69 (if available)
Adafruit_BMP280 bmp;      // BMP280 for temperature and pressure

// Placeholder values for system offsets (acceleration and gyroscope for two MPUs)
int16_t systemAccelOffset[3] = {0, 0, 0};  // X, Y, Z offsets for MPU1 accelerometer
int16_t systemGyroOffset1[3] = {0, 0, 0};   // X, Y, Z offsets for MPU1 gyroscope


// Buffers for temperature and MPU readings
const int bufferSize = 10;
float temperatureBuffer[bufferSize];
int16_t mpuBuffer[bufferSize][6];  // Buffer for 6 MPU values (AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ)

// Initialize buffer indices
int tempBufferIndex = 0;
int mpuBufferIndex = 0;

//Temperature Threshold
float tempThreshold = 5.0

// Function to calibrate MPU and set offsets
void calibrateMPU(MPU6050 &mpu, int16_t accelOffset[], int16_t gyroOffset[]) {
    mpu.CalibrateAccel(6);   
    mpu.CalibrateGyro(6);    
}

// Function to read and calibrate all sensors, saving offsets to placeholders
void calibrateSystem() {
    int numSamples = 100; 
    int32_t accelSum[3] = {0, 0, 0}; 
    int32_t gyroSum[3] = {0, 0, 0};  
    int16_t ax1, ay1, az1, gx1, gy1, gz1;
    int16_t ax2, ay2, az2, gx2, gy2, gz2;

    // Collect and sum readings from both sensors
    for (int i = 0; i < numSamples; i++) {
        mpu1.getMotion6(&ax1, &ay1, &az1, &gx1, &gy1, &gz1);
        accelSum[0] += ax1;
        accelSum[1] += ay1;
        accelSum[2] += az1;
        gyroSum[0] += gx1;
        gyroSum[1] += gy1;
        gyroSum[2] += gz1;

        if (mpu2.testConnection()) {
            mpu2.getMotion6(&ax2, &ay2, &az2, &gx2, &gy2, &gz2);
            accelSum[0] += ax2;
            accelSum[1] += ay2;
            accelSum[2] += az2;
            gyroSum[0] += gx2;
            gyroSum[1] += gy2;
            gyroSum[2] += gz2;
        }

        delay(10); 
    }

    // Calculate average readings by dividing by the number of samples
    int16_t accelAvg[3], gyroAvg[3];
    accelAvg[0] = accelSum[0] / (numSamples * 2);
    accelAvg[1] = accelSum[1] / (numSamples * 2);
    accelAvg[2] = accelSum[2] / (numSamples * 2);
    gyroAvg[0] = gyroSum[0] / (numSamples * 2);
    gyroAvg[1] = gyroSum[1] / (numSamples * 2);
    gyroAvg[2] = gyroSum[2] / (numSamples * 2);

    // Calculate the difference from zero (nominal) as the system offset
    accelOffset1[0] = -accelAvg[0];
    accelOffset1[1] = -accelAvg[1];
    accelOffset1[2] = -accelAvg[2];
    gyroOffset1[0] = -gyroAvg[0];
    gyroOffset1[1] = -gyroAvg[1];
    gyroOffset1[2] = -gyroAvg[2];

    // Set calculated offsets to both sensors
    systemAccelOffset[0] =  accelOffset1[0];
    systemAccelOffset[1] =  accelOffset1[1];
    systemAccelOffset[2] =  accelOffset1[2];
    systemGyroOffset[0] =  gyroOffset1[0];
    systemGyroOffset[1] =  gyroOffset1[1];
    systemGyroOffset[2] =  gyroOffset1[2];

    Serial.println("System calibration complete:");
    Serial.print("Accel Offsets (X, Y, Z): ");
    Serial.print(systemAccelOffset[0]); Serial.print(", ");
    Serial.print(systemAccelOffset[1]); Serial.print(", ");
    Serial.println(systemAccelOffset[2]);
    Serial.print("Gyro Offsets (X, Y, Z): ");
    Serial.print(systemGyroOffset[0]); Serial.print(", ");
    Serial.print(systemGyroOffset[1]); Serial.print(", ");
    Serial.println(gsystemGyroOffset[2]);
}


// Initialize all sensors
void initializeSensors() {
    Wire.begin();
    mpu1.initialize();
    if (mpu2.testConnection()) {
        mpu2.initialize();
    }
    if (!bmp.begin(0x76)) {
        Serial.println("BMP280 initialization failed!");
        while (1);
    }
}

// Function to read values from all sensors
void readSensors() {
    int16_t ax, ay, az, gx, gy, gz;
    
    mpu1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    addToMPUBuffer(ax, ay, az, gx, gy, gz);
    
    if (mpu2.testConnection()) {
        mpu2.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        addToMPUBuffer(ax, ay, az, gx, gy, gz);
    }
    
    float temp = bmp.readTemperature();
    addToTemperatureBuffer(temp);
}

// Add values to MPU buffer
void addToMPUBuffer(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz) {
    mpuBuffer[mpuBufferIndex][0] = ax;
    mpuBuffer[mpuBufferIndex][1] = ay;
    mpuBuffer[mpuBufferIndex][2] = az;
    mpuBuffer[mpuBufferIndex][3] = gx;
    mpuBuffer[mpuBufferIndex][4] = gy;
    mpuBuffer[mpuBufferIndex][5] = gz;
    mpuBufferIndex = (mpuBufferIndex + 1) % bufferSize;
}

// Add temperature to buffer and detect spikes
void addToTemperatureBuffer(float temp) {
    float lastTemp = temperatureBuffer[tempBufferIndex];
    temperatureBuffer[tempBufferIndex] = temp;
    tempBufferIndex = (tempBufferIndex + 1) % bufferSize;
    
    if (abs(temp - lastTemp) > tempThreshold) {
        clearMPUBuffer();
        Serial.println("Temperature spike detected! Clearing MPU buffer.");
    }
}

// Function to clear the MPU buffer
void clearMPUBuffer() {
    for (int i = 0; i < bufferSize; i++) {
        for (int j = 0; j < 6; j++) {
            mpuBuffer[i][j] = 0;
        }
    }
}

// Print MPU buffer values
void printMPUBuffer() {
    Serial.println("MPU Readings:");
    for (int i = 0; i < bufferSize; i++) {
        Serial.print("AccelX: "); Serial.print(mpuBuffer[i][0]);
        Serial.print(" AccelY: "); Serial.print(mpuBuffer[i][1]);
        Serial.print(" AccelZ: "); Serial.print(mpuBuffer[i][2]);
        Serial.print(" GyroX: "); Serial.print(mpuBuffer[i][3]);
        Serial.print(" GyroY: "); Serial.print(mpuBuffer[i][4]);
        Serial.print(" GyroZ: "); Serial.println(mpuBuffer[i][5]);
    }
    Serial.println("----");
}

void setup() {
    Serial.begin(9600);
    initializeSensors();
    calibrateSystem();
}

void loop() {
    // Read values from sensors
    readSensors();
    
    // Print MPU buffer values to Serial
    printMPUBuffer();
    
    delay(100); // Adjust sampling rate as needed
}
