#include <XBee.h>
XBee xbee = XBee();
const int Samples = 180;
float RSSIArray[Samples], HeadingArray[Samples];
Rx16Response rx16 = Rx16Response();
int maxRSSI = 0;
int count = 0;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
  Initial();
}

void loop() {
  //Retrieve Samples packets and store the Heading/RSSI Pairs in seperate arrays
  Retrieve();
  int finalHeading = ProcessData();
  count++;
  
  if (count ==35){
    for (int i = 0; i< Samples; i ++)
    {
       RSSIArray[i] = (RSSIArray[(i+2)%180]+RSSIArray[(i+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(i-1+180)%180]+RSSIArray[(i-2+180)%180])/5; 
    }
    count =0;
    Serial.println("AVERAGED");
  }
  
  Serial.println(finalHeading);
}



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


void Initial(){
  for (int i = 0; i< Samples; i ++)
  {
    RSSIArray[i] = 5;
    HeadingArray[i] = i;
  }
  
   for (int i = 1; i < Samples; ++i)
 {
   int j = HeadingArray[i];
   int l = RSSIArray[i];
   int k;
   for (k = i - 1; (k >= 0) && (j < HeadingArray[k]); k--)
   {
     HeadingArray[k + 1] = HeadingArray[k];
     RSSIArray[k + 1] = RSSIArray[k];    
   }
   HeadingArray[k + 1] = j;
   RSSIArray[k + 1] = l;
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
      for (int i = Samples-1; i>0; i--)
      {
        if (HeadingArray[i]==rx16.getData(0) || (HeadingArray[i]>rx16.getData(0) && HeadingArray[i-1]<rx16.getData(0)))
        {
          RSSIArray[i] = (RSSIArray[(i+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(i-1+180)%180])/3; 
          HeadingArray[i] = rx16.getData(0); 
          //Serial.println(HeadingArray[i]); 
          //Serial.println(RSSIArray[i]); 
        }
      }
    }
  }
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
  return HeadingArray[maxIndex];  
}
  
  
  
  
  
  
  
  
  
  

