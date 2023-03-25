//------------------------------------------------------------
#define RELAY A5
#define CURRENT_SENSE A3
#define BUTTON_IN A0
#define BUTTON_OUT A1
#define OVERRUN_MS  10000
#define MIN_AMPS  1.0

//------------------------------------------------------------
int mVperAmp = 100; // use 185 for 5A Module 100 for 20A Module and 66 for 30A Module
uint32_t start_time = 0;

//------------------------------------------------------------
float getVPP()
{
   int maxValue = 0;          // store max value here
   int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis() - start_time) < 100) //sample for 100ms
   {
       int readValue = analogRead(CURRENT_SENSE);
       // see if you have a new maxValue
       if (readValue > maxValue) 
           maxValue = readValue;
       if (readValue < minValue) 
           minValue = readValue;
   }
   
   return ((maxValue - minValue) * 5.0) / 1024.0;
}

//------------------------------------------------------------
float getAmps()
{
  double voltage = getVPP();
  double VRMS = (voltage / 2.0) * 0.707;  //root 2 is 0.707
  double ampsRMS = (VRMS * 1000) / mVperAmp; 

  Serial.println(ampsRMS);
  
  return ampsRMS;
}
//------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY, OUTPUT);
  
  pinMode(BUTTON_OUT, OUTPUT);
  pinMode(BUTTON_IN, INPUT_PULLUP);
  
  digitalWrite(BUTTON_OUT, LOW);
}

//------------------------------------------------------------
void loop() {

  bool powerOn = getAmps() > MIN_AMPS; // 1 amp draw, switch on
  
  if( powerOn )
  {
    delay(150);
    
    powerOn = getAmps() > MIN_AMPS;
    
    if(powerOn)  // if still drawing current
      start_time = millis();
  }
    
  if( start_time > 0 && ( millis() - start_time < OVERRUN_MS ))  // wait for a bit before switching off
    powerOn = true;
  else
    start_time = 0;

  bool buttonPressed = digitalRead(BUTTON_IN) == false;  // get the inverted state of the button
  
  if( buttonPressed ) {
    start_time = 0;
    powerOn = true;
  }
    
  digitalWrite(RELAY, powerOn);
}
