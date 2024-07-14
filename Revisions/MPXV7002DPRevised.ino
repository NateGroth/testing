// Analog pin connections
const int pressurePin = A0;  // Analog pin connected to MPXV7002DP output
const int voltagePin = A1;   // Analog pin connected to Arduino's 3.3V output

// Constants
const float nominalVoltage = 3.3;       // Nominal 3.3V output voltage
const float voltageScaleFactor = 5.0 / 1023.0; // Scale factor for analog to voltage conversion
const float pressureScaleFactor = 1.0;  // Scale factor for pressure (adjust based on testing)

// Buffer for storing recent pressure readings
const int bufferSize = 10;
float pressureBuffer[bufferSize];
int bufferIndex = 0;

// Last voltage reading for spike detection
float lastVoltage = nominalVoltage;

// Threshold for detecting voltage fluctuations (adjust based on sensitivity)
float voltageThreshold = 0.05; // 50 mV threshold

void setup() {
    Serial.begin(9600);
    Serial.println("MPXV7002DP Pressure Sensor Monitoring with Voltage Compensation Started");
}

// Function to add a pressure reading to the buffer
void addToPressureBuffer(float pressure) {
    pressureBuffer[bufferIndex] = pressure;
    bufferIndex = (bufferIndex + 1) % bufferSize;
}

// Function to clear the pressure buffer
void clearPressureBuffer() {
    for (int i = 0; i < bufferSize; i++) {
        pressureBuffer[i] = 0;
    }
    Serial.println("Voltage fluctuation detected! Clearing pressure buffer.");
}

// Function to apply a linear offset to buffer data based on voltage deviation
void applyVoltageCompensation(float currentVoltage) {
    float voltageDeviation = currentVoltage - nominalVoltage;
    float compensationFactor = voltageDeviation * pressureScaleFactor;

    for (int i = 0; i < bufferSize; i++) {
        pressureBuffer[i] += compensationFactor;
    }
}

void loop() {
    // Read the current voltage
    float currentVoltage = analogRead(voltagePin) * voltageScaleFactor;

    // Detect significant voltage fluctuations
    if (abs(currentVoltage - lastVoltage) > voltageThreshold) {
        clearPressureBuffer();
    } else {
        // Apply voltage compensation if there's a small deviation
        applyVoltageCompensation(currentVoltage);
    }
    lastVoltage = currentVoltage;

    // Read the pressure from MPXV7002DP
    int rawPressure = analogRead(pressurePin);
    float pressure = (rawPressure * voltageScaleFactor - nominalVoltage) * pressureScaleFactor;

    // Add the current pressure reading to the buffer
    addToPressureBuffer(pressure);

    // Print the current voltage, pressure, and buffer contents
    Serial.print("Voltage (V): ");
    Serial.print(currentVoltage, 3);
    Serial.print(" | Raw Pressure Reading: ");
    Serial.print(rawPressure);
    Serial.print(" | Compensated Pressure (kPa): ");
    Serial.println(pressure, 3);

    delay(1000); // Delay of 1 second between readings
}