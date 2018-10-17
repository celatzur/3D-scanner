// ************************************************************************************************************************
// *** 3D scanner turntable
// ***
// *** Turntable for a 3D scanner controlled with arduino, who also shoots, via bluetooth, a phone camera to reconstruct 
// *** the object with photogrametry.
// *** 
// *** Components:
// *** 
// *** - Arduino Nano
// *** - 2 push buttons (start/stop, shoot)
// *** - On/off button for bluetooth
// *** - Cheap Bluetooth device
// *** - 28BYJ-48 Stepper Motor
// *** - ULN2003 driver for the motor
// *** 
// *** Created at 08 of October 2018
// *** By Celatzur
// *** Modified at 09 of October 2018
// *** By Celatzur
// *** 
// *** https://github.com/celatzur/3D-scanner
// *** 
// ************************************************************************************************************************

// *** Library to control the 28BYJ-48 Stepper Motor, better than the default <Stepper.h>
#include "AccelStepper.h"
#include <EnableInterrupt.h> 

#define HALFSTEP 8 //Half-step mode (8 step control signal sequence)

// Motor pin definitions
#define motorPin1  2     // IN1 on the ULN2003 driver
#define motorPin2  3     // IN2 on the ULN2003 driver
#define motorPin3  4     // IN3 on the ULN2003 driver
#define motorPin4  5     // IN4 on the ULN2003 driver

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

double stepsPerDegree = 4078.0 * 2.0 / 360.0;

// Push-button definition
#define buttonStartPin 6  // Start/Stop button
#define buttonShootPin 7  // Shoot camera button

int buttonStartState;             // the current reading from the input pin
int buttonStartLastState = LOW;   // the previous reading from the input pin
int buttonShootState;             // the current reading from the input pin
int buttonShootLastState = LOW;   // the previous reading from the input pin

volatile bool buttonPushed=false;  //Variable to remember when a button has been pushed
volatile bool buttonStartPushed=false; //Variable to remember when Start button has been pushed
volatile bool buttonShootPushed=false; //Variable to remember when Shoot button has been pushed

#define debounceDelay = 100 //in ms, for the push buttons

// LED definition to indicate the running state
#define ledStartPin = 8;      // the number of the LED pin
int ledStartState = HIGH;         // the current state of the output pin

// ************************************************************************************************************************
// *** Setup  
// ************************************************************************************************************************

void setup() {
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(200);
  //stepper1.moveTo(4096/2);  //Rotate 1 revolution

  pinMode(buttonStartPin, INPUT);
  pinMode(buttonShootPin, INPUT);  
  pinMode(ledStartPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledStartPin, ledStartState);

  //Interruptions
  enableInterrupt(buttonStartPin, isr_startButtonPushed, RISING);
  //enableInterrupt(buttonShootPin, isr_shootButtonPushed, RISING);
}

// ************************************************************************************************************************
// *** Main loop: wait an interrupt to start/stop the process. It can shoot a photo
// ************************************************************************************************************************

void loop() {

  while(!buttonPushed){ 
    //do nothing
  }

 //Change direction when the stepper reaches the target position
  if (stepper1.distanceToGo() == 0) {
    stopTable();
 //   stepper1.moveTo(-stepper1.currentPosition());  //Reverse motor
  }

  stepper1.run();//Start
}

// ************************************************************************************************************************
// *** Functions
// ************************************************************************************************************************

// *** Function executed when start button is pushed, through an interrupt
void isr_startButtonPushed(){ 

  uint32_t interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > debounceDelay) {
    led_status = !led_status;
    digitalWrite(LED_BUILTIN, led_status);
  }

  last_interrupt_time = interrupt_time;

    startButtonPushed=true;
//if(buttonPushed){break;}  dins dun for

    if(!buttonPushed){
        buttonPushed=true;
        //Do whatever, Check if it have already started, then it's stop, else is start until finishing
    }    
}

// *** Function executed when shoot button is pushed, there is no need for an interruption
void shootButtonPushed(){ 

    shootButtonPushed=true;

    if(!buttonPushed){
        buttonPushed=true;
        //Do whatever, Shoot the camera, if it's connected
    }
}

// *** Function to stop turning the table
void stopTable(){
}

// *** Function to start tunrning the table untill one revolution
void startTable(){
}
