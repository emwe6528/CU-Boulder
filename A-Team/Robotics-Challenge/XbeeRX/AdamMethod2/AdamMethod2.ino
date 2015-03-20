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
  //Retrieve Samples packets and store them Heading/RSSI Pairs in seperate arrays
  Retrieve();

  count++;
  
  if (count ==50){
    for (int i = 0; i< Samples; i ++)
    {
       RSSIArray[i] = RSSIArray[(i+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(i-1+180)%180])/3; 
    }
    count =0;
    //Serial.println("AVERAGED");
    int finalHeading = ProcessData();
    Serial.println(finalHeading);
  }
 
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
     
      for (int i = Samples-1; i>0; i--)
      {
        if (rx16.getData(0)>=0 && rx16.getData(0)<=179){
          if (abs(RSSIArray[i]-RSSIArray[(i+1)%180])>5 && abs(RSSIArray[i]-RSSIArray[(i-1+180)%180])>5){
            RSSIArray[i] = (RSSIArray[(i+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(i-1+180)%180])/3; 
          }
          else{RSSIArray[i] = (RSSIArray[(i+1)%180] + abs(rx16.getRssi()-100) + RSSIArray[(i-1+180)%180])/3;}
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
  
  
  
  
  
  
  
  
  
  

