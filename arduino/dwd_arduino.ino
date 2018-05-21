//
// This is the script for new version of arduino yun
//
#include <Bridge.h>
#include <HttpClient.h>
#include <TimedAction.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// variables for electronics
static const int trigPin = 13; // pin of the trig end of the proximity sensor
static const int echoPin = 11; // pin of the echo end of the proximity sensor

static const int diffuserButtonPin = 7; // pin of the button of the diffuser. power is always on

static const int LEDPIN = 9; // pin of the LED ring
static const int NUMPIXELS = 16; // number of LED controlled by Arduino
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

// variables for sensor detection and time
int proximitySensorValue; // in cm
boolean currentProximity; // inside the range or not
boolean previousProximity;

boolean isOn;
unsigned long startTime;
unsigned long elapsedTime; // the output we want

//variables for sending data
int sendData;
String APIkey = "SXCIFQ6TNUJE53QG";      // Use your own API WRITE key here

void switchDiffuserState(boolean isOn) {
  if (isOn) { // turn diffuser on
    Serial.println("diffuser is on!");
    digitalWrite(diffuserButtonPin, HIGH);
    delay(1000);
    digitalWrite(diffuserButtonPin, LOW);
    delay(1000);
    digitalWrite(diffuserButtonPin, HIGH);
    delay(1000);
    digitalWrite(diffuserButtonPin, LOW);
  } else {
    Serial.println("diffuser is off!");
    digitalWrite(diffuserButtonPin, HIGH);
    delay(1000);
    digitalWrite(diffuserButtonPin, LOW);
    delay(1000);
    digitalWrite(diffuserButtonPin, HIGH);
    delay(1000);
    digitalWrite(diffuserButtonPin, LOW);
  }
}

// helper function for sending data to ThingSpeak
void sendDataToThingSpeakHelper(String data) {
  HttpClient client;
  String clientStr;

  Serial.println("connecting to ThingSpeak ...");

  // Sometimes the DNS lookup doesn't work so it's best to use ThingSpeak's
  // static IP address.
  clientStr = "http://184.106.153.149/update?api_key=" + APIkey + "&field1=";
  clientStr += data;

  Serial.println(clientStr);

  // The next line sends the data to ThingSpeak
  client.get(clientStr);
  Serial.println("...done");

  // Read the entry number returned by ThingSpeak
  String result;
  while (client.available()) {
    char c = client.read();
    result += String(c);
  }

  if (result.toInt() == 0) {
    Serial.println("Failed to send to ThingSpeak");
  }
  else {
    // Success - display the entry number returned by ThingSpeak
    Serial.println("Created entry " + result + ", data: " + data);
  }
}

// send data to ThingSpeak
void sendDataToThingSpeak () { // send data every 15 seconds
  if (sendData != 0) { // there is data
    sendDataToThingSpeakHelper(String(sendData)); // send data
    sendData = 0; // reset the data
  }
  // else do nothing
}
//this initializes a TimedAction class that will send data to ThinSpeak every 1 minuates.
TimedAction sendDataAction = TimedAction(60000, sendDataToThingSpeak);


// control LED
void lightupLED (int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i+=8) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}


// proximity sensor
int proximityRead (int proximityTrigPin, int proximityEchoPin) { // raw value
  // defines variables
  long duration;
  int distance;

  // Clears the trigPin
  digitalWrite(proximityTrigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(proximityTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(proximityTrigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(proximityEchoPin, HIGH);
  // Serial.println("duration: "+ String(duration));
  // Calculating the distance
  distance = duration * 0.034 / 2;

  if (distance <= 350) { // inside detection range, 350 cm
    return distance;
  } else { // out of detaction range
    return -1; // unstable value
  }
}

boolean isInsideRange(int currentNormalValue, int range) {
  if ((currentNormalValue > 0) && (currentNormalValue < range)) {
    return true;
  } else {
    return false;
  }
}


void setup() {
  Serial.begin(9600);

  pinMode(diffuserButtonPin, OUTPUT);
  digitalWrite(diffuserButtonPin, LOW);
  digitalWrite(diffuserButtonPin, LOW);
  digitalWrite(diffuserButtonPin, LOW); // the initial state of the buton of the diffuser is off

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  currentProximity = false;
  previousProximity = false;

  isOn = false;
  sendData = 0;

  // LED
  pixels.begin(); // initializes the NeoPixel library

  // Wait for Linux to boot
  Serial.println("Waiting 20 seconds for Linux to boot ...");
  delay(20000);

  // Bridge startup
  Bridge.begin();
}

void loop() {
  sendDataAction.check();
  proximitySensorValue = proximityRead(trigPin, echoPin);
  if (isInsideRange(proximitySensorValue, 30)) {
    currentProximity = true;
  } else {
    currentProximity = false;
  }

  if (currentProximity == true && previousProximity == true) {
    // person is detected in the range
    Serial.println("inside the range");
    // light up the LED
    lightupLED(255, 255, 255);
    // diffuser turns on if it is off
    if (isOn == false) { // the diffuser is off
      // turn the diffuser on
      isOn = true;
      switchDiffuserState(isOn);
      // start the timer
      startTime = millis();
      Serial.println("turn the deffuser on, and strat to record the time");
    } else { // already on, do nothing
      Serial.println("continue diffusing");
    }

  } else {
    // nobody is inside in the range
    Serial.println("outside the range");
    // close the LED
    lightupLED(0, 0, 0);
    // diffuser turns off if it is on
    if (isOn == true) {
      // turn the diffuser off
      isOn = false;
      switchDiffuserState(isOn);
      // end the timer
      elapsedTime = millis() - startTime;
      // accumulate the escaped time
      sendData += elapsedTime;
      String outputStr = "turn the deffuser off, the interval is:";
      outputStr += elapsedTime;
      Serial.println(outputStr);
    } else { // already off, do nothing
      Serial.println("remain closed");
    }
  }
  previousProximity = currentProximity;
  // no delay, the power supply of the LEDs conflict with power of the diffuser
}
