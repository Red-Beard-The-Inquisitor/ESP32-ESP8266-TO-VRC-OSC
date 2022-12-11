// VRC OSC Bool toggle and float input for ESP8266
/// Only one analog input natively supported on ESP8266, can be expanded if you know how
//// tested and working on Wemos D1 Mini ESP8266 with https://arduino.esp8266.com/stable/package_esp8266com_index.json

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>  //https://github.com/stahlnow/OSCLib-for-ESP8266 // built for esp8266
#include <Keypad.h>      //https://playground.arduino.cc/Code/Keypad/  // easy to work with matrix keypad input


// If lastsentbool is set to 0, then fist invoke of that toggle will be "true" in VRC. 
// Each can be set diffrent if some of your avatar parameters start in "false" vs "true"
// will need one "int lastsentbool* = 0;" for each bool toggle added here

int lastsentbool1 = 0;
int lastsentbool2 = 0;
int lastsentbool3 = 0;
int lastsentbool4 = 0;

/////////////////////////////
// definitions necessary for analog to float

int analogvalraw1 = 0;
int analogval1 = 0;
int analogvallast1 = 0;
float mappedval1 = 0;
float floatval1 = 0;

int jitter = 3;  // Jitter value, sets how much any of the analog inputs must change before assuming a movement has occured
                 // Increase this value if you notice the float constantly changing with no user input
                 // This will vary based on stability of the analog signal and your boards ADC

// Parameter addresses found in C:\Users\User\AppData\LocalLow\VRChat\VRChat\OSC\usr_****\Avatars\
// Open the JOSN with the avatar ID you want control parameters of
// parameter toggles will also work on anyother avatar with a matching parameter address or addresses

char toggle1[] = "/avatar/parameters/Audio_Link";
char toggle2[] = "/avatar/parameters/*****";
char toggle3[] = "/avatar/parameters/*****";
char toggle4[] = "/avatar/parameters/*****";

char float1[] = "/avatar/parameters/*****";


////////////////////////////////////////////////////////////////////////////

int currentbool = 0;  // do not touch
int LED = 2;          // set to led pin to indicate connected status
int analoginput1 = A0; // defines your analog input pin

////////////////////////////////////////////////////////////////////////////

char ssid[] = "*****";  // your network SSID (name)
char pass[] = "*****";       // your network password


WiFiUDP Udp;                             // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, *, *);  // remote IP of your computer, ip address of computer running VRC
const unsigned int outPort = 9000;       // remote port to receive OSC, 9000 is the default for VRC
const unsigned int localPort = 2390;     // local port to listen for OSC packets (not used/tested), not used just leave as is

unsigned long previousMillis = 0; // timer things
int interval = 0; // timer things

///////////////////////////////////////////////////////////////////////////

// Keypad setup, see https://playground.arduino.cc/Code/Keypad/ for more instructions

const byte ROWS = 1;  //one rows
const byte COLS = 4;  //four columns

char keys[ROWS][COLS] = {
  { '1', '2', '3', '4' },

};

// tested, working pin assignments for 4 buttons on ESP8266, add/edit as needed

byte rowPins[ROWS] = { 0 };              //connect to the row pinouts of the keypad
byte colPins[COLS] = { 4, 12, 13, 15 };  //connect to the column pinouts of the keypad


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

///////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  pinMode(LED, OUTPUT);
  Serial.println();
  digitalWrite(LED, HIGH);  // sets onboard led to "off"
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  digitalWrite(LED, LOW);  // Sets led to "on" once wifi connected and UDP is started
}

