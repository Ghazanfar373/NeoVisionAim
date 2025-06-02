#define  dirPin_SteperPan    14
#define  stepPin_StepperPan  13
#define  enPin_StepperPan    12
#define  dirPin_SteperTilt    2
#define  stepPin_StepperTilt 15
#define  enPin_StepperTilt    4
#define  BrakePan   18//13
#define  BrakeTilt  19//12
#define  lsPanRight 32
#define  lsPanLeft  33
#define  lsTiltUp   26
#define  lsTiltDwn  25
    volatile int  panValue,tiltValue;
    byte          recBuffer[5];
    volatile int  gimbleMode ;
    volatile int  panMinLimit  =  1000, panMaxLimit  = 1900, panCenter  = 1500;
    int  tiltMinLimit =  1000, tiltMaxLimit = 1900, tiltCenter = 1500;
    const byte pwm_pin_pan   =   35;
    const byte pwm_pin_tilt  =   34;
    const byte pinSbus_Modes =   23;
    byte counterMode=1;
    const byte pinJetson_Modes=21; 
    int  timeDelay = 500;
    int pwmCycles = 5;
    boolean flagAutoMode = false;
void setup() { 
    //put your setup code here, to run once:
    //Declare pins as input:
    pinMode(pinSbus_Modes,INPUT);
    pinMode(pwm_pin_pan, INPUT);
    pinMode(pwm_pin_tilt, INPUT);
    //======================Pin OUTPUT Tilt====================================
    pinMode(pinJetson_Modes,OUTPUT);
    pinMode(stepPin_StepperTilt, OUTPUT);
    pinMode(dirPin_SteperTilt, OUTPUT);
    pinMode(enPin_StepperTilt, OUTPUT);
     //======================Pin OUTPUT Pan====================================
    pinMode(stepPin_StepperPan, OUTPUT);
    pinMode(dirPin_SteperPan, OUTPUT);
    pinMode(enPin_StepperPan, OUTPUT);
    //====================Limit Switches==============================
    pinMode(lsPanRight,INPUT);
    pinMode(lsPanLeft,INPUT);
    pinMode(lsTiltUp,INPUT);
    pinMode(lsTiltDwn,INPUT);
    //=======================Brakes====================================
    pinMode(BrakeTilt, OUTPUT);
    pinMode(BrakePan, OUTPUT);
    Serial.begin(115200);
  }
//Main Loop
void loop() {
      // pinMode(interruptPin, INPUT_PULLUP);
      // attachInterrupt(digitalPinToInterrupt(interruptPin), drive, FALLING);
     gimbleMode = pulseIn(pinSbus_Modes, HIGH, 25000);
     if( gimbleMode <= 1000 && gimbleMode>0){ digitalWrite(pinJetson_Modes,LOW); flagAutoMode = false;}  else if(gimbleMode>=1450){ digitalWrite(pinJetson_Modes,HIGH); flagAutoMode = true;}
  flagAutoMode = true;
 //Decision Tree (Modes Control Switch)
    switch(flagAutoMode){
      
      case HIGH:
          autoMode();
          break;
      case LOW:
          manualMode();
          break;
    
    }
    //Serial.println(panValue);
    //Serial.print("   " );
    //Serial.println(tiltValue);
    //energizeGimble();
//   unsigned long timeEnd = micros();
//   unsigned long duration = timeEnd - timeBegin;
//   double averageDuration = (double)duration / 1000.0;
//   Serial.print("Elapsed Time (milli): ");
     Serial.print("Gimble Mode: ");
     Serial.println(gimbleMode);
    
}
//==============================================================================================
void energizeGimble(){
  if(panValue>(panCenter + 80) && panValue<panMaxLimit)
  {
   if(digitalRead(lsPanRight)==LOW ){
    //Serial.println("Inside Pan Max");
    digitalWrite(BrakePan, HIGH);
    digitalWrite(enPin_StepperPan, HIGH);
    digitalWrite(dirPin_SteperPan, HIGH);
     for(int i=0;i<pwmCycles;i++){
    digitalWrite(stepPin_StepperPan, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperPan, LOW);
    delayMicroseconds(timeDelay);
     }
  }
}
if(panValue<panCenter && panValue>panMinLimit)
  {
   if(digitalRead(lsPanLeft)==LOW ){
    //Serial.println("Inside Pan Min");
    digitalWrite(BrakePan, HIGH);
    digitalWrite(enPin_StepperPan, HIGH);
    digitalWrite(dirPin_SteperPan, LOW);
     for(int i=0;i<pwmCycles;i++){
    digitalWrite(stepPin_StepperPan, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperPan, LOW);
    delayMicroseconds(timeDelay);
     }
   }
}                                                                                   
if(tiltValue>(tiltCenter + 80) && tiltValue<tiltMaxLimit)
  {
   if(digitalRead(lsTiltUp)==LOW ){
    //Serial.println("Inside Tilt Max");
    digitalWrite(BrakeTilt, HIGH);
    digitalWrite(enPin_StepperTilt, HIGH);
    digitalWrite(dirPin_SteperTilt, HIGH);
    for(int i=0;i<pwmCycles;i++){
    digitalWrite(stepPin_StepperTilt, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperTilt, LOW);
    delayMicroseconds(timeDelay);
   }
}
  }
if(tiltValue<tiltCenter && tiltValue>tiltMinLimit)
  {
   if(digitalRead(lsTiltDwn)==LOW){
    //Serial.println("Inside Tilt Min");
    digitalWrite(BrakeTilt, HIGH); 
    digitalWrite(enPin_StepperTilt, HIGH);
    digitalWrite(dirPin_SteperTilt, LOW);
    for(int i=0;i<pwmCycles;i++){
    digitalWrite(stepPin_StepperTilt, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperTilt, LOW);
    delayMicroseconds(timeDelay);
    }
   }
}
//if(tiltValue > tiltCenter && tiltValue <= tiltCenter+80) digitalWrite(BrakeTilt, LOW);
//if(panValue > panCenter && panValue <= panCenter+80) digitalWrite(BrakePan, LOW);

}

