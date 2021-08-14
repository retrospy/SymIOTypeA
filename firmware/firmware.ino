#include "SlowSoftSerial.h"

SlowSoftSerial sss(0,1);
char currentCommand;
bool awaitingCommand = false;
bool awaitingSecondaryCommand = false;
int baudIdx = 0;
int buadRates[6] = {110, 300, 600, 1200, 2400, 4800};
const char* buadRatesStr[6]= {"110", "300", "600", "1200", "2400", "4800"};
const char* buadRatesCode[6]= {"D5", "4C", "24", "10", "06", "01"};
const bool buadRatesEcho[6]= {true, true, false, false, false, false};

void setup() {
  pinMode(2, INPUT);
  Serial.begin(115200);
  sss.begin(buadRates[baudIdx]);
}

void loop() {

  if (digitalRead(2) == LOW)
    _reboot_Teensyduino_();
    
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
