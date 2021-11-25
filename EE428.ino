#include <Servo.h>
#include <SharpIR.h>
#include <Math.h>
#include <MedianFilterLib2.h>

// Define model and input pin:
#define IRPinR A0
#define IRPinL A5
#define IRPinM A4
#define model 430

// Create variable to store the distance:
float distanceR_cm;  // distance on left sensor
float distanceL_cm;  // distance on right sensor
float distanceM_cm;  // distance on middle sensor

float pie = 3.1415926535897932384626433832795;
float sensorDist; // distance between sensors

float angle_wall;     // angle with wall
float angle_adj;      // angle of adjustment required to be perpendicular to wall

float medianR;        //right
float medianL;        //left
float medianM;        //middle

byte percentL = 0;   //percent of left motors
byte percentR = 0;   //from -100 to 100

int PWMvalueL = 1500;
int PWMvalueR = 1500;

//direction to get closer to target
bool forward = false;
bool right = false;
bool left = false;

// Create a new instance of the SharpIR class:
SharpIR IrRSensor = SharpIR(IRPinR, model);
SharpIR IrLSensor = SharpIR(IRPinL, model);
SharpIR IrMSensor = SharpIR(IRPinM, model);

int ws = 200;  // window size for the median finder

Servo servo1, motorR, motorL;

void setup() {
  servo1.attach(9, 500, 2500);    //flipper servo
  servo1.write(90);
  motorR.attach(3); //associate the right to a pin
  motorL.attach(5); //associate the left to a pin

  Serial.begin (115200);
  Serial.println (F("Ready!"));
}

void loop() {
  
  sensorDist = 4.5; // inches between IR sensor

  // for loop for finding the medians of the IR data sets
  //smoothes out random outliers
  for (int i = 0; i < ws+1; i++)
  {
    MedianFilter2<float> medianFilter2(ws);

    // Get a distance measurement and store it as distance_cm:
    distanceR_cm = IrRSensor.getDistance();
    distanceL_cm = IrLSensor.getDistance();
    distanceM_cm = IrMSensor.getDistance();

    medianL = medianFilter2.AddValue(distanceL_cm); 
    medianR = medianFilter2.AddValue(distanceR_cm); 
    medianM = medianFilter2.AddValue(distanceM_cm);
  }

  //may not need this, keep just in case
  //angle_adj = atan((medianR/2.54 - medianL/2.54)/sensorDist)/pie*180; // find the angle of adjustment. converts cm to Inches before calculation, 

  
  
  if (!forward and !left and !right)    //no targets in range
    seek_target();                      //find new target
  else
    follow();

  
  delay(50);
}

void seek_target()    //find something to follow
{
  //spin to the right slowly
  motorR.writeMicroseconds(1475);   //motors are reversed from each other
  motorL.writeMicroseconds(1475);   //each motor 5%
}

void follow()         //follow your target
{
  //scale motor % to a motor PWM value
  PWMvalueL = percentL * 5 + 1500; //scale up to 1000-2000
  PWMvalueR = percentR * 5 + 1500; //scale up to 1000-2000

  //write PWM value to data pins
  motorR.writeMicroseconds(PWMvalueR);
  motorL.writeMicroseconds(PWMvalueL);
}
