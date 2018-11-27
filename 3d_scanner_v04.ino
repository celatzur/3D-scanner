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
// *** Modified at 18 of November 2018
// *** By Celatzur
// *** 
// *** https://github.com/celatzur/3D-scanner
// *** 
// ************************************************************************************************************************

// ************************************************************************************************************************
// *** TTD
// *** Detect if the bluetooth remote is ON, and don't send signals anyway. 
// *** I must wait for about 15 seconds after power up, before sending photo-shoot signals (connecting them to ground)
// *** Connect and activate the buzzer
// ************************************************************************************************************************

// *** Library to control the 28BYJ-48 Stepper Motor, better than the default <Stepper.h>
#include "AccelStepper.h"
#include <EnableInterrupt.h> 

#define DEBUG 1                           // For scaffolding

#define HALFSTEP 8                        // Half-step mode (8 step control signal sequence)

// Motor pin definitions
#define motorPin1  5                      // IN1 on the ULN2003 driver
#define motorPin2  4                      // IN2 on the ULN2003 driver
#define motorPin3  6                      // IN3 on the ULN2003 driver
#define motorPin4  7                      // IN4 on the ULN2003 driver

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepperMotor(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// double stepsPerDegree = 4078.0 * 2.0 / 360.0;

// Push-button definition
const int buttonStartPin = 3;             // Start or Stop button
const int buttonShootPin = 2;             // Shoot camera button

volatile bool buttonStartPushed=false;    // Variable to remember when Start button has been pushed (volatile inside ISR)
volatile bool buttonShootPushed=false;    // Variable to remember when Shoot button has been pushed (volatile inside ISR)

const int debounceDelay = 100;            //in ms, for the push buttons

// LED definition to indicate the running state
const int ledStartPin = 8;                // Number of the LED pin

int boardLedPin = 13; 

const int shootPin = 10;                  // Pin to shoot the bluetooth camera (LOW)

const int buzzerPin = 9;                  // Pin to buzzer to mark the start and ending of a full rotation

volatile bool tableRotating=false;        // To define if the motor is already rotating

const int stepsBetweenShoots = 500;
int nStepsBwShots = 0;
int i = 0;



// ************************************************************************************************************************
// *** Setup  
// ************************************************************************************************************************

void setup() {
  stepperMotor.setMaxSpeed(1000.0);
  stepperMotor.setAcceleration(100.0);
  stepperMotor.setSpeed(100);
  stepperMotor.setCurrentPosition(0);
  stepperMotor.moveTo(4096);          // 4096 steps for 1 rotation in 28BYJ-48 – 5V Stepper Motor
  //stepperMotor.moveTo(0);
  stepperMotor.run(); 

  pinMode(buttonStartPin, INPUT);
  pinMode(buttonShootPin, INPUT); 
   
  pinMode(ledStartPin, OUTPUT);
  digitalWrite(ledStartPin, LOW);         // LOW for LED off

  pinMode(boardLedPin, OUTPUT);
  digitalWrite(boardLedPin, LOW);         // LOW for LED off

  pinMode(buzzerPin, OUTPUT);
  pinMode(shootPin, OUTPUT);
  digitalWrite(shootPin, HIGH);           // LOW for shooting photo

  //Interruptions
  enableInterrupt(buttonStartPin, isr_startButtonPushed, RISING);
  enableInterrupt(buttonShootPin, isr_shootButtonPushed, RISING);

  Serial.begin(115200);
  
  #ifdef DEBUG
  Serial.print("Finished setup \n");
  delay(1000);
  #endif
}

// ************************************************************************************************************************
// *** Main loop: wait an interrupt to start or stop the process. It can shoot a photo
// ************************************************************************************************************************

void loop() {
//stepperMotor.run(); 

/*
#ifdef DEBUG
i++;

if (i == 100) {
  Serial.print("Start main loop. ");
  Serial.print("TableRotating = ");
  Serial.print(tableRotating);
  Serial.print("\n");
  delay(1000);
  i=0;
  }

#endif
*/
if (buttonStartPushed == true) {        // If the ISR says the button is pushed
  buttonStartPushed = false;
  if (tableRotating == false) {         // If its not moving, starts to move
    startRotating();
    tableRotating = true; 
    }
  else {                                // If its moving, stops it
    stopRotating();
    tableRotating = false; 
    }
  }

if (tableRotating == true) {
    nStepsBwShots++;
    if (nStepsBwShots == stepsBetweenShoots ) {
      nStepsBwShots=0;
      //stepperMotor.stop();                //Stop
      shootPhoto();
      //stepperMotor.run();                 //Start
    }
  }

 //When the motor reaches target position, STOPs and beeps
if (stepperMotor.distanceToGo() == 0) {
    stopRotating();
    //stepperMotor.moveTo(-stepperMotor.currentPosition());  //Reverse motor
  }
  else {
    stepperMotor.run();         //Run the stepper towards the destined position
                                //Whenever the run() is called the stepper moves only 1 step onto the direction assigned
    //Serial.print("Running \n");
  }
  
}

// ************************************************************************************************************************
// *** Interrupt function executed when start button is pushed
// ************************************************************************************************************************
void isr_startButtonPushed(){ 
  
  static unsigned long last_interrupt_time = 0;
  uint32_t interrupt_time = millis();
    
  if (interrupt_time - last_interrupt_time > debounceDelay) {
    buttonStartPushed = true;
    #ifdef DEBUG
    Serial.print("ISR START button \n");
//    delay(1000);
    #endif
  }

  last_interrupt_time = interrupt_time;
}

// ************************************************************************************************************************
// *** Interrupt function executed when shoot photo button is pushed
// ************************************************************************************************************************
void isr_shootButtonPushed(){ 

  static unsigned long last_interrupt_time = 0;
  uint32_t interrupt_time = millis();
    
  if (interrupt_time - last_interrupt_time > debounceDelay) {
    #ifdef DEBUG
    Serial.print("ISR Shoot button \n");
//    delay(1000);
    #endif

    shootPhoto();                             //Shoot the camera, if it's connected  
    }

  last_interrupt_time = interrupt_time;
}

// ************************************************************************************************************************
// *** Function to start tunrning the table untill one revolution is done
// ************************************************************************************************************************
void startRotating(){
  #ifdef DEBUG
  Serial.print("\n Start rotating table \n");
//  delay(1000);
  #endif

  digitalWrite(ledStartPin, HIGH);
  stepperMotor.moveTo(4096);                  //4096 steps for 1 rotation in 28BYJ-48 – 5V Stepper Motor
  stepperMotor.run();                         //Start
}

// ************************************************************************************************************************
// *** Function to stop turning the table
// ************************************************************************************************************************
void stopRotating(){
  #ifdef DEBUG
  //Serial.print("Stop rotating table \n");
  //delay(1000);
  #endif

  digitalWrite(ledStartPin, LOW);
  stepperMotor.stop();                        //Stops
}

// ************************************************************************************************************************
// *** Shoots a photo
// ************************************************************************************************************************
void shootPhoto(){
    #ifdef DEBUG
    Serial.print("Shoot photo, stepperMotor.distanceToGo()=");     
    Serial.print(stepperMotor.distanceToGo());  
    Serial.print("\n");  
    //delay(1000);
    digitalWrite(boardLedPin, HIGH);
    delay(1000);
    digitalWrite(boardLedPin, LOW);
    #endif

    digitalWrite(shootPin, LOW);
    delay(500);
    digitalWrite(shootPin, HIGH);
}