//===========================================Control Modes===========================
void autoMode( ){                           //Pan Tilt
  recBuffer[0] = Serial.read();    //Packet Formation: 'X-0000-Y-0000-Z-1'
  if(recBuffer[0] == 0x4d) Serial.readBytes(recBuffer,4);
  //char  arrayPan[] = {recBuffer[1],recBuffer[2],recBuffer[3],recBuffer[4]};
  //char arrayTilt[] = {recBuffer[8],recBuffer[9],recBuffer[10],recBuffer[11]};
  for(int j=0;j<sizeof(recBuffer);j++)Serial.write(recBuffer[j]);
  //Serial.print(cast(arrayPan));xx                                                                                                                                                                                                    
  //Serial.print('\t');
  //Serial.print(cast(arrayPan)-100);
  //Serial.println(cast(arrayTilt));
   panCenter   =   400;  //280;    
   panMaxLimit =   800;     //560;
   panMinLimit =   10;
   //panValue = cast(arrayPan);
   tiltCenter   =  300;   //160;        
   tiltMaxLimit =  600;   //320;
   tiltMinLimit =  10;
   //tiltValue = cast(arrayTilt);
   Serial.flush();
  // energizeGimble();
  }
  //Delta wing inisde tera wing inside tetra wing
void manualMode(){
//   panCenter = 1500 ;
//   panMaxLimit = 1900;
//   panMinLimit = 1000;
//   tiltCenter = 1500;        
//   tiltMaxLimit = 1900;
//   tiltMinLimit = 1000;
  panValue   = pulseIn(pwm_pin_tilt, HIGH, 25000);
 if(panValue < 1000){panValue = 1000;}  else if(panValue  > 2000){panValue  = 2000;}
 tiltValue  = pulseIn(pwm_pin_pan, HIGH, 25000);
 if(tiltValue < 1000){tiltValue = 1000;}  else if(tiltValue  > 2000){tiltValue  = 2000;}
//0.print(tiltValue);
//Serial.print("\t");
//Serial.print(gimbleMode);
//Serial.print("\t");
//Serial.println(panValue);             
energizeGimble();
Serial.flush();
  }  

//method to cast char array into integer value
int cast(char MyArray[]){
int n;
n = atoi(MyArray);
return n;
 }
  
void  readFromController(){
 if(Serial.available()){
noInterrupts();
String s = Serial.readStringUntil('Z');
String xval = s.substring(2,6);
String yval = s.substring(9,14);
//midObjectY=yval.toInt();
//midObjectX=xval.toInt();
//controlServos();
 Serial.flush();
  delay(3);
  }
}
