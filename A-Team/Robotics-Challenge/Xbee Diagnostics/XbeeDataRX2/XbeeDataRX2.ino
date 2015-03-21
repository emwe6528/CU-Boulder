#include <XBee.h>
XBee xbee = XBee();
const int Samples = 180;
float RSSIArray[Samples], HeadingArray[Samples];
Rx16Response rx16 = Rx16Response();
int maxRSSI = 0;
int count = 0;

#define filterSamples   3              // filterSamples should  be an odd number, no smaller than 3
int sensSmoothArray1 [filterSamples];   // array for holding raw sensor values for sensor1 
int rawData1, smoothData1;  // variables for sensor1 data





int sensVal;           // for raw sensor values 
float filterVal = .0001;       // this determines smoothness  - .0001 is max  1 is off (no smoothing)
float smoothedVal;     // this holds the last loop value just use a unique variable for every different sensor that needs smoothing



void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
  Initial();
}

void loop() {
  //Retrieve Samples packets and store them Heading/RSSI Pairs in seperate arrays
  Retrieve();
    int finalHeading = ProcessData();
  //  Serial.println(finalHeading);
    
    
//  count++;
//  
//  if (count ==50){
//    for (int i = 0; i< Samples; i ++)
//    {
//       RSSIArray[i] = (RSSIArray[(i+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(i-1+180)%180])/3; 
//    }
//    count =0;
//    //Serial.println("AVERAGED");
//    int finalHeading = ProcessData();
//    //Serial.println(finalHeading);
//  }
// 
}



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

//Initialize Headings
//Heading array not even necessary
void Initial(){
  for (int i = 0; i< Samples; i ++)
  {
    RSSIArray[i] = 5;
    HeadingArray[i] = i;
  }
}




void Retrieve(){
  delay(100);
  xbee.readPacket(50);
  if (xbee.getResponse().isAvailable())
  {
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
    {
      xbee.getResponse().getRx16Response(rx16);
     int Heading = rx16.getData(0);
      
        if (Heading>=0 && Heading<=179){
        
        
          //Outlier Eliminator
//          for (int i=0;i<Samples;i++){
//            if (abs(RSSIArray[i]-RSSIArray[(i+1)%180])>10 && abs(RSSIArray[i]-RSSIArray[(i-1+180)%180])>10){
//              RSSIArray[i] = (RSSIArray[(i+1)%180] + RSSIArray[(i-1+180)%180])/2; 
//            }
//          }
          
          
          //Adjacent Averaging
          //RSSIArray[Heading] = (RSSIArray[(Heading+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(Heading-1+180)%180])/3;
          
          
          //No Adjustment
          //RSSIArray[Heading] = abs(rx16.getRssi()-100)
          
          
          
          //Low Pass Digital Filter
          //smoothedVal = smooth(abs(rx16.getRssi()-100), filterVal, smoothedVal);
          //RSSIArray[Heading] = smoothedVal;
          
          
          
          //Another Low Pass Digital Filter
           smoothData1 = digitalSmooth(abs(rx16.getRssi()-100), sensSmoothArray1);
           RSSIArray[Heading] = smoothData1;
          
          Serial.println(Heading);
          Serial.println(abs(rx16.getRssi()-100));   
          Serial.println(RSSIArray[Heading]);
        }      
    }
  }
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

         
          
int smooth(int data, float filterVal, float smoothedVal){
  if (filterVal > 1){      // check to make sure param's are within range
    filterVal = .99;
  }
  else if (filterVal <= 0){
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}
          



int ProcessData(){
  int maxIndex = 0;
  maxRSSI = RSSIArray[maxIndex];
  for (int i=1; i<Samples; i++)
  {
    if (maxRSSI<RSSIArray[i])
    {
      maxRSSI = RSSIArray[i];
      maxIndex = i;
    }
  }
  return maxIndex;  
}
  
  
  
  
  
  
  
  
  
  

