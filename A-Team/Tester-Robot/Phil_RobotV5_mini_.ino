//Code for *Name To Be Choosen* Robot. Hereafter referred to as, NT-BC Autonomous Navigator (mini). * mini version has no servos
//A product of the Galactic Empire.
//=============================================================================================================================
//Ordered by:
//the Lord of the Sith (Philip Baranowski)
//Created by:
//apprehensive StormTrooper TH-3108(Thomas Horning)
//Original: 03/18/2016
//Mini Version: 12/17/2016
//Edits by: Thomas Horning (12/17/2016)
//
//
//=============================================================================================================================
//This code takes a compass (I2C over Analog 4 and 5), IR sensor, and Beacon (Fio v3 + XBee S1) as input.
//Motor direction-Motor Power (Ardumoto Shield), and Servo (90 degree limit **Not included in mini), as output.
//It begins by checking for a value to be sent back from the FioV3 board that as recieved a compass heading
//through RF and converted it to the heading towards the beacon.
//This is a serial communication over Digital 0. NT-BC then checks its own compass heading from I2C communication with a compass 
//communicating over Analog pins 4 and 5. It then checks its IR sensor input through Analog Pin 0. The Average of a 
//specified sum of these readings is returned. NT-BC becomes clever next(It has bonded with it's creator Phil and learned
//the secrets of humanity). NT-BC may experience bad terrain, killer aliens, or Rebel scum.
//In the Event NT-BC comes into contact with said object in its motion, it will stop and turn its sensors to check its surroundings.
//These values are used to determine how it should turn. In the seemingly low possibility NT-BC is not in emminent danger, it will
//try and find it's way home. Home being a centrally mounted transmitting Radio Frequency antenna and transmitter processing unit
//on a rotary system for optimal lighthouse transmission (Yeah, its a box on a spinning stick). After Deep reflection NT-BC will 
//have discovered the direction it should turn to match the heading needed to reach home. It will continue turning in that 
//direction until it is within a + or - 10 degree tolerance. Once all is said in done, when there are no more Predators to avoid
//and when its path seems clear, like the Hobbits returning to the Shire NT-BC shall march forward towards home.
//=============================================================================================================================== 

//Code commented out is for full version

//#include <Servo.h>//Servo Library
#include <HMC5883L.h>//Compass Library
#include <Wire.h> //I2C Arduino Library

#define address 0x1E//For communication
//Servo Library activation
//Servo myservo;
//Motor Setup
int pwm_a = 3; //Motor side A Power
int pwm_b = 11; //Motor side B power
int dir_a = 12; //Motor side A Direction
int dir_b = 13;//Motor side B Direction
//Sensor Setup
int IRsensorPin = A0;// Define the input pin for the IR sensor
int IRsensorValue; //Forward View
//int IRsensorValue1; //Right  View
//int IRsensorValue2; //Left View
float sensorPool; //Pool to hold the sum of the average sensor value
float sensorAverage; //The averaged sensor value
//Beacon-Compass Setup
float BeaconDir; //Initial Beacon Direction
float BeaconDir1; //Value to assign 360 or 0
float BeaconDir2;
float MyHeading; //Current reading from compass
//===================================================================
//Setup
//===================================================================
void setup() {
  //Activate Digital Pin modes
  pinMode(pwm_a, OUTPUT);//Set up motor pins
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  //myservo.attach(9); //Set up Servo pin
  pinMode(IRsensorPin, INPUT);   // Set up IR sensor pin
  Serial.begin(57600); //Serial communication speed set up
  Wire.begin(); //I2C setup
  //Put the Compass IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with Compass
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission(); //finish setup
}


