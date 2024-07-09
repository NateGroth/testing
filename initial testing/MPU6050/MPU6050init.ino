#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

int buffersize=1000;    
int acel_deadzone=8;     
int giro_deadzone=1;     


MPU6050 accelgyro(0x68); 

int16_t ax, ay, az,gx, gy, gz;

int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz,state=0;
int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;
int16_t gx, gy, gz, ax, ay, az;
            


void setup() {
    Serial.begin(115200);
    Wire.begin();
    mpu.initialize();
    TWBR = 24;

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!");
        while (1); 
    }
    Serial.println("MPU6050 connected successfully");

    accelgyro.setXAccelOffset(0);
    accelgyro.setYAccelOffset(0);
    accelgyro.setZAccelOffset(0);
    accelgyro.setXGyroOffset(0);
    accelgyro.setYGyroOffset(0);
    accelgyro.setZGyroOffset(0);
}

void loop() {
  if (state==0){
    Serial.println("\nReading sensors for first time...");
    meansensors();
    state++;
    delay(1000);
  }

  if (state==1) {
    Serial.println("\nCalculating offsets...");
    calibration();
    state++;
    delay(1000);
  }

  if (state==2) {
    meansensors();
    Serial.println("\nFINISHED!");
    Serial.print("\nSensor readings with offsets:\t");
    Serial.print(mean_ax); 
    Serial.print("\t");
    Serial.print(mean_ay); 
    Serial.print("\t");
    Serial.print(mean_az); 
    Serial.print("\t");
    Serial.print(mean_gx); 
    Serial.print("\t");
    Serial.print(mean_gy); 
    Serial.print("\t");
    Serial.println(mean_gz);
    Serial.print("Your offsets:\t");
    Serial.print(ax_offset); 
    Serial.print("\t");
    Serial.print(ay_offset); 
    Serial.print("\t");
    Serial.print(az_offset); 
    Serial.print("\t");
    Serial.print(gx_offset); 
    Serial.print("\t");
    Serial.print(gy_offset); 
    Serial.print("\t");
    Serial.println(gz_offset); 
    Serial.println("\nData is printed as: acelX acelY acelZ giroX giroY giroZ");
    state++;
  }

  if (state==3) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("ax: "); Serial.print(ax);
    Serial.print("ay: "); Serial.print(ay);
    Serial.print("az: "); Serial.print(az);
    Serial.print("gx: "); Serial.print(gx);
    Serial.print("gy: "); Serial.print(gy);
    Serial.print("gz: "); Serial.print(gz);
    Serial.println("");
  }
}

void meansensors(){
  long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;

  while (i<(buffersize+101)){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    if (i>100 && i<=(buffersize+100)){ 
      buff_ax=buff_ax+ax;
      buff_ay=buff_ay+ay;
      buff_az=buff_az+az;
      buff_gx=buff_gx+gx;
      buff_gy=buff_gy+gy;
      buff_gz=buff_gz+gz;
    }
    if (i==(buffersize+100)){
      mean_ax=buff_ax/buffersize;
      mean_ay=buff_ay/buffersize;
      mean_az=buff_az/buffersize;
      mean_gx=buff_gx/buffersize;
      mean_gy=buff_gy/buffersize;
      mean_gz=buff_gz/buffersize;
    }
    i++;
    delay(2);
  }
}

void calibration(){
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

        
    }
}

