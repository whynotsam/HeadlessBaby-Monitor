//transciever libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//time related reference variables
unsigned long referenceTime = 0; //time reference declared
unsigned long seconds; //seconds variable
unsigned long pirResetRef; //stores PIR seconds threshhold for reseting
unsigned long soundLedDimTimeRef = 0; // milliseconds between each step of brightness
unsigned long soundLedOffTimer = 0; //stores sound LED seconds threshhold for reseting

//mic variables
int micPin = A3; // defines mic pin as analog pin A3
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

unsigned long lightTime = 0;
unsigned long startMillis;
unsigned int peakToPeak;
unsigned int signalMax;
unsigned int signalMin;
double volts;
//int micState; //set up a variable to hold info from mic

//Define tranceiver pins and pipes
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"10000", "20000"}; // 10000 = read, 20000 = write

// variables for motion sensor
int pirPin = 2; // Input from Motion Sensor
int pirDisable = 0; //PIR Sensor Disable
int motionDetected = 0; // Variable for Motion Detected
int pirValue; // Variable to store value from PIR

void setup() {
  Serial.begin(9600);

  // Initialize tranceiver for transmitting
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 20000
  radio.openReadingPipe(1, addresses[0]); // 10000
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  // Setup LEDs as I/O
  pinMode(pirPin, INPUT);// Setup PIR as Input

  // initialize variables
  referenceTime = millis();

  // Initial 1 Minute Delay to stabilize sensor
  Serial.println("hello");
  delay(60000);
  Serial.println("bye");
}

void loop() {
  //seconds
  if (millis() >= (referenceTime)) {
    referenceTime = referenceTime + 1000;  // use 100000 for uS
    seconds = seconds + 1;
  } // end 1 second
  
  // motion sensor code
  // Get value from motion sensor
  if (pirDisable == 0) {
    pirValue = digitalRead(pirPin); //detectedPin
  }

  // See if motion Detected
  if (pirValue == 1) {
    pirResetRef = seconds + 9;
    motionDetected = 1;
    pirValue = 0;
    radio.write(&motionDetected, sizeof(motionDetected));
    delay(5);
  }
  // pause and disable PIR sensor input after triggering to reset sensor
  if (motionDetected == 1) {
      pirDisable = 1; // disables PIR sensor
      radio.write(&motionDetected, sizeof(motionDetected));
      Serial.print(motionDetected);
      Serial.println(" Motion");
  }
  // 9 seconds elapsed, PIR sensor enabled and ready to detect again
  if (seconds == pirResetRef) { 
      motionDetected = 0;
      pirDisable = 0;
  }
  
//  // sound sensor code
//   startMillis = millis();  // Start of sample window
//   peakToPeak = 0;   // peak-to-peak level
//
//   signalMax = 0;
//   signalMin = 1024;
//
//   // collect data for 50 mS
//   while (millis() - startMillis < sampleWindow)
//   {
//      sample = analogRead(micPin);
//      if (sample < 1024)  // toss out spurious readings
//      {
//         if (sample > signalMax)
//         {
//            signalMax = sample;  // save just the max levels
//         }
//         else if (sample < signalMin)
//         {
//            signalMin = sample;  // save just the min levels
//         }
//      }
//   }
//   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
//   volts = (peakToPeak * 5.0) / 1024;  // convert to volts
//
//   if (volts > 1) {
//        micState = 1;
//        radio.write(&micState, sizeof(micState));
//        delay(5);
//        Serial.print(micState);
//        Serial.println(" sound");
//   }
//   Serial.print("volts:");
//   Serial.println(volts);
}
