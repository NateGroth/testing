#include <Wire.h>

int asPin = 1; 
double asOffsetV = 0.0; // variable for V offset (autozero)

void setup() {
Serial.begin(9600);
asOffsetV = analogRead(asPin) * .0047; // Zero speed sensor reading * 4.7 mV per count
}

void loop() {
double asVolts = 0.0;
double compVOut = 0.0;
double dynPress = 0.0;
double airSpeed = 0.0;

asVolts = analogRead(asPin) * .0047;

Serial.print("Raw reading (V): ");
Serial.println(asVolts);

Serial.print("Offset (V): ");
Serial.println(asOffsetV);

compVOut = asVolts - asOffsetV;

Serial.print("Reading - Offset (V): ");
Serial.println(compVOut);

//dynPress = (compVOut / 5.0 - .2) / .2; // Transfer function with no autozero
if(compVOut < .005) { // Set noise to 0, min speed is ~8mph
compVOut = 0.0;
}
dynPress = compVOut * 1000.0; // With autozero, dynamic pressure in kPa = Vout, convert kPa to P

Serial.print("Dynamic pressure (Pa): ");
Serial.println(dynPress);

airSpeed = sqrt((2 * dynPress)/1.225); // Converts pressure to m/s, 1.225 k/m3 is standard air density

Serial.print("Speed (m/S): ");
Serial.println(airSpeed);
// Serial.print("*********************Speed (mph): ");
// Serial.println(airSpeed * 2.236); // m/s to mph

Serial.println();

delay(1000);
}