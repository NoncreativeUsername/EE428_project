#include <Servo.h>
#include <DigitalIO.h>
#include <PsxControllerBitBang.h>
#include <avr/pgmspace.h>


//start controller stuff
#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

// These can be changed freely when using the bitbanged protocol
const byte PIN_PS2_ATT = 10;
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 13;

const byte PIN_BUTTONPRESS = A0;
const byte PIN_HAVECONTROLLER = A1;

const char buttonSelectName[] PROGMEM = "Select";
const char buttonL3Name[] PROGMEM = "L3";
const char buttonR3Name[] PROGMEM = "R3";
const char buttonStartName[] PROGMEM = "Start";
const char buttonUpName[] PROGMEM = "Up";
const char buttonRightName[] PROGMEM = "Right";
const char buttonDownName[] PROGMEM = "Down";
const char buttonLeftName[] PROGMEM = "Left";
const char buttonL2Name[] PROGMEM = "L2";
const char buttonR2Name[] PROGMEM = "R2";
const char buttonL1Name[] PROGMEM = "L1";
const char buttonR1Name[] PROGMEM = "R1";
const char buttonTriangleName[] PROGMEM = "Triangle";
const char buttonCircleName[] PROGMEM = "Circle";
const char buttonCrossName[] PROGMEM = "Cross";
const char buttonSquareName[] PROGMEM = "Square";

const char* const psxButtonNames[PSX_BUTTONS_NO] PROGMEM = {
  buttonSelectName,
  buttonL3Name,
  buttonR3Name,
  buttonStartName,
  buttonUpName,
  buttonRightName,
  buttonDownName,
  buttonLeftName,
  buttonL2Name,
  buttonR2Name,
  buttonL1Name,
  buttonR1Name,
  buttonTriangleName,
  buttonCircleName,
  buttonCrossName,
  buttonSquareName
};
byte psxButtonToIndex (PsxButtons psxButtons) {
  byte i;

  for (i = 0; i < PSX_BUTTONS_NO; ++i) {
    if (psxButtons & 0x01) {
      break;
    }

    psxButtons >>= 1U;
  }

  return i;
}

FlashStr getButtonName (PsxButtons psxButton) {
  FlashStr ret = F("");
  
  byte b = psxButtonToIndex (psxButton);
  if (b < PSX_BUTTONS_NO) {
    PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
    ret = PSTR_TO_F (bName);
  }

  return ret;
}
// reverse engineer this part
void dumpButtons (PsxButtons psxButtons) {
  static PsxButtons lastB = 0;

  if (psxButtons != lastB) {
    lastB = psxButtons;     // Save it before we alter it
    
    Serial.print (F("Pressed: "));

    for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
      byte b = psxButtonToIndex (psxButtons);
      if (b < PSX_BUTTONS_NO) {
        PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
        Serial.print (PSTR_TO_F (bName));
      }

      psxButtons &= ~(1 << b);

      if (psxButtons != 0) {
        Serial.print (F(", "));
      }
    }

    Serial.println ();
  }
}
//might delete this func
void dumpAnalog (const char *str, const byte x, const byte y) {
  Serial.print (str);
  Serial.print (F(" analog: x = "));
  Serial.print (x);
  Serial.print (F(", y = "));
  Serial.println (y);
}



const char ctrlTypeUnknown[] PROGMEM = "Unknown";
const char ctrlTypeDualShock[] PROGMEM = "Dual Shock";
const char ctrlTypeDsWireless[] PROGMEM = "Dual Shock Wireless";
const char ctrlTypeGuitHero[] PROGMEM = "Guitar Hero";
const char ctrlTypeOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerTypeStrings[PSCTRL_MAX + 1] PROGMEM = {
  ctrlTypeUnknown,
  ctrlTypeDualShock,
  ctrlTypeDsWireless,
  ctrlTypeGuitHero,
  ctrlTypeOutOfBounds
};


PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

boolean haveController = false;
//end controller stuff

//start servo stuff
Servo servo1;   //create servo

const int servo1Pin = 9;  // servo1 接 Pin 9
const int every1 = 15;
int servo1Target = 0;

int servo1Dir = 10;

int posOfServo1 = 90;   //intial servo position
//end servo stuff

//start motor stuff
int percentL = 0;   //percent of left motors
int percentR = 0;   //from -100 to 100
int pins[] = {5, 3}; //the PWM signal output pins

//const int arraySize = sizeof(pins)/sizeof(int);
Servo motorR, motorL;

void setup(){
  pinMode(5, OUTPUT);     //left motor
  pinMode(3, OUTPUT);     //right motor

  servo1.attach(servo1Pin, 500, 2500);    //attach servo on pin 9

  //for (int i=0; i<arraySize; i++)
    //controllers[i].attach(pins[i]); //associate the object to a pin

  motorL.attach(6); //associate the left to a pin
  motorR.attach(3); //associate the right to a pin
  
  fastPinMode (PIN_BUTTONPRESS, OUTPUT);
  fastPinMode (PIN_HAVECONTROLLER, OUTPUT);
  
  delay (300);

  Serial.begin (115200);
  Serial.println (F("Ready!"));


}

 
void loop () {
  static byte slx, sly, srx, sry;
  
  fastDigitalWrite (PIN_HAVECONTROLLER, haveController);

  //hot swap stuff
  if (!haveController) {
    if (psx.begin ()) {
      Serial.println (F("Controller found!"));
      delay (300);
      if (!psx.enterConfigMode ()) {
        Serial.println (F("Cannot enter config mode"));
      } else {
        PsxControllerType ctype = psx.getControllerType ();
        PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
        Serial.print (F("Controller Type is: "));
        Serial.println (PSTR_TO_F (cname));

        if (!psx.enableAnalogSticks ()) {
          Serial.println (F("Cannot enable analog sticks"));
        }
        
        //~ if (!psx.setAnalogMode (false)) {
          //~ Serial.println (F("Cannot disable analog mode"));
        //~ }
        //~ delay (10);
        
        if (!psx.enableAnalogButtons ()) {
          Serial.println (F("Cannot enable analog buttons"));
        }
        
        if (!psx.exitConfigMode ()) {
          Serial.println (F("Cannot exit config mode"));
        }
      }
      
      haveController = true;
    }
  } else {
    if (!psx.read ()) {
      Serial.println (F("Controller lost :("));
      haveController = false;
    } else {
      fastDigitalWrite (PIN_BUTTONPRESS, !!psx.getButtonWord ());
      dumpButtons (psx.getButtonWord ());
      /*
      byte lx, ly;
      psx.getLeftAnalog (lx, ly);
      if (lx != slx || ly != sly) {
        dumpAnalog ("Left", lx, ly);
        slx = lx;
        sly = ly;
      }

      byte rx, ry;
      psx.getRightAnalog (rx, ry);
      if (rx != srx || ry != sry) {
        dumpAnalog ("Right", rx, ry);
        srx = rx;
        sry = ry;
      }
      */
    }
  }

  //get left stick values for driving
  byte lx, ly;
  psx.getLeftAnalog (lx, ly);
  
  //get right stick values for driving
  byte rx, ry;
  psx.getRightAnalog(rx, ry);

  //scale stick y values to motor percent
  percentR = -(ry - 128)*0.1;
  percentL = -(ly - 128)*0.1;

  //scale values to pwm values
  int PWMvalueL = percentL * 5 + 1500; //scale up to 1000-2000
  int PWMvalueR = percentR * 5 + 1500; //scale up to 1000-2000

  //write PWM value to motor pins
  motorL.writeMicroseconds(PWMvalueR);
  motorR.writeMicroseconds(PWMvalueL);
  
  delay (1000 / 60);
} 
