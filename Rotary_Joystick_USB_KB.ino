// LS-30 Rotary Joystick to MAME Interface
// using ATTiny85 with vUSB micronucleus bootloader
//
// Reads the 12 rotary switches and detects when the position has changed.
// Generates a usb keyboard key press to indicate a clockwise or
// counter-clockwise movement has occurred.
//
// Target Hardware:  ATTiny85 running Micronucleus bootloader 
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
// Required Arduino libraries, board files, and utilities:
//
// Before the ATTiny85 can be programmed with the sketch from the IDE, the Micronucleus bootloader must be flashed 
// with the ATTiny85 on a breadboard and using an Uno as a programmer (or some other method).
// Once that is done and the ATTiny85 is installed on the USB PCB, it will appear as a USB device when plugged in and 
// the IDE can program it over USB.
//
// Download Micronucleus bootloader v2.4 file t85_default.hex from
//    https://github.com/ArminJo/micronucleus-firmware/tree/ca7216847e6bbf695067076b6c07703ca2d96761/firmware/releases
//
// To upload t85_default.hex bootloader, an Arduino Uno can be used as an ISP programmer to program an ATTiny85
// on a breadboard.  A wiring diagram can be found here as a reference
// https://homemadehardware.com/guides/programming-an-attiny85/
// but just use that site as a wiring example between the Uno programmer and the ATTiny85 on a breadboard.
// Setup the Uno as a programmer by loading the IDE example sketch "ArduinoISP" (which comes with the IDE) 
// and program the Uno.  Now the Uno can act as a programmer between the computer and the ATTiny chip.
// Add a capacitor (around 1 to 10 uF) between the Uno Reset and Ground pins (capacitor + to Reset and - to Gnd)
// This prevents the Uno from getting reset by the computer when attempting to program the ATTiny.
// Then wire up the connections between Uno and ATTiny85 to flash the bootloader with the following instructions.
//
// This example procedure was done on Windows 10.  Similar instructions can be found for Linux/Mac by following the
// general process and using the equivalent versions of tools available on the other OS. 
//
// Download avrdude.conf and avrdude.exe from
//    https://github.com/ArminJo/micronucleus-firmware/tree/ca7216847e6bbf695067076b6c07703ca2d96761/windows_exe
// Place the avrdude files in the same folder as the previously downloaded micronucleus bootloader file t85_default.hex
// Also place the flash_bootloader.bat file from the github project https://github.com/GadgetReboot/RotaryUSB in the same folder.
// Run flash_bootloder.bat and it will launch avrdude with the settings needed to flash t85_default.hex into the ATTiny85
// A commandline parameter is needed to specify the com port of the Uno being used as a programmer.
// That will be the same COM port that was used in the IDE to set the Uno as a programmer.
// example of a valid command line:   flash_bootloader.bat COM1
// If the programming fails but the wiring seems good, it's possible the fuses in the ATTiny need to be reset if the
// chip was previously configured to disable the reset pin and use it as a GPIO.  Reset is needed for programming
// so either a different chip should be used, or a high voltage programmer must be used to reset the fuses and 
// allow programming again.  
// Example fuse reset procedure: https://www.youtube.com/watch?v=JskwC4bhgx4
//
// With the ATTiny85 programmed with a bootloader, the Uno programmer is no longer needed.
// Plug the ATTiny85 into the USB board and plug it into a USB port on a computer.
// If the device doesn't show up, 
// Windows drivers may need to be installed to recognize the board as a USB device.
// The ones that worked in this Windows 10 example were
// https://github.com/digistump/DigistumpArduino/releases/download/1.6.7/Digistump.Drivers.zip
// DPinst64.exe was run, which installed drivers and allowed the ATTiny85 board to appear as a USB Keyboard
// when the sketch was flashed.
//
// When the USB board is ready to be programmed with the sketch over USB with the Arduino IDE,
// Tested with Arduino IDE 2.3.3 on Windows 10 using:
//             Micronucleus bootloader 2.4  t85_default.hex flashed to ATTiny85 using avrdude tool, run with flash_bootloader.bat
//             Install Arduino IDE Bounce2 library 2.1 (some newer versions may not compile for some reason)
//             Install Arduino IDE Board Manager: Digistump AVR Boards 1.6.7 to add Digispark board support:
//     Add url to Arduino preferences/additional board URLs 
//     https://raw.githubusercontent.com/digistump/arduino-boards-index/master/package_digistump_index.json
//     Then in the Tools menu to add board support, search for the Digistump AVR 1.6.7 board file and install it
//     With this sketch open, choose the target board Digispark Default 16.5 MHz   from the Tools menu.
//     no other options should be needed from the Tools menu to configure the project for uploading
//
// Now the sketch should compile and upload.
// Note that the board should be unplugged prior to starting a sketch upload from the IDE. 
// The IDE will say when it is time to plug in the USB board (with a 60 second timeout) and it will program it.
// It may take a few seconds for the OS to detect the USB device before the IDE can program it.
//
// After programming, if the board is power cycled, it should appear as a USB keyboard and the rotating
// joystick should produce keystrokes when moved.  The keystrokes can be changed within the sketch, especially
// if two boards are used on the same computer for 2 player games.
// Each board should have unique keystrokes assigned and when both are plugged into the same computer, 
// they should behave like the same keyboard, just sending different keys.
//
// Gadget Reboot
// https://www.youtube.com/@gadgetreboot


