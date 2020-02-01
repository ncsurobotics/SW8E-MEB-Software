#include <Servo.h>
#include "pinmap.h"
Servo myServo; // create servo object to control a servo
const int KILL = 3; // Killswitch input
int incomingByte = 0;
int buttonState = 0; 
int trackState;
int depthPin = A0;
int voltPin = A1;
boolean hasRun = false;
int depthVal;
int voltageVal;
int batteryVal;
int batteryPin = A1; /////////////////////////////////////////where is battery pin?
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
    WaitFor7E, WaitForFF,Read, Write, Address/*SetThrusters, SetThrusterDir, SetServoDir, SetServoSpeed,On*/
};

  //i think nthrusters is just another variable to keep track of thrusters/ setting thruster speeds or somethine. or its not right
States currentState = States::WaitFor7E;
void setup() {
    Serial.begin(9600);
    myServo.attach(servoPin);     //attach servo object to specific pin
    myServo.writeMicroseconds(1500);
    pinMode(ThrustDir, OUTPUT);
    pinMode(depthPin, INPUT);
    pinMode(KILL, INPUT);  
    for(int i = 0; i < ThrustPins; i++) {//iterates through each thrust pin and sets its pin mode (there are 8 so this is more efficient than writing it all out)
       pinMode(pwmPins[i], OUTPUT);
       pinMode(dirPins[i], OUTPUT);
    }
 /* digitalWrite(DIR0, HIGH); //delay(1500); analogWrite(PWM0, current0);*/ 
};
void Track(){
      if (trackCount == false){
         trackingState();
        }
  }//added hasRun following each case instead of just at end. This should fix the printing of case 6 4 times (need to test w/ arduino in)
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
      case 6: Serial.println("reading depthVal");
      hasRun = true;
      break;
      case 7: Serial.println("reading voltVal");
      hasRun = true;
      break;
    }
};


void resetAll() {
    for(int i = 0; i < ThrustPins; i++) {//iterate through thrust pins and set them to 0
    analogWrite(pwmPins[i], 0);
    }
};
void WaitForFF(){
    //Serial.print("waiting for FF");
    //myservo.writeMicroseconds(1600);
};
/*
void declareState(){
        hasRun = true;
        Serial.println("State == correctbojangle");
        return hasRun;
};*/
void loop() {
    // send data only when you receive data:
    if (Serial.available() > 0) {
       // read the incoming byte:
       incomingByte = Serial.read();
       // say what you got:
       //Serial.print("I received: ");
      // Serial.println(incomingByte, DEC);
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
        currentState = States:: WaitForFF;
           if(hasRun == false){
              trackState = 1;
              Track();
           }
        hasRun = false;
        break;
    case States::WaitForFF:
          if(hasRun == false){
              trackState = 2;
              Track();
           }
//going from 0x10 to 0x11 (A to B) does not trigger trackState because only A is recycling it right now. 
//I don't know where B is going so I havn't put a hasRun=false anywhere yet
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
           //why is this running 4 times?
           if(incomingByte == 'T'/*0x20*/){
               depthVal = analogRead(depthPin);
               hasRun = false;
               if(hasRun == false){
                   trackState = 6;
                   Track();
               } 
               currentState = States:: WaitFor7E;
           } //does voltage have a specific bit?
           else if (incomingByte == 'Y'/*0x21*/){
               voltageVal = analogRead(voltPin);//is voltage the same as battery? And should it have a different input value?
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
  
