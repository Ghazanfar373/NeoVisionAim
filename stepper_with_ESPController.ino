#define  dirPin_SteperPan 2
#define  stepPin_StepperPan 15
#define  enPin_StepperPan 4
#define  dirPin_SteperTilt 19
#define  stepPin_StepperTilt 18
#define  enPin_StepperTilt 21
#define  BRACK1 11
#define  BRACK2 10
#define  lsPanRight 32
#define  lsPanLeft  33
#define  lsTiltUp   25
#define  lsTiltDwn  26

int  panValue,tiltValue;
byte recBuffer[17];
int  gimbleMode ;
int panMinLimit  =  1000, panMaxLimit  = 1900, panCenter  = 1500;
int tiltMinLimit =  1000, tiltMaxLimit = 1900, tiltCenter = 1500;
byte pwm_pin_pan = 36;
byte pwm_pin_tilt = 39;
byte pinSbus_Modes = 34;
byte counterMode=1;
byte pinJetson_Modes = 13;
int timeDelay = 500;

void setup() { 
  // put your setup code here, to run once:
  // Declare pins as input:
  pinMode(pinSbus_Modes,INPUT);
  pinMode(pwm_pin_pan, INPUT);
  pinMode(pwm_pin_tilt, INPUT);
  //Declare pins as output:
  pinMode(pinJetson_Modes,OUTPUT);
  pinMode(stepPin_StepperPan, OUTPUT);
  pinMode(dirPin_SteperPan, OUTPUT);
  pinMode(enPin_StepperPan, OUTPUT);
  pinMode(stepPin_StepperTilt, OUTPUT);
  pinMode(dirPin_SteperTilt, OUTPUT);
  pinMode(enPin_StepperTilt, OUTPUT);

  //.................Limit Switches...........
  pinMode(lsPanRight,INPUT);
  pinMode(lsPanLeft,INPUT);
  pinMode(lsTiltUp,INPUT);
  pinMode(lsTiltDwn,INPUT);
  //..........................................
  Serial.begin(9600);
}

//method to cast char array into integer value
int cast(char MyArray[]){
int n;
n = atoi(MyArray);
return n;
 }
 
//Main Loop
void loop() {
// put your main code here, to run repeatedly:
panValue = pulseIn(pwm_pin_pan, HIGH, 25000);
// if(panValue < 1000){panValue = 1000;}  else if(panValue  > 2000){panValue  = 2000;}
tiltValue = pulseIn(pwm_pin_tilt, HIGH, 25000);
// if(tiltValue < 1000){tiltValue = 1000;}  else if(tiltValue  > 2000){tiltValue  = 2000;}
 //..............................................Control Modes................................
gimbleMode = pulseIn(pinSbus_Modes, HIGH, 25000);
////Decision Tree (Modes Control Switch)
if(gimbleMode> 0 && gimbleMode ==1000){
  digitalWrite(pinJetson_Modes,LOW);
}
else if(gimbleMode>0 && gimbleMode>=1500){
  digitalWrite(pinJetson_Modes,HIGH);
}//             Pan    Tilt
//  char s = Serial.read();    //Packet Formation: 'X-0000-Y-0000-Z-1'
//  if(s == 'X') Serial.readBytes(recBuffer,17);
//  char arrayPan[] = {recBuffer[1],recBuffer[2],recBuffer[3],recBuffer[4]};
//  char arrayTilt[] = {recBuffer[8],recBuffer[9],recBuffer[10],recBuffer[11]};
//  //for(int j=0;j<sizeof(arrayPan);j++)Serial.write(arrayPan[j]);
////   Serial.print(cast(arrayPan));xx                                                                                                                                                                                                    
////   Serial.print('\t');
////   Serial.print(cast(arrayPan)-100);
////   Serial.println(cast(arrayTilt));
//   panCenter = 280 ;
//   panMaxLimit = 560;
//   panMinLimit = 10;
//   panValue = cast(arrayPan);
//   tiltCenter = 160;
//   tiltMaxLimit = 320;
//   tiltMinLimit = 10;
//   tiltValue = cast(arrayTilt);
//}
//Serial.flush();
//..............................................................................................
Serial.print(panValue);
Serial.print("   " );
Serial.println(tiltValue);
if(panValue>(panCenter + 80) && panValue<panMaxLimit)
{
  if(digitalRead(lsPanRight)==LOW ){
   
    digitalWrite(enPin_StepperPan, HIGH);
    digitalWrite(dirPin_SteperPan, HIGH);
    digitalWrite(stepPin_StepperPan, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperPan, LOW);
    delayMicroseconds(timeDelay);
   
  }
}
//Data  Saving inside the factory material....
if(panValue<panCenter && panValue>panMinLimit)
  {
   if(digitalRead(lsPanLeft)==LOW ){
    digitalWrite(enPin_StepperPan, HIGH);
    digitalWrite(dirPin_SteperPan, LOW);
    digitalWrite(stepPin_StepperPan, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperPan, LOW);
    delayMicroseconds(timeDelay);
   }
}

if(tiltValue>(tiltCenter + 80) && tiltValue<tiltMaxLimit)
  {
   if(digitalRead(lsTiltUp)==LOW ){
    digitalWrite(enPin_StepperTilt, HIGH);
    digitalWrite(dirPin_SteperTilt, HIGH);
    digitalWrite(stepPin_StepperTilt, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperTilt, LOW);
    delayMicroseconds(timeDelay);
   }
}

//Data  Saving inside the factory material....
if(tiltValue<tiltCenter && tiltValue>tiltMinLimit)
  {
   if(digitalRead(lsTiltDwn)==LOW){
    digitalWrite(enPin_StepperTilt, HIGH);
    digitalWrite(dirPin_SteperTilt, LOW);
    digitalWrite(stepPin_StepperTilt, HIGH);
    delayMicroseconds(timeDelay);
    digitalWrite(stepPin_StepperTilt, LOW);
    delayMicroseconds(timeDelay);
   }
}

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
