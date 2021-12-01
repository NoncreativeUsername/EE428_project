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

String direction_prime = "hold";
String direction_1 = "hold";
String direction_2 = "hold";
String direction_3 = "hold";

int PWMvalueL = 1500;
int PWMvalueR = 1500;


// Create a new instance of the SharpIR class:
//SharpIR IrRSensor = SharpIR(IRPinR, 430);
//SharpIR IrLSensor = SharpIR(IRPinL, 430);
//SharpIR IrMSensor = SharpIR(A0, 1080);

int ws = 150;  // window size for the median finder

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
  
  for (int i = 0; i < ws+1; i++)
  {
    MedianFilter2<float> medianFilter2(ws);

    // Get a distance measurement and store it as distance_cm:
    value3 = analogRead(A2);  //left

    medianL = medianFilter2.AddValue(value3);
  }

    for (int i = 0; i < ws+1; i++)
  {
    MedianFilter2<float> medianFilter2(ws);

    // Get a distance measurement and store it as distance_cm:
    value1 = analogRead(A0);  //right

    medianR = medianFilter2.AddValue(value1); 
  }

    for (int i = 0; i < ws+1; i++)
  {
    MedianFilter2<float> medianFilter2(ws);

    // Get a distance measurement and store it as distance_cm:
    value2 = analogRead(A1);  //center
 
    medianM = medianFilter2.AddValue(value2);
  }
  
  //value1 = analogRead(A0);  //right
  //value2 = analogRead(A1);  //center
  //value3 = analogRead(A2);  //left
  Serial.print("R: ");
  Serial.print(medianR);
  //value1 = analogRead(A0)*0.0048828125;
  //distanceM_cm = 13*pow(value, -1);
  Serial.print("  C: ");
  Serial.print(medianM);
  Serial.print("  L: ");
  Serial.println(medianL);

  if (medianM > 250)     //object to close stop
  {
    direction_1 = "hold";
  }
  else if (medianM > 120)     //object to far, follow
  {
    direction_1 = "forward";
  }
  else if (medianL > 50)    //object to the left, turn
  {
    direction_1 = "left";
  }
  else if (medianR > 50)    //object to the right, turn
  {
    direction_1 = "right";
  }
  else                      //object lost, search
  {
    direction_1 = "spin";
  }

  //direction must be the same 3 times in a row before being changed
  if ((direction_1 == direction_2) and (direction_2 == direction_3))
  {
    direction_prime = direction_1;
  }

  //kind of a shift register
  direction_3 = direction_2;
  direction_2 = direction_1;
  
  if (direction_prime == "hold")     //object to close stop
  {
    Serial.println("hold");
    PWMvalueR = 1500;
    PWMvalueL = 1500;
  }
  else if (direction_prime == "forward")     //object to far, follow
  {
    Serial.println("forward");
    PWMvalueR = 1600;
    PWMvalueL = 1400;
  }
  else if (direction_prime == "left")    //object to the left, turn
  {
    Serial.println("turn left");
    PWMvalueR = 1600;
    PWMvalueL = 1600;
    /*use this if you need to slow it down
    delay(50);
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);
    */
  }
  else if (direction_prime == "right")    //object to the right, turn
  {
    Serial.println("turn right");
    PWMvalueR = 1400;
    PWMvalueL = 1400;
    /*
    delay(50);
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);*/
  }
  else                      //object lost, search
  {
    Serial.println("spin");
    PWMvalueR = 1400;
    PWMvalueL = 1400;
  }

  //try to stop random jumps in motor speed
  PWMvalueR = constrain(PWMvalueR, 1400, 1600);
  PWMvalueL = constrain(PWMvalueL, 1400, 1600);

  motorR.writeMicroseconds(PWMvalueR);
  motorL.writeMicroseconds(PWMvalueL);

  //delay(50);    //unsure if delay is necessary
}
