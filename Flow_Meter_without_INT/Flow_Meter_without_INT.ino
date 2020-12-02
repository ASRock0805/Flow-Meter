/*
  - Compact Electromagnetic Flow Sensor (Aichi Tokei VN05)
      Use ADC to calculate the number of unit pulses per interval to convert the water flow rate (mL/minute).

  Created 6 Oct. 2020
  by Yi-Xuan Wang

  References:
  https://www.aichitokei.net/products/compact-electromagnetic-flow-sensor-vn/
*/

/*--- Preprocessor ---*/
#define flowPin A0  // The pin location of the sensor (ADC)

/*--- Constants ---*/
const unsigned long baudSpeed = 115200; // Sets the data rate in bits per second (baud) for serial data transmission

const float vIn = 5.0;                                  // Supply voltage from Arduino
const int resBits = 10;                                 // Resolution of ADC (10 bits)
const float vConv = (vIn / (pow(2.0, resBits) - 1.0));  // Voltage of ADC level

/*--- Global Variables ---*/
float vOut = 0.0;
volatile unsigned long pulse = 0; // Measuring the sensor of the signal
byte lastPulseState = digitalRead(flowPin);

/*--- Initialization ---*/
void setup(void) {
  Serial.begin(115200);           // Sets serial port baud to 115200 bps
  pinMode(flowPin, INPUT_PULLUP); // initializes interrupt D2 declared as an input and pull up resitor is enabled
}

/*--- Operating ---*/
void loop(void) {
  vOut = analogRead(flowPin) * vConv;
  byte pulseState = digitalRead(flowPin);

  if (pulseState && !lastPulseState) {
    pulse = pulse + 1;
  }
  lastPulseState = pulseState; // Save the current state as the last state, for next time through the loop

  /*--- Sensor prompt ---*/
  Serial.print("Pulse: ");     // Pulse count of flow sensor
  Serial.print(pulse);
  Serial.print(", Voltage: "); // Voltage of flow sensor
  Serial.print(vOut, 2);
  Serial.println(" V");

  delay(1);
}
