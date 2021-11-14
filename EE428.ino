#include <PS2X_lib.h>
#include <Servo.h>
#include <SharpIR.h>
#include <Math.h>
#include <MedianFilterLib2.h>

// Define model and input pin:
#define IRPinR A0
#define IRPinL A5
#define model 430

// Create variable to store the distance:
float distanceR_cm;  // distance on left sensor
float distanceL_cm;  // destance on right sensor

float pie = 3.1415926535897932384626433832795;
float sensorDist; // distance between sensors

float angle_wall;    // angle with wall
float angle_adj;     // angle of adjustment required to be perpendicular to wall

float medianR;
float medianL;

// Create a new instance of the SharpIR class:
SharpIR IrRSensor = SharpIR(IRPinR, model);
SharpIR IrLSensor = SharpIR(IRPinL, model);

int ws = 200;  // window size for the median finder

PS2X ps2x;

//motor pwm values
int percentL = 0;   //percent of left motors
int percentR = 0;   //from -100 to 100
//int dir = 0;        //int input will determine robot direction
int pins[] = {6, 8}; //the PWM signal output pins

const int arraySize = sizeof(pins)/sizeof(int);
Servo controllers[arraySize];

//controller values
int error = 0; 
byte type = 0;
byte vibrate = 0;

void setup() {
  Serial.begin(9600);
  
  for (int i=0; i<arraySize; i++)
    controllers[i].attach(pins[i]); //associate the object to a pin
  error = ps2x.config_gamepad(13,11,10,12, true, true);   //GamePad(clock, command, attention, data, Pressures?, Rumble?) 
  if(error == 0){
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    //Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  type = ps2x.readType(); 
    switch(type) {
      case 0:
       Serial.println("Unknown Controller type");
      break;
      case 1:
       Serial.println("DualShock Controller Found");
      break;
      case 2:
        Serial.println("GuitarHero Controller Found");
      break;
    }
    delay(500);
}

void loop() {
  sensorDist = 4.5; // inches between IR sensor

  // for loop for finding the medians of the IR data sets
  for (int i = 0; i < ws+1; i++){
    MedianFilter2<float> medianFilter2(ws);

  // Get a distance measurement and store it as distance_cm:
  distanceR_cm = IrRSensor.distance();
  
  int PWMvalueL = percentL * 5 + 1500; //scale up to 1000-2000
  int PWMvalueR = percentR * 5 + 1500; //scale up to 1000-2000

  //write PWM value to data pins
  controllers[0].writeMicroseconds(PWMvalueR);
  controllers[1].writeMicroseconds(PWMvalueR);
  
  delay(50);
}
