/* ////////////////////////////////





Summary




//////////////////////////////////*/



#include <XBee.h>
XBee xbee = XBee();


// filterSamples should  be an odd number, no smaller than 3 (<<# sensitive, #<< insensitive)
#define filterSamples  7          
#define filterSamples2  17

// holds past RSSI values for filtering
int sensSmoothArray [filterSamples]; 
int sensSmoothArray2 [filterSamples2];

// variables for sensor data
int rawData, smoothData;  
int rawData2, smoothData2;  
int currentHeading = 0, currentRSSI = 0;





void setup() {
  Serial.begin(57600); 
  Serial1.begin(57600);
  xbee.setSerial(Serial1);
}




void loop() {
  
    Retrieve();
    
    // Serial.println(currentHeading);
    Serial.println(currentRSSI);
    
    smoothData = digitalSmooth(currentRSSI, sensSmoothArray);
    // Serial.println(currentHeading);
    Serial.println(smoothData);
    
    smoothData2 = digitalSmooth2(currentRSSI, sensSmoothArray2);
    //Serial.println(currentHeading);
    Serial.println(smoothData2);

    
    delay(100);  // Gives Matlab time to process data
}




/////////////////////////////////////////////////////
////////////////Local Functions//////////////////////
/////////////////////////////////////////////////////

// Receive transmitted data and store it

void Retrieve(){
  xbee.readPacket(50);    //Wait 50 to receive packet
  if (xbee.getResponse().isAvailable())     //Execute only if packet found
  {
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
    {
      xbee.getResponse().getRx16Response(rx16);
      
      //Store the transmitted data and RSSI
      currentHeading = rx16.getData(0);
      currentRSSI = abs(rx16.getRssi()-100);     
    }
  }
}




/* digitalSmooth
 Paul Badger 2007
 A digital smoothing filter for smoothing sensor jitter 
 This filter accepts one new piece of data each time through a loop, which the 
 filter inputs into a rolling array, replacing the oldest data with the latest reading.
 The array is then transferred to another array, and that array is sorted from low to high. 
 Then the highest and lowest %15 of samples are thrown out. The remaining data is averaged
 and the result is returned.

 Every sensor used with the digitalSmooth function needs to have its own array to hold 
 the raw sensor values. This array is then passed to the function, for it's use.
 This is done with the name of the array associated with the particular sensor.
 */

int digitalSmooth2(int rawIn, int *sensSmoothArray2){     // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
 // static int raw[filterSamples];
  static int sorted[filterSamples2];
  boolean done;

  i = (i + 1) % filterSamples2;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray2[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j=0; j<filterSamples2; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray2[j];
  }

  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples2 - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples2 * 15)  / 100), 1); 
  top = min((((filterSamples2 * 85) / 100) + 1  ), (filterSamples2 - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 

  }
  return total / k;    // divide by number of samples
}



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

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1); 
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 

  }
  return total / k;    // divide by number of samples
}