void loop() {
  //Find my Compass Heading 
  BeaconCheck();//checks if Beacon data has been recieved 
  //(yes, there is a delay between checks I'm working on mitigating any effects if they appear)
  /* For Testing
  delay(1000);
  Serial.print("Needed Heading: ");
  Serial.println(BeaconDir1);
  Serial.println(BeaconDir2);
  */
  
  //Find my Heading 
  MyHeading = myHeading(); //Pull compass data
  /* For Testing
  Serial.print("My Heading: ");
  Serial.println(MyHeading);
  */
  
  //Check Sensor
  //myservo.write(78); //Set sensor Forward
  //delay(25); 
  //Arguement is number of sensorchecks to average with a specified delay for sensor clarity (Count length, delay time)
   sensorReading(10, 10);
  
  // Obstacle Detected
  while(sensorAverage > 125) { //while the sensor reads greater than 125 (something close)
      avoidance2(); //Avoid direct obstacle (Wake-up NT-BC Its time to think)
      sensorReading(50, 50);  //Now that that is over, recheck your sensors because that is SMRT  
      Serial.println("IR Reading: ");
      Serial.println(sensorAverage);  
  }
  //Assuming we arent staring down a brick wall let's face the right way
  //Correct Heading ****Need to move to function
  if(MyHeading > (BeaconDir2 + 10.0) and MyHeading < (BeaconDir1 - 10.0)) { //if NT-BC is in a tolerance of + or - 10 degrees from the Beacon reading
    //Correct Heading
    Serial.println("Correcting Heading");
    // if NT-BC's compass heading is less than the heading towards the beacon and are counterclockwise to its location turn right
    while(MyHeading < (BeaconDir1 - 10.0) and MyHeading > (BeaconDir1 - 180.0)) {
      rightTurn(50); //turn right until within tolerance
      BeaconCheck(); //Recheck the location of the beacon
      MyHeading = myHeading(); // Recheck the current heading in order to escape the loop of turning
    }
    //Guess what this other one does... That's right, it turns Left!!! Muwahahahaha 
    while(MyHeading > (BeaconDir2 + 10.0) and MyHeading < 180.0){ //Second Verse, Same as the First!... but Left
      leftTurn(50);
      BeaconCheck();
      MyHeading = myHeading();
    }
  }

  //Move Forward
  else { 
      forward(100); //Forward motion
    }
    delay(100); // Over-all code rest delay (Reaction Time) **Needs to be tested for refinement
  }
  
// The moment you have been waiting for!

/*=============================================================================
//Functions
=============================================================================*/ 

// Check Serial for Beacon data (an integer heading)
float BeaconCheck() {
  while(Serial.available()) { //While data is available
    BeaconDir1 = Serial.read(); //Read Serial data
    //Serial.println(BeaconDir1);
    if(BeaconDir1 == 0) { // Zero makes math hard, nothing is no fun anyway
      BeaconDir1 = 360.0; //So he have a workable number for some of the math
      BeaconDir2 = 0;
    }
    else { // otherwise its a good angle for mathing
      BeaconDir2 = BeaconDir1;
    }
    return BeaconDir1; //NT-BC sends its decision back to itself (Codeception)
    return BeaconDir2;
    //delay(1000); //For Testing
  }
}

//Recieve and determine NT-BC current heading
float myHeading() {
  int x,y,z; // integer letters?... think about that for a while
  Wire.beginTransmission(address); //beginTransmission.... kinda on the nose.
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
  Wire.requestFrom(address, 6); //Read data from each axis, 2 registers per axis
  if(6<=Wire.available()){ // If 6 packets of data come in
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }
  float heading = atan2(y,x); // Take the ArcTangent of y and x componenets 
  if(heading < 0) //Zeros being bad 
      heading += 2*PI; //Adds to pi to allow > 180 readings 
  float headingDegrees = heading * 180/M_PI; //Always know what mode your in, NT-BC lives Degrees
  //Serial.print("Theta: "); //For Testing
  //Serial.println(headingDegrees);
  return headingDegrees; //Codeception
}

// Get the average sensor value out of x counts. Delay needed for IR clarity.
float sensorReading(int x, int sec) {
   for(int i = 0; i < x; i++) { //The amount of sums of sensor values
    IRsensorValue = analogRead(IRsensorPin); //Get Sensor value
    sensorPool += IRsensorValue; //Add to sensor Sum
    delay(sec); //Delay for clarity
    /* For Testing
    Serial.println("IR Reading");
    Serial.println(IRsensorValue);
    */
  }
  sensorAverage = sensorPool/x; //Divide by summation number to get average
  return sensorAverage;//Codeception
  /* For Testing  
  Serial.println("IR Reading: ");
  Serial.println(sensorAverage);
  */
}

//Guess what this does
void forward(int sec) { 
      Serial.println("Moving forward");
      digitalWrite(dir_a, LOW); //Dir_a = Right
      digitalWrite(dir_b, HIGH); //Dir_b = Left
     //Set both motors to run at 100%
      analogWrite(pwm_a, 255); //Right
      analogWrite(pwm_b, 255); //Left
      delay(sec);
}

