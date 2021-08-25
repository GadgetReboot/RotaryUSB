// LS-30 Rotary Joystick to MAME Interface
//
// MAME is configured to take CW/CCW inputs to play arcade games that used
// these joysticks with 12 inputs so this adapter is required to translate
// the joystick switches into usable rotation signals for MAME.
//
// Reads the 12 rotary switches and detects when the position has changed.
// Generates a usb keyboard key press to indicate a clockwise or
// counter-clockwise movement has occurred.
//
//
// Target Hardware:  ATTiny85 running Micronucleus bootloader (like Digispark)
//                   Rotary Joystick with 12 position switch
//
// Connections:
// Joystick Harness Pin     ATTiny85 Port B Pin
// 1, 5, 9                         0
// 2, 6, 10                        1
// 3, 7, 11                        2
// 4, 8, 12                        5
// 13                             GND
//
//
// Required libraries and board files:
// Bounce2    https://github.com/thomasfredericks/Bounce2
// Digispark board support
//
// Tested with Arduino IDE 1.8.13
// To program the ATTiny85, choose board type:  Digispark Default 16.5 MHz
//
// Gadget Reboot
// https://www.youtube.com/gadgetreboot


#include "DigiKeyboard.h"
#include <Bounce2.h>

#define debounceTime 30      // switch debounce in ms
#define switchOne     0      // rotary switch input pins on DigiSpark
#define switchTwo     1
#define switchThree   2
#define switchFour    5

byte lastState = 0;      // previous stored joystick reading
byte curState  = 0;      // current joystick reading to evaluate against last reading

// create debouncer objects for inputs
Bounce debouncedOne   = Bounce();
Bounce debouncedTwo   = Bounce();
Bounce debouncedThree = Bounce();
Bounce debouncedFour  = Bounce();

void setup() {

  DigiKeyboard.sendKeyStroke(0);
  debouncedOne.attach( switchOne , INPUT_PULLUP  );     // setup the bounce instance
  debouncedOne.interval(debounceTime);                  // debounce interval in ms

  debouncedTwo.attach( switchTwo , INPUT_PULLUP  );     // setup the bounce instance
  debouncedTwo.interval(debounceTime);                  // debounce interval in ms

  debouncedThree.attach( switchThree , INPUT_PULLUP  ); // setup the bounce instance
  debouncedThree.interval(debounceTime);                // debounce interval in ms

  debouncedFour.attach( switchFour , INPUT_PULLUP  );   // setup the bounce instance
  debouncedFour.interval(debounceTime);                 // debounce interval in ms

  // take an initial reading of the rotary switches

  debouncedOne.update();                                 // update the switch status
  if (debouncedOne.read() == LOW)                        // if a switch was grounded, update the status register
    bitSet(curState, 0);

  debouncedTwo.update();                                 // update the switch status
  if (debouncedTwo.read() == LOW)                        // if a switch was grounded, update the status register
    bitSet(curState, 1);

  debouncedThree.update();                                // update the switch status
  if (debouncedThree.read() == LOW)                       // if a switch was grounded, update the status register
    bitSet(curState, 2);

  debouncedFour.update();                                 // update the switch status
  if (debouncedFour.read() == LOW)                        // if a switch was grounded, update the status register
    bitSet(curState, 3);

  lastState = curState;  // make last and current readings identical to avoid a false output trigger on power up

}  // end setup()


void loop() {

  DigiKeyboard.update();
  readJoystick();        // read the current rotary switch states into curState register
  processJoystick();     // determine if a rotation has occurred and set a flag if so

}  // end loop()


// read the joystick rotary switches into the curState register
void readJoystick() {

  debouncedOne.update();                   // update the debouncer status
  if (debouncedOne.fell()) {               // if a switch was grounded, update the status register
    curState = 0;                          // clear the register and re-build it from current switch reading
    bitSet(curState, 0);
  }

  debouncedTwo.update();                   // update the debouncer status
  if (debouncedTwo.fell()) {               // if a switch was grounded, update the status register
    curState = 0;                          // clear the register and re-build it from current switch reading
    bitSet(curState, 1);
  }

  debouncedThree.update();                 // update the debouncer status
  if (debouncedThree.fell()) {             // if a switch was grounded, update the status register
    curState = 0;                          // clear the register and re-build it from current switch reading
    bitSet(curState, 2);
  }

  debouncedFour.update();                  // update the debouncer status
  if (debouncedFour.fell()) {              // if a switch was grounded, update the status register
    curState = 0;                          // clear the register and re-build it from current switch reading
    bitSet(curState, 3);
  }

}  // end readJoystick()


// check if rotary switches are different from last saved reading
// and set the appropriate flag for the detected rotation
void processJoystick() {

  int diff = (lastState - curState);  // check for a difference in joystick readings

  // clockwise rotation has occurred
  if ( ((diff < 0) && !((lastState == B0001) && (curState == B1000))) |
       ((diff > 0) && (lastState == B1000) && (curState == B0001)) ) {

    DigiKeyboard.sendKeyStroke(KEY_R, MOD_SHIFT_LEFT);

    // debugLastCur();  // show debug last and current info on usb keyboard out

  }

  // counter-clockwise rotation has occurred
  else if ( ((diff > 0) && !((lastState == B1000) && (curState == B0001))) |
            ((diff < 0) && (lastState == B0001) && (curState == B1000)) ) {

    DigiKeyboard.sendKeyStroke(KEY_L, MOD_SHIFT_LEFT);

    // debugLastCur();  // show debug last and current info on usb keyboard out

  }
  lastState = curState;

}  // end processJoystick()


void debugLastCur() {

  DigiKeyboard.println();
  DigiKeyboard.print("Last:");
  switch (lastState) {
    case 1:
      DigiKeyboard.println("0001");
      break;
    case 2:
      DigiKeyboard.println("0010");
      break;
    case 4:
      DigiKeyboard.println("0100");
      break;
    case 8:
      DigiKeyboard.println("1000");
      break;
    default:
      DigiKeyboard.println("nul");
      break;
  }


  DigiKeyboard.print("Curr:");
  switch (curState) {
    case 1:
      DigiKeyboard.println("0001");
      break;
    case 2:
      DigiKeyboard.println("0010");
      break;
    case 4:
      DigiKeyboard.println("0100");
      break;
    case 8:
      DigiKeyboard.println("1000");
      break;
    default:
      DigiKeyboard.println("nul");
      break;
  }

}
