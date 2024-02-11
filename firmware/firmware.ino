#if !defined(ARDUINO_RASPBERRY_PI_PICO)
#include "SlowSoftSerial.h"

SlowSoftSerial sss(0,1);
#else
#define sss Serial1
#endif

char currentCommand;
bool awaitingCommand = false;
bool awaitingSecondaryCommand = false;
int baudIdx = 0;
int buadRates[6] = {110, 300, 600, 1200, 2400, 4800};
const char* buadRatesStr[6]= {"110", "300", "600", "1200", "2400", "4800"};
const char* buadRatesCode[6]= {"D5", "4C", "24", "10", "06", "01"};
const bool buadRatesEcho[6]= {true, true, false, false, false, false};

 #define AIRCR_Register (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)))

void setup() {
  pinMode(2, INPUT_PULLUP);
  Serial.begin(115200);
#if defined(ARDUINO_RASPBERRY_PI_PICO)
  Serial1.setRX(1);
  Serial1.setTX(0);
#endif
  sss.begin(buadRates[baudIdx]);
}

void loop() {

  if (digitalRead(2) == LOW)
#if !defined(ARDUINO_RASPBERRY_PI_PICO)
    _reboot_Teensyduino_();
#else
    AIRCR_Register = 0x5FA0004;
#endif

  if (Serial.available()) 
  {      
    char c = Serial.read();
    
    if (c == 26 )
    {
      baudIdx = (baudIdx + 1) % 6;
     
      sss.end();
      sss.begin(buadRates[baudIdx]);

      Serial.println("");
      Serial.print("Setting baud to ");
      Serial.print(buadRatesStr[baudIdx]);
      Serial.println(".");
      
      return;
    }
    else 
    {
      sss.write(c);   // read it and send it out sss (pins 0 & 1)
    }
    if (buadRatesEcho[baudIdx])
    {
      currentCommand = c;
    }
  }
  
  if (sss.available()) 
  {    
    char c = sss.read();
    bool p = true;
    if (c == 195)
      c = 13;
    if (c == 254 && buadRatesEcho[baudIdx])
    {
      Serial.write(currentCommand);
      c = ' ';
    }
    else if (c == 225)
    {
      Serial.write(10);
      c = 13;
    }
  
    if (p)
      Serial.write(c & 0b01111111);
  }

}
