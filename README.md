# RotaryUSB
LS30 Rotary Joystick to USB Interface

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
