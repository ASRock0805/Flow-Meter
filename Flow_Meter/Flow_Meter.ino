/*
  Compact Electromagnetic Flow Sensor (Aichi Tokei VN05)
      Use interrupts to calculate the number of unit pulses per interval to convert the water flow rate (mL/minute).


  Created 6 Oct. 2020
  by Yi-Xuan Wang

  References:
  https://www.aichitokei.net/products/compact-electromagnetic-flow-sensor-vn/
*/

/*--- Preprocessor ---*/
#define flowPin 2 // The pin location of the sensor (D2) w/ interrupt (INT.0)

/*--- Constant ---*/
const unsigned long baudSpeed = 115200UL; // Sets the data rate in bits per second (baud) for serial data transmission
const unsigned long period = 1000UL;      // The value is a number of milliseconds

/*--- Global Variables ---*/
unsigned long startTime;            // Start time
unsigned long currentTime;          // Current time
unsigned long timer;                // Stopwatch

volatile unsigned long pulseCount;  // Measuring the falling edges of the signal
static unsigned long cumCount;      // Cumulative count
float flowRate;                     // Value of water flow rate
float flowML;                       // Unit converter (milliliter, mL)
float totalML;                      // Volume of cumulative water

/*--- Function Prototype ---*/
void getCounter(void);
void setup(void);
void loop(void);

/*--- Functions Definition ---*/
// Interrupt Service Routine (ISR) for Flow Sensor
void getCounter(void) {         
  pulseCount = pulseCount + 1;  // Every falling edge of the sensor signals to increment the pulse count
}

/*--- Initialization ---*/
void setup(void) {
  Serial.begin(baudSpeed);        // Initializes serial port
  pinMode(flowPin, INPUT_PULLUP); // Initializes interrupt digital pin 2 declared as an input and pull-up resitor enabled
  startTime = millis();           // Initial start time

 // Flow Sensor Initialization
  pulseCount = 0;
  cumCount = 0;
  flowRate = 0.0f;
  flowML = 0.0f;
  attachInterrupt(digitalPinToInterrupt(flowPin), getCounter, FALLING); // The interrupt is attached
}

/*--- Measurement ---*/
void loop(void) {
  // Every second, calculate and print the measured value
  currentTime = millis();                     // Get the current "time"

  if ((currentTime - startTime) >= period) {  // Test whether the period has elapsed
    timer = startTime / period;               // Calculate the period of time

    // Flow Sensor
    detachInterrupt(digitalPinToInterrupt(flowPin));  // Clears the function used to attend a specific interrupt
    cumCount = cumCount + pulseCount;                 // Count increment
    // Estimated Volume: 0.5004 ml/Pulse
    flowRate = (float)abs(((-7.0f * pow(10.0, -18.0)) * sq(pulseCount)) + (0.5004f * pulseCount) - (8.0f * pow(10.0, -12.0)));
    flowML = flowRate * 60.0f;                         // Milliliter per pulse converter to milliliter per minute

    if (isinf(flowML) || isnan(flowML) || (flowML <= 0.0f)) {
      flowML = -1;
    }

    /*--- Sensor prompt ---*/
    Serial.print("Cumulative Count: ");
    Serial.print(cumCount);
    Serial.print(", Pulse Count: ");
    Serial.print(pulseCount);
    Serial.print("Current Flow Rate: ");
    Serial.print(flowML);
    Serial.print(" mL/min., ");

    Serial.print(timer);
    Serial.println(" sec.");

    /*--- System Return ---*/
    startTime = currentTime;                                              // Save the start time of the current state
    pulseCount = 0;                                                       // Set pulseCount to 0 ready for calculations
    attachInterrupt(digitalPinToInterrupt(flowPin), getCounter, FALLING); // Reattach interrupt
  } else {
    return;
  }
}
