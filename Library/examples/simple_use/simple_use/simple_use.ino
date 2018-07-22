#include <at200pc.h>

At200pc at(Serial1,21); // Serial1 is the uart on the uC, and 21 is the pin to RTS signal to wake up the tunner



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {

  
  
  at.write(REQ_ANT1); // Send a command to devices

 at.read(); // This function parse all information from device.
 
 Serial.println(at.antenna_selected());
  

}
