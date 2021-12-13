#include <Encoder.h>

//set up encoders
Encoder leftRight(33, 34);
Encoder upDown(35, 36);

//initialize variables
int val = 0;
int num = 0;
int onOffButton = 32;
int colorButton = 31;
bool buttonOn = false;
bool lastButtonState = LOW;
bool buttonState = LOW;
long oldPositionHor = 0;
long oldPositionVert = 0;
int oldMidi = 64;
int newMidi = 64;
int horizVal = 0;
int vertVal = 0;
int resetVal = 0;
int RedPotPin= A18;
int GreenPotPin=A19;
int BluePotPin= A20;

//////////////////////////////////////
int RedVal = 0;    
int GreenVal = 0;   
int BlueVal = 0;    
int inByte = 0;         // incoming serial byte

//set up pinModes and the serial monitor
void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
  pinMode(onOffButton, INPUT);
  pinMode(colorButton, INPUT);
  usbMIDI.sendNoteOff(64, 0, 1);
  usbMIDI.sendNoteOn(6, 4, 127, 1);
  while (!Serial) {
   ; 
  }
  //run establishContact to make sure there is communication between Arduino and Processing
  establishContact();
}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    // Map the potentiometers so they read 0-255 which is the range for RGB values - allows the user to adjust each value to manually create the color for the line
    RedVal = map(analogRead(RedPotPin), 0, 1023, 0, 255);
    GreenVal = map(analogRead(GreenPotPin), 0, 1023, 0, 255);
    BlueVal = map(analogRead(BluePotPin), 0, 1023, 0, 255);
    //set horizontal and vertical values using the horizontalMove() and verticalMove() functions
    horizVal = horizontalMove();
    vertVal = verticalMove();
    
    //check if on/off button is pressed and send info to reset drawing
    resetVal = resetDrawing();
    if(resetVal == 5){
      num = 5;                                //if the button to reset is pressed num=5 - corresponds with an if statement in processing to reset the drawing
    } else if (horizVal > 0){
      num = horizVal;                         //if the returned horizVal is >0 send num=horizVal - corresponds with an if statement in processing to move horizontally either left or right depending on value
    } else if (vertVal > 0){
      num = vertVal;                          //if the returned vertVal is >0 send num=vertVal - corresponds with an if statement in processing to move vertically either up or down depending on value
    } else if (horizVal == 0 and vertVal == 0){
      num = 0;                                //if vertVal and horizVal neither rotary encoder has been moved and doesn't trigger anything in the processing code to draw
    }
    //call function to check if the right rotary encoder button has been pressed to switch between rainbow line mode and manual color mode
    checkColorButton(); 

    // send sensor values separated by commas so processing can separate the string
    Serial.print(RedVal);
    Serial.print(",");
    Serial.print(GreenVal);
    Serial.print(",");
    Serial.print(BlueVal);
    Serial.print(","); 
    Serial.print(num);
    Serial.print(",");
    if (buttonOn == false){
      Serial.println("0");                  //triggers manual color mode in processing
    }
    else{
      Serial.println("6");                  //triggers rainbow mode in processing
    }


  }
}


   
//check if on/off button is pressed and send info to reset drawing
int resetDrawing(){
    if(digitalRead(onOffButton) == true){
    //button has been pressed, return all the variables to their originl values to reset
    oldPositionHor = 0;
    oldPositionVert = 0;
    val = 5;
    oldMidi = 64;
    newMidi = 64;
    usbMIDI.sendNoteOff(oldMidi, 0, 1);
    usbMIDI.sendNoteOn(newMidi, 127, 1);
    return val; 
  } else{
    //button not presses, don't change anything
   val = 0;
   return val;
  }
}

//function to calculate horizontal movement and return and integer value
int horizontalMove (){
  long newPositionHor = leftRight.read();
    //Check if the horizontal positions have changed
  if (newPositionHor != oldPositionHor) {
    //if the horizontal position is greater than the prev horizontal position move left
    if (oldPositionHor > newPositionHor){
      val = 3;
      newMidi = oldMidi-1;
      usbMIDI.sendNoteOff(oldMidi, 0, 1);
      usbMIDI.sendNoteOn(newMidi, 127, 1);
      oldMidi = newMidi;
      oldPositionHor = newPositionHor;
      return val;
    }
    
    //if the horizontal position is less than the prev horizontal position move right
    if (oldPositionHor < newPositionHor) {
      val = 4;
      newMidi = oldMidi+1;
      usbMIDI.sendNoteOff(oldMidi, 0, 1);
      usbMIDI.sendNoteOn(newMidi, 127, 1);
      oldMidi = newMidi;
      oldPositionHor = newPositionHor;
      return val;
    }
  } 
 //  else{
   val = 0;
   return val;
 // }
}

//function to calculate vertical movement and return and integer value
int verticalMove (){
    long newPositionVert = upDown.read();
    //Check if the vertical positions have changed
  if (newPositionVert != oldPositionVert) {
     //if the vertical position is greater than the prev vertical position move down
    if (oldPositionVert > newPositionVert){
      val = 2;
      newMidi = oldMidi-13;
      usbMIDI.sendNoteOff(oldMidi, 0, 1);
      usbMIDI.sendNoteOn(newMidi, 127, 1);
      oldMidi = newMidi;
       oldPositionVert = newPositionVert;
      return val;
    }
     //if the vertical position is less than the prev vertical position move up
    if (oldPositionVert < newPositionVert){
      val = 1;
      newMidi = oldMidi+13;
      usbMIDI.sendNoteOff(oldMidi, 0, 1);
      usbMIDI.sendNoteOn(newMidi, 127, 1);
      oldMidi = newMidi;
       oldPositionVert = newPositionVert;
      return val;
    }
  } 
  //else{
    val = 0;
    return val;
  //}
}

//function to establish initial contact with processing
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}

//function to check if the rigth button in the rotary encoder has been pressed
void checkColorButton(){
    //latching button code so the button functions more like a switch
    lastButtonState = buttonState;
    buttonState = digitalRead(colorButton);
    if(lastButtonState == LOW and buttonState == HIGH){
      buttonOn = !buttonOn;
    }
  
}
