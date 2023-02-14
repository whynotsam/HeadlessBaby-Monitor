#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

unsigned long referenceTime = 0; //time reference declared
unsigned long seconds; //seconds variable
unsigned long stateLEDResetTimeRef;
unsigned long soundLedOffTimer = 0; //

//Define tranceiver pins and pipes
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"10000", "20000"};

// Define pins for LEDs
int greenLED = 5;
int redLED = 4;
int soundLed = 15; // LED pin for indication of sound detection

// Motion detection variables
int motionDetected = 0;

// variables for sound sensor (mic and LED)
//int brightness = 0;    // how bright the LED is (255 max, 254 is used because it's evenly divisible by 2, the fadeAmount)
//int fadeAmount = 2;    // how many steps to fade the LED by
int micState = 0; //set up a variable to hold info from D0 (HIGH or LOW)
//int soundSensorDetection = 0; // 0 if nothing, 1 if detection
//int fadeState = 0; // 0 is no fade, 1 is start fade 

void setup() {
  Serial.begin(9600);

  // Initialize tranceiver for recveiving
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 10000
  radio.openReadingPipe(1, addresses[1]); // 20000
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  // Setup LEDs as I/O
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(soundLed, OUTPUT);

  // initialize variables and pins
  referenceTime = millis();
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW); 
  digitalWrite(soundLed, LOW);
}

void loop() {

  //seconds
  if (millis() >= (referenceTime)) {
    referenceTime = referenceTime + 1000;  // use 100000 for uS
    seconds = seconds + 1;
  } // end 1 second

  // motion sensor code
  radio.read(&motionDetected, sizeof(motionDetected));
  delay(5);
  
  if (motionDetected == 1) {
    stateLEDResetTimeRef = seconds + 5; //add 5 seconds to LED reset timer
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, HIGH);
    digitalWrite(soundLed,HIGH);
    Serial.println("Motion On");
    Serial.print(micState);
    Serial.println(" micState");
  }

  if (seconds > stateLEDResetTimeRef) {
    motionDetected = 0;
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
//    Serial.println("Motion Off");
//    Serial.print(micState);
//    Serial.println(" micState");
  }
  
  // sound sensor code
  delay(5);
  radio.read(&micState, sizeof(micState));
  
//  if(micState == 1) { // if the mic sensor is triggerd, turn on the LED
//    soundLedOffTimer = seconds + 3;
//    digitalWrite(soundLed,HIGH);
//    Serial.println("Sound On");
//  }
  if (seconds > soundLedOffTimer) {
    micState = 0;
    digitalWrite(soundLed,LOW);
//    Serial.println("Sound Off");
  }
}
