// DigiMouse test and usage documentation
// CAUTION!!!! This does click things!!!!!!!!
// Originally created by Sean Murphy (duckythescientist)

//#include "DigiKeyboard.h"
#include "DigiMouse.h"


void setup() {
  DigiMouse.begin(); //start or reenumerate USB - BREAKING CHANGE from old versions that didn't require this
}

#define D_Y 20
#define D_X 20

void loop() {
  
  // If not using plentiful DigiMouse.delay(), make sure to call
  // DigiMouse.update() at least every 50ms
  
  // move across the screen
  // these are signed chars
  DigiMouse.moveY(D_Y); //down
  DigiMouse.delay(200);
  DigiMouse.setButtons(1<<0);
  DigiMouse.delay(300);
  DigiMouse.setButtons(0);
  DigiMouse.delay(300);
  DigiMouse.moveX(D_X); //right
  DigiMouse.delay(200);
  DigiMouse.setButtons(1<<0);
  DigiMouse.delay(300);
  DigiMouse.setButtons(0);
  DigiMouse.delay(300);
  DigiMouse.moveY(-D_Y); //up
  DigiMouse.delay(200);
  DigiMouse.setButtons(1<<0);
  DigiMouse.delay(300);
  DigiMouse.setButtons(0);
  DigiMouse.delay(300);
  DigiMouse.moveX(-D_X); //left
  DigiMouse.delay(200);
  DigiMouse.setButtons(1<<0);
  DigiMouse.delay(300);
  DigiMouse.setButtons(0);
  DigiMouse.delay(300);
  
//  DigiMouse.scroll(5);
//  DigiMouse.delay(500);
  
  // or DigiMouse.move(X, Y, scroll) works
  
  // three buttons are the three LSBs of an unsigned char
  //DigiMouse.setButtons(1<<0); //left click
  //DigiMouse.delay(500);
 // DigiMouse.setButtons(0); //unclick all
  //DigiMouse.delay(500);

  //or you can use these functions to click
  //DigiMouse.rightClick();
  //DigiMouse.delay(500);
  //DigiMouse.leftClick();
  //DigiMouse.delay(500);
  //DigiMouse.middleClick();
  //DigiMouse.delay(500);

//  DigiKeyboard.sendKeyStroke(0);
//  DigiKeyboard.println("Hello Digispark!");
//  DigiKeyboard.delay(5000);
}