void loop() {  

  char key = keypad.getKey();  // Checks if key is pressed


  // Cases are added for each defined key
  // Each executes a "void sendparam*"
  // to add a case, copy and paste "case" through "break;"
  // increment "case '*'" to match an added key
  // increment the "lastsentbool*" and "sendparam*"
  // bonus, you can execute multible "sendparam*" with one key, just add another defined "sendparam*();"
  // it will execute both with one button press



  if ((WiFi.status() == WL_CONNECTED) && key) {  // Only do something with the keypress if connected to WiFi
    Serial.println(key);                         // prints current pressed key to serial for debug
    switch (key) {
      case '1':                                 // if key "1" is pressed
        if ((lastsentbool1) != 1) {             // checks if the last sent bool for this parameter was true, if not
          currentbool = 1;                      // sets the "to be sent bool" to "true"
          sendparam1();                         // executes the void that sends the parameter
          Serial.print("sendparameter1true");   // for debug, can be omitted
        } else {                                // if the last sent bool was true
          currentbool = 0;                      // sets the "to be sent bool" to "false"
          sendparam1();                         // executes the void that sends the parameter
          Serial.print("sendparameter1false");  // for debug, can be omitted
        }
        break;  // exits the switch case, returns to begining and waits for a keypress
      case '2':
        if ((lastsentbool2) != 1) {
          currentbool = 1;
          sendparam2();
          Serial.print("sendparameter2true");
        } else {
          currentbool = 0;
          sendparam2();
          Serial.print("sendparameter2false");
        }
        break;
      case '3':
        if ((lastsentbool3) != 1) {
          currentbool = 1;
          sendparam3();
          Serial.print("sendparameter3true");
        } else {
          currentbool = 0;
          sendparam3();
          Serial.print("sendparameter3false");
        }
        break;
      case '4':
        if ((lastsentbool4) != 1) {
          currentbool = 1;
          sendparam4();
          Serial.print("sendparameter4true");
        } else {
          currentbool = 0;
          sendparam4();
          Serial.print("sendparameter4false");
        }
        break;
    }

    delay(100);
    interval = 500;       // Sets extra time to wait after keypress before reading analog inputs
                          // helps avoid jitter from board voltage fluctuation during led blink

///////////////////////////////////////////////////////////////////////
// analog input to float section

  }
  unsigned long currentMillis = millis(); // timer things

  if (currentMillis - previousMillis > interval) {                     // checks if enough time has passed since last run, if it has
    previousMillis = currentMillis;                                   // marks time of when this section ran
    interval = 30;                                                    // Sets how often analog inputs are read, avoids reading every cycle as it's not needed.
    analogvalraw1 = analogRead(analoginput1);             // reads the analog values
    analogval1 = constrain(analogvalraw1,20,1010);        // constrains output. to ensure full range is output later, Basiclly adds a dead zone at each extreme

    if (((analogval1) > ((analogvallast1) + (jitter))) || ((analogval1) < ((analogvallast1) - (jitter)))) { // Compares current analog vlaue to last analog value read, if it has changed more than +/- "jitter"
      analogvallast1 = (analogval1);                                                                        // updates current analog value to be used
      mappedval1 = map(analogvallast1, 20, 1010, 0, 100);                                                   // maps analog value to a 0 - 100 value
      floatval1 = (mappedval1) / 100;                                                                       // divides mapped value to make it a 0.00 - 1.00 float
      sendparam5();                                                                                         // executes the void to send the new float value
      Serial.print("sendparameter5float ");                                                                 // debug
      Serial.println(floatval1);                                                                            // debug, prints the sent float value to serial
    }
  }
}

////////////////////////////////////////////////////

// Each void repesents a toggle, will need to add one per individual toggle you want to control
// to add, copy and paste a void and increment the last number in "sendparam*", "toggle*" and "lastsentbool*"
// make sure you have a matching "int lastsentbool*" and "char toggle*[]" setup as well

void sendparam1() {
  OSCMessage msg(toggle1);
  msg.add(currentbool);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  lastsentbool1 = (currentbool);
  ledblink();
}
void sendparam2() {
  OSCMessage msg(toggle2);
  msg.add(currentbool);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  lastsentbool2 = (currentbool);
  ledblink();
}
void sendparam3() {
  OSCMessage msg(toggle3);
  msg.add(currentbool);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  lastsentbool3 = (currentbool);
  ledblink();
}
void sendparam4() {
  OSCMessage msg(toggle4);
  msg.add(currentbool);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  lastsentbool4 = (currentbool);
  ledblink();
}

//////////////////////////////////////////////////////////////
// Void for sending a float value

void sendparam5() {
  OSCMessage msg(float1);
  msg.add(floatval1);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();  
}

//////////////////////////////////////////

/// Blinks status led when bool parameter is sent

void ledblink() {
  digitalWrite(LED, HIGH);
  delay(50);
  digitalWrite(LED, LOW);
}
