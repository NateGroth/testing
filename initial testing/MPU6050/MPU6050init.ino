#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const int testIterations = 30;   // Number of times to repeat the test
const int sampleRate = 100;      // Sampling rate in Hz
const int testDuration = 5000;   // Duration of each test in milliseconds (5 seconds)
float gyroCompensation[3][testIterations]; // Store compensation values for Gyro X, Y, Z
float accelCompensation[3][testIterations]; // Store compensation values for Accel X, Y, Z

void setup() {
    Serial.begin(9600);
    Wire.begin();
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!");
        while (1); // Halt if connection fails
    }
    Serial.println("MPU6050 connected successfully");
}

void loop() {
    for (int iteration = 0; iteration < testIterations; iteration++) {
        float gyroHigh[3] = {-10000, -10000, -10000}; // Initialize high values
        float gyroLow[3] = {10000, 10000, 10000};     // Initialize low values
        float accelHigh[3] = {-10000, -10000, -10000}; // Initialize high values
        float accelLow[3] = {10000, 10000, 10000};     // Initialize low values

        unsigned long startTime = millis();

        while (millis() - startTime < testDuration) {
            int16_t gx, gy, gz, ax, ay, az;
            mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

            // Update high and low values for gyro
            gyroHigh[0] = max(gyroHigh[0], gx);
            gyroHigh[1] = max(gyroHigh[1], gy);
            gyroHigh[2] = max(gyroHigh[2], gz);

            gyroLow[0] = min(gyroLow[0], gx);
            gyroLow[1] = min(gyroLow[1], gy);
            gyroLow[2] = min(gyroLow[2], gz);

            // Update high and low values for accelerometer
            accelHigh[0] = max(accelHigh[0], ax);
            accelHigh[1] = max(accelHigh[1], ay);
            accelHigh[2] = max(accelHigh[2], az);

            accelLow[0] = min(accelLow[0], ax);
            accelLow[1] = min(accelLow[1], ay);
            accelLow[2] = min(accelLow[2], az);

            delay(1000 / sampleRate); // Sampling delay to maintain 100 Hz
        }

        // Calculate average and compensation values
        for (int i = 0; i < 3; i++) {
            float gyroAvg = (gyroHigh[i] + gyroLow[i]) / 2.0;
            float accelAvg = (accelHigh[i] + accelLow[i]) / 2.0;
            gyroCompensation[i][iteration] = gyroAvg;   // Store compensation value for gyro
            accelCompensation[i][iteration] = accelAvg; // Store compensation value for accel
        }

        // Output the current compensation values to Serial
        Serial.print("Iteration ");
        Serial.print(iteration + 1);
        Serial.print(": Gyro Comp X=");
        Serial.print(gyroCompensation[0][iteration]);
        Serial.print(", Y=");
        Serial.print(gyroCompensation[1][iteration]);
        Serial.print(", Z=");
        Serial.print(gyroCompensation[2][iteration]);

        Serial.print(" | Accel Comp X=");
        Serial.print(accelCompensation[0][iteration]);
        Serial.print(", Y=");
        Serial.print(accelCompensation[1][iteration]);
        Serial.print(", Z=");
        Serial.println(accelCompensation[2][iteration]);
    }

    // Stop after running all iterations
    Serial.println("Testing complete.");
    while (1); // Halt the program after completing the tests
}
