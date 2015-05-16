#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 9

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 9, 3, 2);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress Temp0, Temp1, Temp2, Temp3, Temp4, Temp5, Temp6;

int pressure;
float pressureV;
float psi;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  
  lcd.begin(20, 4);
  
   // set up display
  lcd.print("PSI");
  lcd.setCursor(11, 0);
  lcd.print("T0");
  lcd.setCursor(0, 1);
  lcd.print("T1");
  lcd.setCursor(11, 1);
  lcd.print("T2");
  lcd.setCursor(0, 2);
  lcd.print("T3");
  lcd.setCursor(11, 2);
  lcd.print("T4");
  lcd.setCursor(0, 3);
  lcd.print("T5");
  lcd.setCursor(11, 3);
  lcd.print("T6");

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  // 
  // method 1: by index
  if (!sensors.getAddress(Temp0, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(Temp1, 1)) Serial.println("Unable to find address for Device 1"); 
  if (!sensors.getAddress(Temp2, 2)) Serial.println("Unable to find address for Device 2"); 
  if (!sensors.getAddress(Temp3, 3)) Serial.println("Unable to find address for Device 3"); 
  if (!sensors.getAddress(Temp4, 4)) Serial.println("Unable to find address for Device 4");
  if (!sensors.getAddress(Temp5, 5)) Serial.println("Unable to find address for Device 5");
  if (!sensors.getAddress(Temp6, 6)) Serial.println("Unable to find address for Device 6");  

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them.  It might be a good idea to 
  // check the CRC to make sure you didn't get garbage.  The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  // show the addresses we found on the bus
  /*
  Serial.print("Device 0 Address: ");
  printAddress(Temp0);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(Temp1);
  Serial.println();
  
  Serial.print("Device 2 Address: ");
  printAddress(Temp2);
  Serial.println();
  
  Serial.print("Device 3 Address: ");
  printAddress(Temp3);
  Serial.println();
  
  Serial.print("Device 4 Address: ");
  printAddress(Temp4);
  Serial.println();
  
  Serial.print("Device 5 Address: ");
  printAddress(Temp5);
  Serial.println();
  
  Serial.print("Device 6 Address: ");
  printAddress(Temp6);
  Serial.println();
  */

  // set the resolution to 9 bit
  sensors.setResolution(Temp0, TEMPERATURE_PRECISION);
  sensors.setResolution(Temp1, TEMPERATURE_PRECISION);
  sensors.setResolution(Temp2, TEMPERATURE_PRECISION);
  sensors.setResolution(Temp3, TEMPERATURE_PRECISION);
  sensors.setResolution(Temp4, TEMPERATURE_PRECISION);
  sensors.setResolution(Temp5, TEMPERATURE_PRECISION);
  sensors.setResolution(Temp6, TEMPERATURE_PRECISION);

  /*Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(Temp0), DEC); 
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(Temp1), DEC); 
  Serial.println();
  
  Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(Temp2), DEC); 
  Serial.println();
  
  Serial.print("Device 3 Resolution: ");
  Serial.print(sensors.getResolution(Temp3), DEC); 
  Serial.println();
  
  Serial.print("Device 4 Resolution: ");
  Serial.print(sensors.getResolution(Temp4), DEC); 
  Serial.println();
  
  Serial.print("Device 5 Resolution: ");
  Serial.print(sensors.getResolution(Temp5), DEC); 
  Serial.println();
  
  Serial.print("Device 6 Resolution: ");
  Serial.print(sensors.getResolution(Temp6), DEC); 
  Serial.println();
  */
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  //Serial.print(" Temp F: ");
  //Serial.print(DallasTemperature::toFahrenheit(tempC));
  
}

void printTemperatureLCD(DeviceAddress deviceAddress, int col, int row)
{
  
  float tempC = sensors.getTempC(deviceAddress);
  lcd.setCursor(col,row);
  lcd.print(tempC);
}

/*
// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}
*/

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  //Serial.print("Device Address: ");
 // printAddress(deviceAddress);
 // Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{ 
  // get pressure
  pressure = analogRead(A4);
  pressureV = pressure*(5.0/1023);
  psi = (pressureV-0.5)*(15.0/4.0);
  
  // print pressure
  Serial.print("PSI ");
  Serial.println(psi);
  lcd.setCursor(4,0);
  lcd.print(psi);
  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  printData(Temp0);
  printData(Temp1);
  printData(Temp2);
  printData(Temp3);
  printData(Temp4);
  printData(Temp5);
  printData(Temp6);
  
  printTemperatureLCD(Temp0,14,0);
  printTemperatureLCD(Temp1,4,1);
  printTemperatureLCD(Temp2,14,1);
  printTemperatureLCD(Temp3,4,2);
  printTemperatureLCD(Temp4,14,2);
  printTemperatureLCD(Temp5,4,3);
  printTemperatureLCD(Temp6,14,3);
}

