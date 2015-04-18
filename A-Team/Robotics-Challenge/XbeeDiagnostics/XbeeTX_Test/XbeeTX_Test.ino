#include <XBee.h>

XBee xbee = XBee();

void setup(){
  Serial.begin(57600);
  Serial1.begin(57600);
  xbee.setSerial(Serial1);
}

void loop(){
  uint8_t payload[] = {100};
  //Address of receiving device can be anything while in broadcasting mode
  Tx16Request tx = Tx16Request(0x5678, payload, sizeof(payload));
  xbee.send(tx);
  
  //Delay must be longer than the readPacket timeout on the receiving module
  delay(30);
}


