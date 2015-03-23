
#define Samples = 180;
float DataArray[Samples], XArray[Samples];

#define filterSamples  9           // filterSamples should  be an odd number, no smaller than 3
int sensSmoothArray1 [filterSamples];   // array for holding raw sensor values for sensor1 
int rawData1, smoothData1;  // variables for sensor1 data



void setup() {
  Serial.begin(9600);     // initialize serial communications at 9600 bps:
}

void loop() {
  //Retrieve data and store in XArray/DataArray
  for(int i = 0;i<150;i++) Retrieve();
  
  
  //Optional. Pass entire DataArray through Filter 
  for(int i = 0;i<Samples;i++){
           while (RSSIArray[i] == 5 && i <Samples) i++;    
           smoothData1 = digitalSmooth(RSSIArray[i], sensSmoothArray1);
           RSSIArray[i] = smoothData1;
  }
}



/////////////////////////////////////////////////////
///////////////Local Functions///////////////////////
/////////////////////////////////////////////////////






void Retrieve(){
   int Heading = rx16.getData(0);
  
  //Set Parameters for Data
  if (Heading>=0 && Heading<=179){
    
    //No Adjustment
    RSSIArray[Heading] = abs(rx16.getRssi()-100);  
    
    //Another Low Pass Digital Filter
    //           smoothData1 = digitalSmooth(abs(rx16.getRssi()-100), sensSmoothArray1);
    //           RSSIArray[Heading] = smoothData1;
     
    //          Serial.println(Heading);
    //          Serial.println(abs(rx16.getRssi()-100));   
    //          Serial.println(RSSIArray[Heading]);
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
  int counter = 0;
  maxRSSI = RSSIArray[maxIndex];
  for (int i=1; i<Samples; i++)
  {
    if (maxRSSI<RSSIArray[i]) maxRSSI = RSSIArray[i];
  }
  for (int i=1; i<Samples; i++){
    if ((maxRSSI-3)<=RSSIArray[i])
    {
      maxIndex += i;
      counter++;
    }
  }
  return maxIndex/counter;  
}
  
  
  
  
  
  
  
  
  
  

