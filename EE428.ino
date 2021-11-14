#include <PS2X_lib.h>
#include <Servo.h>

PS2X ps2x;
 
//right now, the library does NOT support hot-pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

//motor pwm values
int percentL = 0;   //percent of left motors
int percentR = 0;   //from -100 to 100
//int dir = 0;        //int input will determine robot direction
int pins[] = {6, 9}; //the PWM signal output pins

const int arraySize = sizeof(pins)/sizeof(int);
Servo controllers[arraySize];
Servo flip;         //flipper servo
int flip_pos = 0;   //flipper position

//controller values
int error = 0; 
byte type = 0;
byte vibrate = 0;

void setup(){
  Serial.begin(9600);
  
  for (int i=0; i<arraySize; i++)
    controllers[i].attach(pins[i]); //associate the object to a pin
    

  error = ps2x.config_gamepad(13,11,10,12, true, true);   //GamePad(clock, command, attention, data, Pressures?, Rumble?) 
  if(error == 0){
    Serial.println("Found Controller, configured successful");
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

void loop(){
  int PWMvalueL = percentL * 5 + 1500; //scale up to 1000-2000
  int PWMvalueR = percentR * 5 + 1500; //scale up to 1000-2000

  //write PWM value to data pins
  controllers[0].writeMicroseconds(PWMvalueR);
  controllers[1].writeMicroseconds(PWMvalueR);
  
  /* You must Read Gamepad to get new values
  Read GamePad and set vibration values
  ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
  if you don't enable the rumble, use ps2x.read_gamepad(); with no values
  you should call this at least once a second
  */
if(error == 1) 
 return; 

//DualShock Controller
  ps2x.read_gamepad();          //read controller
  if (ps2x.Button(PSB_R2))      //right trigger
  {
    percentR = 10;
  }
  if (ps2x.Button(PSB_L2))      //left trigger
  {
    percentL = 10; 
  }

  if (ps2x.Button(PSB_BLUE))            //activate flipper when button is pressed
  {
    flip_pos = 45;
  }
  else
  {
    flip_pos = 0;
  }

  flip.write(flip_pos);         //write flipper angle

delay(50);
} 
