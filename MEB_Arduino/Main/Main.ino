#include <Servo.h>
#include "pinmap.h"
Servo myServo;
unsigned long start;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds
int lastSecond = -1;
int seconds;
const int KILL = 3; // Killswitch input
int incomingByte = 0;
int buttonState = 0; 
int trackState;
int depthPin = A0;
int voltPin = A1;
boolean hasRun = false;
double timeUsed;
int depthVal;
int voltageVal;
int batteryVal;
int batteryPin = A1;
int servoPin = 9;
int killPin = 12; 
int thrustSpeed = 1500;
int servoSpeed;
int servoDir;
int thrustersOn = 0;
int pwmPins[ThrustPins] = {3,5,6,11};
int dirPins[ThrustPins] = {2,4,7,8};
boolean killed = false;
boolean trackCount = false;

enum class States {
    WaitFor7E, WaitForFF,Read, Write, Address
};

States currentState = States::WaitFor7E;

void setup() {
    Serial.begin(9600);
    myServo.attach(servoPin); //attach servo object to specific pin
    myServo.writeMicroseconds(1500);
    pinMode(ThrustDir, OUTPUT);
    pinMode(depthPin, INPUT);
    pinMode(KILL, INPUT); 
   /*iterates through each thrust pin and sets its pin mode 
    *(there are 8 so this is more efficient than writing it all out)
    */
    for(int i = 0; i < ThrustPins; i++) {
       pinMode(pwmPins[i], OUTPUT);
       pinMode(dirPins[i], OUTPUT);
    }
   
 /* digitalWrite(DIR0, HIGH); //delay(1500); analogWrite(PWM0, current0);*/ 
};
/* checks if current state has been written to serial */
void Track(){
      if (trackCount == false){
         trackingState();
      }
}

/* tracks and wrties state to serial */ 
void trackingState(){
    switch (trackState){
      case 1: Serial.println("current state: WaitFor7E");
      hasRun = true;
      break;
      case 2: Serial.println("current state: WaitForFF");
      hasRun = true;
      break;
      case 3: Serial.println("current state: Read");
      hasRun = true;
      break;
      case 4: Serial.println("current state: Write");
      hasRun = true;
      break;
      case 5: Serial.println("current state: Address");
      hasRun = true;
      break;
      case 6: Serial.println(" (depthVal)");
      //Serial.write(depthVal);
      hasRun = true;
      break;
      case 7: Serial.println("reading voltVal");
      hasRun = true;
      break;
    }
};
/* iterate through thrust pins and set them to 0 */
void resetAll() {
    for(int i = 0; i < ThrustPins; i++) {
    analogWrite(pwmPins[i], 0);
    }
};
void WaitForFF(){
    //Serial.print("waiting for FF");
    //myservo.writeMicroseconds(1600);
};

void loop() {
    // send data only when you receive data:
    if (Serial.available() > 0) {
       // read the incoming byte:
       incomingByte = Serial.read();
       // say what you got:
       //Serial.print("I received: ");
       // Serial.println(incomingByte, DEC);
    }
    currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
    if (currentMillis - start >= period){  //test whether the period has elapsed
        depthVal = analogRead(depthPin);
        Serial.println(depthVal);
        start = currentMillis;
    }
    
    killed = digitalRead(killPin); //the kill pin only receives data if the system needs to be killed so it we kill the motors for any input from killPin
    if (killed) {
      resetAll();
    }
    /*if (incomingByte == 'C'0x7E){
      Serial.print("c is received");
      currentState = States:: WaitForFF;
      thrustersOn = 0;
    }*/
        switch(currentState){
        case States::WaitFor7E:
            if(hasRun == false){
                trackState = 1;
                Track();
            }
            if(incomingByte == 'X'){
                currentState = States:: WaitForFF;
                hasRun = false;
            }
           
            break;
        case States::WaitForFF:
            if(hasRun == false){
                trackState = 2;
                Track();
            }
            switch(incomingByte){
                case 'A'/*0x10*/ : currentState = States:: Read; hasRun = false;
                break;
                case 'B' /*0x11*/: currentState = States:: Write; hasRun = false;
                break;
            }
            break;
        case States:: Read:
            if(hasRun == false){
                trackState = 3;
                Track();
            }
            if(incomingByte == 'C'){ 
                hasRun = false; 
                currentState = States:: Address;
            }
            break;
        case States:: Address: 
            if(hasRun == false){
                trackState = 5;
                Track();
            } 
            if(incomingByte == 'T'/*0x20*/){
                depthVal = analogRead(depthPin);
                Serial.print(depthVal);
                hasRun = false;
                if(hasRun == false){
                    trackState = 6;
                    Track();
                } 
                currentState = States:: WaitFor7E;
            } //does voltage have a specific bit?
            else if (incomingByte == 'Y'/*0x21*/){
                voltageVal = analogRead(voltPin);
                hasRun = false;
                if(hasRun == false){
                     trackState = 7;
                     Track();
                }
                currentState = States:: WaitFor7E;
             }
             else if (incomingByte == 'U'/*0x22*/){
                 batteryVal = analogRead(batteryPin);
             }
             else if (incomingByte == 'I'/*0x30*/){
                 digitalRead(killPin);
             }
            break;
        case States:: Write:
            if(hasRun == false){
                trackState = 4;
                Track();
            }
            currentState = States:: WaitFor7E;
            break;
       
       /* switch(incomingByte){
          case 0x80 : resetAll();
                break;
            case 0x10 : currentState = States:: SetThrusters;
                break;
            case 0x11 : currentState = States:: SetServoDir;
                break;
            case 0x12 : currentState = States:: SetThrusterDir;
                break;
            case 0x20 :
                depthVal = analogRead(depthPin);//
                Serial.write(depthVal >> 8);//what is the point of this (does it make it more efficient?
                Serial.write(depthVal & 0xFF);//also what is this doing?
                break;
            case 0x30 : digitalWrite(killPin, 1);//what does this value mean? does it matter what it is?
                break;
        }
        
    case States::SetThrusters:
       trackState = 5;
        switch(incomingByte){
          //case 0x00 : read depth sensor
          //case 0x01 : read kill switch
        }
        break;
    case States::SetThrusterDir:
        if(thrustersOn < ThrustPins ) {
              if(incomingByte == 0) {
                digitalWrite(dirPins[thrustersOn], LOW);
              } else {
                digitalWrite(dirPins[thrustersOn], HIGH);
              }
              thrustersOn++;
              
              if(thrustersOn == 4) {
                currentState = States::WaitFor7E;
                thrustersOn = 0;
              }
        } else {
          currentState = States::WaitFor7E;
          thrustersOn = 0;
        }
        break;
    case States::SetServoDir:
        servoDir = incomingByte;
        States::SetServoSpeed;    
        break;
    case States:: SetServoSpeed:
        servoSpeed = incomingByte;
        if(!killed){ //if no input from killpin, continue setting thrusters. problem is that you continue functioning until you receive a kill signal from the killpin 
                   //so if you dont receive that signal you will keep setting motors even after you lose communication with the system.
            if(servoDir == 0) {
              myServo.writeMicroseconds((int) (1500 - (400.0 / 255.0) * servoSpeed));
            } else {
                  myServo.writeMicroseconds((int) (1500 + (400.0 / 255.0) * servoSpeed));
            }
        } else {  
              myServo.writeMicroseconds(1500);
        }   
        break;
    case States::On:
        currentState = States::WaitFor7E;  
    }*/
}
}
  
