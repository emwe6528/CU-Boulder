#include <XBee.h>
#include <Wire.h>
#include <HMC5883L.h>


HMC5883L compass;
XBee xbee = XBee();
int compassAddress = 0x42 >> 1;



void setup(){
  Wire.begin();
  Serial.begin(57600);
  Serial1.begin(57600);
  xbee.setSerial(Serial1);
  
  compass = HMC5883L();  //new instance of HMC5883L library
  setupHMC5883L();  //setup the HMC5883L
}

void loop(){
  
  float currentVector = getVector();
  uint8_t payload[] = {currentVector/2};

  //Address of receiving device can be anything while transmitting in broadcasting mode
  Tx16Request tx = Tx16Request(0x5678, payload, sizeof(payload));
  xbee.send(tx);
  
  //Delay must be longer than the readPacket timeout on the receiving module
  delay(10);
}



//////////////////////////////////////////////
/////////////Local Functions//////////////////
/////////////////////////////////////////////

// Sets up the compass
void setupHMC5883L(){
  //Setup the HMC5883L, and check for errors
  int error;  
  error = compass.SetScale(1.3); //Set the scale of the compass.
  if(error != 0) Serial.println(compass.GetErrorText(error)); //check if there is an error, and print if so

  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) Serial.println(compass.GetErrorText(error)); //check if there is an error, and print if so
}


// This the the fucntion which gathers the heading from the compass.
float getVector(){
  //Get the reading from the HMC5883L and calculate the heading
  MagnetometerScaled scaled = compass.ReadScaledAxis(); //scaled values from compass.
  float heading = atan2(scaled.YAxis, scaled.XAxis);

  // Correct for when signs are reversed.
  if(heading < 0) heading += 2*PI;
  if(heading > 2*PI) heading -= 2*PI;

  return heading * RAD_TO_DEG; //radians to degrees
}
