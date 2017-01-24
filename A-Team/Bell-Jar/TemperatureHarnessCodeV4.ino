//Updated for SD Card and RTC 01/04/2017
//Updated for Time and Date stamp 01/24/2017

#include <OneWire.h>
#include <DallasTemperature.h>

//Include Libraries for Clock
#include <SPI.h>
#include <SparkFunDS3234RTC.h>

// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 9

// Define Clock style and Pin
#define PRINT_USA_DATE
#define DS13074_CS_PIN A1 

// include SD library
#include <SD.h>

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

// declare pressure variables
int pressure;
float pressureV;
float psi;

// declare variables regarding the SD card
Sd2Card card;
SdVolume volume;
SdFile root;
File fileType;
String fileName = "BJarLog1.csv";

//const int chipSelect = 8;
   
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  pinMode(10,OUTPUT);
 
  //Necissary for SD shield to work
  //pinMode(10, OUTPUT);
  
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

  //Create SD file
  //Begin SD at CS 8
  SD.begin(8);
  Serial.println(fileName);
  fileType = SD.open(fileName, O_CREAT | O_WRITE);
  fileType.println("Date,Time,Run_Time(ms),Pressure,T0,T1,T2,T3,T4,T5,T6");
  //close file
  fileType.close();
  //Turn off SPI to SD
  digitalWrite(8,HIGH);
  //Begin RTC Set-up
  // Call rtc.begin([cs]) to initialize the library
  // The chip-select pin should be sent as the only parameter
  rtc.begin(DS13074_CS_PIN);
  // set the RTC's clock and date to the compiliers 
  // predefined time.
  rtc.autoTime();
  // Update time/date values
  rtc.update();
/////////////////////////////////////////////////////////////////////////////////////
//initialize starting time for RTC
  rtc.autoTime();
  rtc.update();
  int m,mi,h,d,da,y;
  m = rtc.month();
  da = rtc.date();
  h = rtc.hour();
  d = rtc.day();
  y = rtc.year();
  rtc.setTime(0,0,h,d,da,m,y);
  //////////////////////////////////////////////////////////////////////
  
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
//Function to pull time from RTC 
void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }
  
  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                 String(rtc.month()) + "/"); // Print month
#endif
  Serial.println(String(rtc.year()));        // Print year
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  //Serial.print("Device Address: ");
 // printAddress(deviceAddress);
 // Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}
void SDSave(String Date, String Time, float timer, float PSI){
  //Collect Temperature data
  float TEMP0, TEMP1, TEMP2, TEMP3, TEMP4, TEMP5, TEMP6;
  TEMP0 = sensors.getTempC(Temp0);
  TEMP1 = sensors.getTempC(Temp1);
  TEMP2 = sensors.getTempC(Temp2);
  TEMP3 = sensors.getTempC(Temp3);
  TEMP4 = sensors.getTempC(Temp4);
  TEMP5 = sensors.getTempC(Temp5);
  TEMP6 = sensors.getTempC(Temp6);

  
  //Open SPI bus to SD Card
  digitalWrite(A1,HIGH);
  //Close SPI bus to RTC
  digitalWrite(8,LOW);
  //Open File and save data
  fileType = SD.open(fileName,FILE_WRITE);
  fileType.print(Date);
  fileType.print(" ,");
  fileType.print(Time);
  fileType.print(" ,");
  fileType.print(timer);
  fileType.print(" ,");
  fileType.print(psi);
  fileType.print(" ,");
  fileType.print(TEMP0);
  fileType.print(" ,");
  fileType.print(TEMP1);
  fileType.print(" ,");
  fileType.print(TEMP2);
  fileType.print(" ,");
  fileType.print(TEMP3);
  fileType.print(" ,");
  fileType.print(TEMP4);
  fileType.print(" ,");
  fileType.print(TEMP5);
  fileType.print(" ,");
  fileType.print(TEMP6);
  fileType.println();
  //Close SD file and save
  fileType.close();
  //Close SPI to SD
  digitalWrite(A1,LOW);
  //open SPI to RTC
  digitalWrite(8,HIGH);
  

}
void loop(void)
{ 
  //Time set-up
  digitalWrite(DS13074_CS_PIN,LOW);
  static int8_t lastSecond = -1;
  // Call rtc.update() to update all rtc.seconds(), rtc.minutes(),
  // etc. return functions.
  rtc.update();
//  if (rtc.second() != lastSecond) // If the second has changed
//  {
//    printTime(); // Print the new time
//    
//    lastSecond = rtc.second(); // Update lastSecond value
//  }
//  float timer = (60*rtc.minute())+rtc.second(); //Counts seconds
  int s,m,mi,h,d,da,y;
  s = rtc.second();
  mi = rtc.minute();
  m = rtc.month();
  da = rtc.date();
  h = rtc.hour();
  d = rtc.day();
  y = rtc.year();
 
  String Date = String(m) + "/" + String(da) + "/" + String(y);
  String Time = String(h) + ":" + String(mi) + ":" + String(s);
  
  digitalWrite(DS13074_CS_PIN,HIGH);
  float timer = millis();
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

  //save data to SD Card
  SDSave(Date, Time, timer, psi);
  delay(200);
}
