#include <Servo.h>
#include "pinmap.h"
Servo myServo;
unsigned long start;  
unsigned long currentMillis;
const unsigned long period = 100;  //the value is a number of milliseconds
int lastSecond = -1;
int seconds;
int incomingByte = 0;
int buttonState = 0; 
int trackState;
int depthPin = A0;
int voltPin = A1;
int depthAve = 0;
boolean hasRun = false;
double timeUsed;
double depthVal;
double depthSum;
int count = 0;
int killVal;
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

/*enum class States {
    WAIT_FOR_7E, WAIT_FOR_FF,READ, WRITE, Address
};*/
enum States {
    WAIT_FOR_7E,
    WAIT_FOR_FF,
    READ,
    WRITE,
    ADDRESS
  };

States currentState = WAIT_FOR_7E;

void setup() {
    Serial.begin(9600);
    myServo.attach(servoPin); //attach servo object to specific pin
    myServo.writeMicroseconds(1500);
    pinMode(ThrustDir, OUTPUT);
    pinMode(depthPin, INPUT);
    pinMode(killPin, INPUT); 
   /*iterates through each thrust pin and sets its pin mode 
    *(there are 8 so this is more efficient than writing it all out)
    */
    for(int i = 0; i < ThrustPins; i++) {
       pinMode(pwmPins[i], OUTPUT);
       pinMode(dirPins[i], OUTPUT);
    }
   
 /* digitalWRITE(DIR0, HIGH); //delay(1500); analogWRITE(PWM0, current0);*/ 
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
      case 1: Serial.println("current state: WAIT_FOR_7E");
      hasRun = true;
      break;
      case 2: Serial.println("current state: WAIT_FOR_FF");
      hasRun = true;
      break;
      case 3: Serial.println("current state: READ");
      hasRun = true;
      break;
      case 4: Serial.println("current state: WRITE");
      hasRun = true;
      break;
      case 5: Serial.println("current state: Address");
      hasRun = true;
      break;
      case 6: Serial.println(" (depthVal)");
      //Serial.write(depthVal);
      hasRun = true;
      break;
      case 7: Serial.println("READing voltVal");
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
//void WaitForFF(){
    //Serial.print("waiting for FF");
    //myservo.writeMicroseconds(1600);
//};
//depth values 0-1023
void takingAverage() {
     depthSum += analogRead(depthPin);
     count++;
     //Serial.println(depthSum);
}

void loop() {
    // send data only when you receive data:
    if (Serial.available() > 0) {
       // READ the incoming byte:
       incomingByte = Serial.read();
       // say what you got:
       //Serial.print("I received: ");
       // Serial.println(incomingByte, DEC);
    }
    killed = digitalRead(killPin); //the kill pin only receives data if the system needs to be killed so it we kill the motors for any input from killPin
    /*if (killed) {
      resetAll();
    }*/
    takingAverage();
    depthAve = abs(depthSum/count);
   // Serial.println(depthAve);
    currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
    if (currentMillis - start >= period){  //test whether the period has elapsed
        //9000 is arbitrary integer used as identifier for the serial output
        //it might need to be changed depending on what real depth values the sub achieves
        Serial.println(900000 + depthAve + .1 * killed);
        start = currentMillis;
    }
    
    /*if (incomingByte == 'C'0x7E){
      Serial.print("c is received");
      currentState = States:: WAIT_FOR_FF;
      thrustersOn = 0;
    }*/
        switch(currentState){
        case WAIT_FOR_7E:
            if(hasRun == false){
                trackState = 1;
                Track();
            }
            if(incomingByte == 'X'){
                currentState = WAIT_FOR_FF;
                hasRun = false;
            }
           
            break;
        case WAIT_FOR_FF:
            if(hasRun == false){
                trackState = 2;
                Track();
            }
            switch(incomingByte){
                case 'A'/*0x10*/ : currentState = READ; hasRun = false;
                break;
                case 'B' /*0x11*/: currentState = WRITE; hasRun = false;
                break;
            }
            break;
        case READ:
            if(hasRun == false){
                trackState = 3;
                Track();
            }
            if(incomingByte == 'C'){ 
                hasRun = false; 
                currentState = ADDRESS;
            }
            break;
        case ADDRESS:
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
                States currentState = WAIT_FOR_7E;
            } //does voltage have a specific bit?
            else if (incomingByte == 'Y'/*0x21*/){
                voltageVal = analogRead(voltPin);
                killed = true;
                hasRun = false;
                if(hasRun == false){
                     trackState = 7;
                     Track();
                }
                currentState = WAIT_FOR_7E;
             }
             else if (incomingByte == 'U'/*0x22*/){
                 batteryVal = analogRead(batteryPin);
             }
             else if (incomingByte == 'I'/*0x30*/){
                 digitalRead(killPin);
             }
            break;
        case WRITE:
            if(hasRun == false){
                trackState = 4;
                Track();
            }
            currentState = WAIT_FOR_7E;
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
                depthVal = analogREAD(depthPin);//
                Serial.WRITE(depthVal >> 8);//what is the point of this (does it make it more efficient?
                Serial.WRITE(depthVal & 0xFF);//also what is this doing?
                break;
            case 0x30 : digitalWRITE(killPin, 1);//what does this value mean? does it matter what it is?
                break;
        }
        
    case States::SetThrusters:
       trackState = 5;
        switch(incomingByte){
          //case 0x00 : READ depth sensor
          //case 0x01 : READ kill switch
        }
        break;
    case States::SetThrusterDir:
        if(thrustersOn < ThrustPins ) {
              if(incomingByte == 0) {
                digitalWRITE(dirPins[thrustersOn], LOW);
              } else {
                digitalWRITE(dirPins[thrustersOn], HIGH);
              }
              thrustersOn++;
              
              if(thrustersOn == 4) {
                currentState = States::WAIT_FOR_7E;
                thrustersOn = 0;
              }
        } else {
          currentState = States::WAIT_FOR_7E;
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
              myServo.WRITEMicroseconds((int) (1500 - (400.0 / 255.0) * servoSpeed));
            } else {
                  myServo.WRITEMicroseconds((int) (1500 + (400.0 / 255.0) * servoSpeed));
            }
        } else {  
              myServo.WRITEMicroseconds(1500);
        }   
        break;
    case States::On:
        currentState = States::WAIT_FOR_7E;  
    }*/
}
}
