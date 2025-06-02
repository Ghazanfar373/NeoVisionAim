// Stepper Motor
// Michael Klements
// The DIY Life
// 25/01/2017

int motorPin = 3;        //Assign pin numbers
int dirPin = 2;
int motorDir = 0;        //Assign the initial motor direction
int motorSpe = 10;      //Assign the motor speed, a smaller number is a faster speed (shorter delay between pulses)
int rotation = 100;     //Assign the motor rotation in pulses


int lastValue=0;
int lastValueLB=0;
int rotateVal=0;
int val=0;
char incomingByte = 0;
int error= 0;
void setup()
{
  pinMode(motorPin, OUTPUT);  //Assign Pins
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{

if(Serial.available() > 0){
incomingByte = Serial.read();
val = incomingByte - '0';
//char val = Serial.read();
Serial.println(val);
cmndJoyStick(val*10);
delay(1);
Serial.flush();
}

//    if(motorDir==0)    //Set the motor direction
//    {
//      digitalWrite(dirPin, LOW);
//    }
//    else
//    {
//      digitalWrite(dirPin, HIGH);
//    }
//    for(int j = 0 ; j <= rotation ; j++)  //Run the motor to the input rotation at the input speed.
//    {
//      digitalWrite(motorPin, HIGH);
//      delay(motorSpe);
//      digitalWrite(motorPin, LOW);
//    }
//    if(motorDir==0)    //Change the motor direction at the end of the rotation travelled
//    {
//      motorDir=1;
//    }
//    else
//    {
//      motorDir=0;
//    }
    
}


void cmndJoyStick(int val){
 
  if(val>100 && val<200){
    if(val>lastValue){
      error=val-lastValue;
       digitalWrite(dirPin,HIGH);
    for(int i=0 ; i <= error ; i++){
      digitalWrite(motorPin, HIGH);
      delayMicroseconds(5000);
      digitalWrite(motorPin, LOW);
      }
    }else if(val<lastValue){
      error=lastValue-val;
      digitalWrite(dirPin,LOW);
      for(int i=0 ; i <= error ; i++){
    
      digitalWrite(motorPin, HIGH);
      delayMicroseconds(5000);
      digitalWrite(motorPin, LOW);
      }
      }
      lastValue=val;
    }
    if(val<100 && val>0){
      if(val>lastValueLB){
      error=val-lastValueLB;
       digitalWrite(dirPin,HIGH);
    for(int i=0 ; i <= error ; i++){
      digitalWrite(motorPin, HIGH);
      delayMicroseconds(5000);
      digitalWrite(motorPin, LOW);
      }
    }else if(val<lastValueLB){
      error=lastValueLB-val;
      digitalWrite(dirPin,LOW);
      for(int i=0 ; i <= error ; i++){
    
      digitalWrite(motorPin, HIGH);
      delayMicroseconds(5000);
      digitalWrite(motorPin, LOW);
      }
      }
  lastValueLB = val;
    }
  
  
  }