//Yup
void rightTurn(int sec) {
          Serial.println("Turning Right");
          digitalWrite(dir_a, LOW);  //Dir_a = Right 
          digitalWrite(dir_b, LOW);  //Dir_b = Left
    
          analogWrite(pwm_a, 255);  //set both motors to run at 100%
          analogWrite(pwm_b, 255);
          delay(sec);
          analogWrite(pwm_a, 0);  //stop motors
          analogWrite(pwm_b, 0);
          delay(500);
          
}

//That obvious
void leftTurn(int sec) {
          Serial.print("Turning Left");
          digitalWrite(dir_a, HIGH);  //Dir_a = Right
          digitalWrite(dir_b, HIGH);  //Dir_b = Left

          analogWrite(pwm_a, 255);  //set both motors to run at 100% duty cycle (fast)
          analogWrite(pwm_b, 255);
          delay(sec);
          analogWrite(pwm_a, 0);  //set both motors to run at 0% duty cycle (stop)
          analogWrite(pwm_b, 0);
          delay(500);   
}

//Yes... That does say Back Turn
void backTurn(int sec) {
          Serial.println("Backing Up");
          digitalWrite(dir_a, HIGH);  //Dir_a = Right
          digitalWrite(dir_b, LOW);   //Dir_b = Left
    
          analogWrite(pwm_a, 255);  //set both motors to run at 100%
          analogWrite(pwm_b, 255);
          delay(sec);
          analogWrite(pwm_a, 0);  //Stop motors
          analogWrite(pwm_b, 0);
          delay(800);
}

//Avoidance manuevuers... maneveurs?....manuvuers?....
//Avoidance Movement Managment Code Block.... 1....
/*void avoidance1() {
      //Back turn to break (stop forward momentum)
      backTurn(100);
      sensorReading(10, 10);
      myservo.write(5); //Turn Servo Right
      delay(500); 
      IRsensorValue1 = analogRead(IRsensorPin);  //Find out if there is anything in the way 
      //Serial.print("Right: ");
      //Serial.print(IRsensorValue1);
     
      myservo.write(150); //Turn Servo Left
      delay(1000); 
      IRsensorValue2 = analogRead(IRsensorPin);  //Find out if there is anything in the way 
      //Serial.print("Left: ");
      //Serial.print(IRsensorValue2);
    
      myservo.write(78); //Turn Servo Forward
      delay(1000);
    
      if(IRsensorValue1 > 90 and IRsensorValue2 > 90) //If Both sides are blocked
        {
          //It's a Trap! Get out of there!
          backTurn(800);
          rightTurn(400);  
        }
          
       else if(IRsensorValue1 > 90 and IRsensorValue2 < 90) //If Right Side is blocked and Left Side is clear
        {
          //Turn Left
          leftTurn(400);    
        }
        
       else if(IRsensorValue1 < 90 and IRsensorValue2 > 90) //If Left is blocked and Right is clear
        {
          //Turn Right
          rightTurn(400);
        }
       else //If both sides clear (Place holder code for randomizer)
        {
          
          int RN = random(0,2);
          switch(RN){
            case 0:
              rightTurn(400);
            case 1:
              leftTurn(400);
          }
       }
}
*/
//Avoidance Movement Managment Code Block 2
//This avoidance block is for the mini version (No Servo)
void avoidance2() {
      int SA1 = 0;
      //Back turn to break (stop forward momentum)
      backTurn(100);
    
      while(sensorAverage > 90) //If initial backwards movement is insufficent
        {
          //It's still following us!
          backTurn(100);
          sensorReading(50, 50);
          //Find out if there is anything in the way 
          //Serial.print("Blockage: ");
          //Serial.print(sensorAverage);
        }
          
       if(sensorAverage <= 90) //If obstacle has been cleared
        {
         //Choose random turn direction
          int RN = random(0,2);
          switch(RN){
            case 0:
              rightTurn(400);
              SA1 = 1;
            case 1:
              leftTurn(400); 
              SA1 = 2; 
        }
        forward(100); //Move forward to avoid impending doom
        //Turn back to original "forward"
        switch(SA1){
          case 1:
          leftTurn(400);
          case 2:
          rightTurn(400);
        }
      }
}
/////////////////////////////////////////////////////////////END////////////////////////////////////////////////For Now...