#include "DigiKeyboard.h"
#include <Bounce2.h>

#define debounceTime 30  // switch debounce in ms
#define switchOne 0      // rotary switch input pins on DigiSpark
#define switchTwo 1
#define switchThree 2
#define switchFour 5

byte lastState = 0;  // previous stored joystick reading
byte curState = 0;   // current joystick reading to evaluate against last reading

// create debouncer objects for inputs
Bounce debouncedOne = Bounce();
Bounce debouncedTwo = Bounce();
Bounce debouncedThree = Bounce();
Bounce debouncedFour = Bounce();

void setup() {

  DigiKeyboard.sendKeyStroke(0);

  pinMode(switchOne, INPUT_PULLUP);
  debouncedOne.attach(switchOne);       // setup the bounce instance
  debouncedOne.interval(debounceTime);  // debounce interval in ms

  pinMode(switchTwo, INPUT_PULLUP);
  debouncedTwo.attach(switchTwo);       // setup the bounce instance
  debouncedTwo.interval(debounceTime);  // debounce interval in ms

  pinMode(switchThree, INPUT_PULLUP);
  debouncedThree.attach(switchThree);     // setup the bounce instance
  debouncedThree.interval(debounceTime);  // debounce interval in ms

  pinMode(switchFour, INPUT_PULLUP);
  debouncedFour.attach(switchFour);      // setup the bounce instance
  debouncedFour.interval(debounceTime);  // debounce interval in ms

  // take an initial reading of the rotary switches

  debouncedOne.update();           // update the switch status
  if (debouncedOne.read() == LOW)  // if a switch was grounded, update the status register
    bitSet(curState, 0);

  debouncedTwo.update();           // update the switch status
  if (debouncedTwo.read() == LOW)  // if a switch was grounded, update the status register
    bitSet(curState, 1);

  debouncedThree.update();           // update the switch status
  if (debouncedThree.read() == LOW)  // if a switch was grounded, update the status register
    bitSet(curState, 2);

  debouncedFour.update();           // update the switch status
  if (debouncedFour.read() == LOW)  // if a switch was grounded, update the status register
    bitSet(curState, 3);

  lastState = curState;  // make last and current readings identical to avoid a false output trigger on power up

}  // end setup()


void loop() {

  DigiKeyboard.update();
  readJoystick();     // read the current rotary switch states into curState register
  processJoystick();  // determine if a rotation has occurred and set a flag if so

}  // end loop()


// read the joystick rotary switches into the curState register
void readJoystick() {

  debouncedOne.update();      // update the debouncer status
  if (debouncedOne.fell()) {  // if a switch was grounded, update the status register
    curState = 0;             // clear the register and re-build it from current switch reading
    bitSet(curState, 0);
  }

  debouncedTwo.update();      // update the debouncer status
  if (debouncedTwo.fell()) {  // if a switch was grounded, update the status register
    curState = 0;             // clear the register and re-build it from current switch reading
    bitSet(curState, 1);
  }

  debouncedThree.update();      // update the debouncer status
  if (debouncedThree.fell()) {  // if a switch was grounded, update the status register
    curState = 0;               // clear the register and re-build it from current switch reading
    bitSet(curState, 2);
  }

  debouncedFour.update();      // update the debouncer status
  if (debouncedFour.fell()) {  // if a switch was grounded, update the status register
    curState = 0;              // clear the register and re-build it from current switch reading
    bitSet(curState, 3);
  }

}  // end readJoystick()


// check if rotary switches are different from last saved reading
// and set the appropriate flag for the detected rotation
void processJoystick() {

  int diff = (lastState - curState);  // check for a difference in joystick readings

  // clockwise rotation has occurred
  if (((diff < 0) && !((lastState == B0001) && (curState == B1000))) | ((diff > 0) && (lastState == B1000) && (curState == B0001))) {

    DigiKeyboard.sendKeyStroke(KEY_R, MOD_SHIFT_LEFT);

    // debugLastCur();  // show debug last and current info on usb keyboard out

  }

  // counter-clockwise rotation has occurred
  else if (((diff > 0) && !((lastState == B1000) && (curState == B0001))) | ((diff < 0) && (lastState == B0001) && (curState == B1000))) {

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
