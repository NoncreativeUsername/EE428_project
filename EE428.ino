
#include <Servo.h>
#include <SharpIR.h>
#include <Math.h>
#include <MedianFilterLib2.h>

// Create variable to store the distance:
float distanceR_cm;  // distance on left sensor
float distanceL_cm;  // distance on right sensor
float distanceM_cm;  // distance on middle sensor

float medianR;        //right
float medianL;        //left
float medianM;        //middle


int PWMvalueL = 1500;
int PWMvalueR = 1500;


// Create a new instance of the SharpIR class:
//SharpIR IrRSensor = SharpIR(IRPinR, 430);
//SharpIR IrLSensor = SharpIR(IRPinL, 430);
//SharpIR IrMSensor = SharpIR(A0, 1080);

int ws = 200;  // window size for the median finder

Servo servo1, motorR, motorL;
float value1 = 0;
float value2 = 0;
float value3 = 0;

void setup() {
  servo1.attach(9, 500, 2500);    //flipper servo
  servo1.write(90);
  motorR.attach(3); //associate the right to a pin
  motorL.attach(5); //associate the left to a pin

  Serial.begin (115200);
}

void loop() {

  // for loop for finding the medians of the IR data sets
  //smoothes out random outliers
  /*
  for (int i = 0; i < ws+1; i++)
  {
    MedianFilter2<float> medianFilter2(ws);

    // Get a distance measurement and store it as distance_cm:
    value1 = analogRead(A0);  //right
    value2 = analogRead(A1);  //center
    value3 = analogRead(A2);  //left
    //distanceR_cm = IrRSensor.getDistance();
    //distanceL_cm = IrLSensor.getDistance();
    //distanceM_cm = IrMSensor.getDistance();

    medianL = medianFilter2.AddValue(value3); 
    medianR = medianFilter2.AddValue(value1); 
    medianM = medianFilter2.AddValue(value2);
  }*/

  //may not need this, keep just in case
  //angle_adj = atan((medianR/2.54 - medianL/2.54)/sensorDist)/pie*180; // find the angle of adjustment. converts cm to Inches before calculation,
  value1 = analogRead(A0);  //right
  value2 = analogRead(A1);  //center
  value3 = analogRead(A2);  //left
  Serial.print("R: ");
  Serial.print(value1);
  //value1 = analogRead(A0)*0.0048828125;
  //distanceM_cm = 13*pow(value, -1);
  Serial.print("  C: ");
  Serial.print(value2);
  Serial.print("  L: ");
  Serial.println(value3);

  if (value2 > 120)     //object to close stop
  {
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);
  }
  else if (value2 > 40)     //object to far, follow
  {
    motorR.writeMicroseconds(1475);
    motorL.writeMicroseconds(1525);
  }
  else if (value3 > 120)    //object to the left, turn
  {
    motorR.writeMicroseconds(1525);
    motorL.writeMicroseconds(1525);
    /*use this if you need to slow it down
    delay(50);
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);
    */
  }
  else if (value2 > 120)    //object to the right, turn
  {
    motorR.writeMicroseconds(1475);
    motorL.writeMicroseconds(1475);
    /*
    delay(50);
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);*/
  }
  else                      //object lost, search
  {
    motorR.writeMicroseconds(1475);
    motorL.writeMicroseconds(1475);
  }


  delay(50);    //unsure if delay is necessary
}
