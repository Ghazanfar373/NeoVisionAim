#include<Servo.h>
String buffer;
//VarSpeedServo servo1,servo2;
Servo servo1,servo2;
String substr[6];
String xval,yval;
int count=0;
void controlServos();
String output ="X-135-Y-135-Z";
#define MAX_STRING_LEN  12
int servoVal1, servoVal2;
//These variables hold the x and y location for the middle of the detected object.
int midObjectY=0;
int midObjectX=0;
//Variables for keeping track of the current servo positions.
char servoTiltPosition = 90;
char servoPanPosition = 90;
//This is the acceptable 'error' for the center of the screen.
int error=10;
//The variables correspond to the middle of the screen, and will be compared to the midObject values
//int midScreenY = 500;//(height/2);        for JoyStick
//int midScreenX = 500;//(width/2);                   
int midScreenY = 240;//(height/2);          for CV
int midScreenX = 160;//(width/2); 
//Step size for servos in degrees
int stepSize=1;
void setup() {
// put your setup code here, to run once:
buffer = "";
Serial.begin(9600);
servo1.attach(12);
servo2.attach(13);
//servo1.write(servoPanPosition);
delay(2000);
//servo2.write(servoTiltPosition);
}

void loop() {
// put your main code here, to run repeatedly:
//Serial.write(0x6a);
if(Serial.available()){
String s = Serial.readStringUntil('Z');
 xval = s.substring(2,6);
 yval = s.substring(9,14);
midObjectY=yval.toInt();
midObjectX=xval.toInt();
controlServos();
 Serial.flush();
  delay(3); 
//.............................Arduino Servos mapping
 // Read the horizontal joystick value  (value between 0 and 1023)
//servoVal1 = map(servoVal1, 0, 1023, 10, 170);        // scale it to use it with the servo (result  between 0 and 180)
//if(servoVal1>150) servoVal1= 150;
//if(servoVal1<40)  servoVal1=40;
//servo1.write(servoVal1,10, false);                        // sets the servo position according to the scaled value    
//
//// Read the horizontal joystick value  (value between 0 and 1023)
//     
//servoVal2 = map(servoVal2, 0, 1023, 70, 170);     // scale it to use it with the servo (result between 70 and 180)
//if(servoVal2>150) servoVal2= 150;
//if(servoVal2<40)  servoVal2=40;
//servo2.write(servoVal2,10, false);                          // sets the servo position according to the scaled value
//
//delay(15);                      
//
//Serial.flush();
//
//float xvalue = (xvalueRec/5)-20; //Maping for width 
//float yvalue = (yvalueRec/5)-20; //Maping for Height 
//    //Serial.write(xvalue);
//    //Serial.write( " "+ yvalue);
////delay(50);
//if(xvalue>150) xvalue= 150;
//if(xvalue<40)  xvalue=40;
//servo1.write(xvalue+15,10, false);  // // move to xvalue degrees, use a speed of 10, do not wait until move is complete (wait if true)
//if(yvalue>150) yvalue= 150;
//if(yvalue<40)  yvalue=40;
//servo2.write(yvalue,10, false);
//Serial.flush();
//delay(5); 
  }
    


}
// Function to return a substring defined by a delimiter at an index
void controlServos(){
   //Find out if the Y component of the object is below the middle of the screen.
    if(midObjectY < (midScreenY - error)){
      if(servoTiltPosition >= 60)servoTiltPosition -=stepSize; //If it is below the middle of the screen, update the tilt position variable to lower the tilt servo.
    }
    //Find out if the Y component of the object is above the middle of the screen.
    else if(midObjectY > (midScreenY + error)){
      if(servoTiltPosition <= 100)servoTiltPosition +=stepSize; //Update the tilt position variable to raise the tilt servo.
    }
    //Find out if the X component of the object is to the left of the middle of the screen.
    if(midObjectX < (midScreenX - error)){
      if(servoPanPosition >= 30)servoPanPosition -=stepSize; //Update the pan position variable to move the servo to the left.
    }
    //Find out if the X component of the object is to the right of the middle of the screen.
    else if(midObjectX > midScreenX + error){
      if(servoPanPosition <= 120)servoPanPosition +=stepSize; //Update the pan position variable to move the servo to the right.
    }
  servo1.write(servoPanPosition);
  delay(2);
  servo2.write(servoTiltPosition);
  delay(1000);
  }



  
