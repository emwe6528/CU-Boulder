/* ////////////////////////////////





Summary




//////////////////////////////////*/





#include <XBee.h>

XBee xbee = XBee();
#define arraySize  180    // Determines RSSIArray size; Allows received headings to overwrite old ones
#define filterSamples  9           // filterSamples should  be an odd number, no smaller than 3 (<<# sensitive, #<< insensitive)
Rx16Response rx16 = Rx16Response();

float RSSIArray[arraySize];      // array for holding raw RSSI values
int sensSmoothArray [filterSamples];   // holds past RSSI values for filtering
int rawData, smoothData;  // variables for sensor data



void setup() {
  //Initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
}



void loop() {
  Retrieve();      //Retrieves packets and their RSSI values and stores them.
}






/////////////////////////////////////////////////////
////////////////Local Functions//////////////////////
/////////////////////////////////////////////////////




//Receives the transmitted packet and stores the information in RSSIArray.

void Retrieve(){
  xbee.readPacket(50);    //Wait 50 to receive packet
  if (xbee.getResponse().isAvailable())     //Execute only if packet found
  {
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
    {
      xbee.getResponse().getRx16Response(rx16);
      
      //Store the transmitted data and RSSI
      int currentHeading = rx16.getData(0);
      int currentRSSI = abs(rx16.getRssi()-100);

      //Stores the RSSI in RSSIArray. Only executes if the data is within parameters.
      if (currentHeading>=0 && currentHeading<=179){
        Serial.println(millis()/1000);
        Serial.println(currentRSSI);
        smoothData = digitalSmooth(currentRSSI, sensSmoothArray);
        Serial.println(millis()/1000);
        Serial.println(smoothData);
        delay(1);
      }      
    }
  }
}







//Digital filter used to smooth the input data. 

int digitalSmooth(int rawIn, int *sensSmoothArray){     // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
 // static int raw[filterSamples];
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j=0; j<filterSamples; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

/*
  for (j = 0; j < (filterSamples); j++){    // print the array to debug
    Serial.print(sorted[j]); 
    Serial.print("   "); 
  }
  Serial.println();
*/

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1); 
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 
    // Serial.print(sorted[j]); 
    // Serial.print("   "); 
  }

//  Serial.println();
//  Serial.print("average = ");
//  Serial.println(total/k);
  return total / k;    // divide by number of samples
}


          

  
  
  
  
  
  
  
  
  

