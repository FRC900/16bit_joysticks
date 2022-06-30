// Simple test of USB Host Joystick
//
// This example is in the public domain

// Joystick host code start
#include "USBHost_t36.h"

#include <bitset>
USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);

#define COUNT_JOYSTICKS 4
JoystickController joysticks[COUNT_JOYSTICKS](myusb);
int user_axis[64];
uint32_t buttons_prev = 0;

USBDriver *drivers[] = {&hub1, &joysticks[0], &joysticks[1], &joysticks[2], &joysticks[3], &hid1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "joystick[0D]", "joystick[1D]", "joystick[2D]", "joystick[3D]",  "HID1"};
bool driver_active[CNT_DEVICES] = {false, false, false, false};

// Lets also look at HID Input devices
USBHIDInput *hiddrivers[] = {&joysticks[0], &joysticks[1], &joysticks[2], &joysticks[3]};
#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
const char * hid_driver_names[CNT_DEVICES] = {"joystick[0H]", "joystick[1H]", "joystick[2H]", "joystick[3H]"};
bool hid_driver_active[CNT_DEVICES] = {false};
bool show_changed_only = false;

uint8_t joystick_left_trigger_value[COUNT_JOYSTICKS] = {0};
uint8_t joystick_right_trigger_value[COUNT_JOYSTICKS] = {0};
uint64_t joystick_full_notify_mask = (uint64_t) - 1;

int psAxis[64];
// Joystick host code end

// Controller Emulator code start

// Configure the number of buttons.  Be careful not
// to use a pin for both a digital button and analog
// axis.  The pullup resistor will interfere with
// the analog voltage.
const int numButtons = 16;
byte allButtons[numButtons];
byte prevButtons[numButtons];
int angle=0;
// Controller Emulator code end

//=============================================================================
// Setup
//=============================================================================
void setup()
{
  Serial1.begin(2000000);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("\n\nUSB Host Joystick Testing");
  myusb.begin();

  // Emulator code
  Serial.begin(9600);
  Joystick.useManualSend(true);
  for (int i=0; i<numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
   Serial.println("Begin Complete Joystick Test");
}


//=============================================================================
// loop
//=============================================================================
void loop()
{
  myusb.Task();
  PrintDeviceListChanges();
  //printAxis();

  // signed 16 bit integers
  int16_t  lftx = joysticks[0].getAxis(0);
  int16_t  lfty = joysticks[0].getAxis(1);
  int16_t  rhtx = joysticks[0].getAxis(2);
  int16_t  rhty = joysticks[0].getAxis(5);
  std::bitset<16> a = lftx;
  std::bitset<16> b = lfty;
  std::bitset<16> c = rhtx;
  
  
  int8_t siglftx = lftx >> 8;
  int8_t lwrlftx = lftx & 0xFF;
  int8_t siglfty = lfty >> 8;
  int8_t lwrlfty = lfty & 0xFF;
  int8_t sigrhtx = rhtx >> 8;
  int8_t lwrrhtx = rhtx & 0xFF;
  Serial.printf("Left stick left right:  %d\n", lftx);
  printBitset(a);
  Serial.printf("Left stick up down:  %d\n", lfty);
  printBitset(b);
  Serial.printf("Right stick left right:  %d\n", rhtx);
  printBitset(c);
  /*
  Serial.println((rhtx & 0xF) >> 0);
  Serial.println((rhtx & 0xF0) >> 4);
  Serial.println((rhtx & 0xF00) >> 8);
  Serial.println((rhtx & 0xF000) >> 12);
  */

  
  Serial.println(siglftx);
  Serial.println(lwrlftx);
  Serial.println(siglfty);
  Serial.println(lwrlfty);
  Serial.println(sigrhtx);
  Serial.println(lwrrhtx);
  
  Joystick.X(siglftx);
  Joystick.Xr(lwrlftx);
  Joystick.Y(siglfty);
  Joystick.Yr(lwrlfty);
  Joystick.Z(sigrhtx);
  Joystick.Zr(lwrrhtx); 
  
  // DO NOT FORGET THIS
  Joystick.send_now();
  joysticks[0].joystickDataClear();
  delay(20);
}

template <size_t bitsetsize>
void printBitset(const std::bitset<bitsetsize> &a) {
    for (auto i = 0; i < 16; i++) {
    int temp = a[i];
    Serial.print(temp);
   }
   Serial.println();
   
  }
void printAxis()
{
  if (joysticks[0].available()) {
    
    int16_t lftx = joysticks[0].getAxis(0);
    int16_t lfty = joysticks[0].getAxis(1);
    int16_t rhtx = joysticks[0].getAxis(2);
    int16_t rhty = joysticks[0].getAxis(5);
    /*
    Serial.printf("Left stick up down:  %d", lftud);
    Serial.println();
    Serial.printf("Left stick left right:  %d", lftlr);
    Serial.println();/-32,768
    
    Serial.printf("Right stick up down:  %d", rhtud);
    Serial.println();
    Serial.printf("Right stick left right:  %d", rhtlr);
    Serial.println();
    */
    Serial.printf("Left stick left right:  %d\n", lftx);
    Serial.println((lftx & 0xF));
    Serial.println((lftx & 0xF0) >> 4);
    Serial.println((lftx & 0xF00) >> 8);
    Serial.println((lftx & 0xF000) >> 12);
  } 
 }

//=============================================================================
// Show when devices are added or removed
//=============================================================================
void PrintDeviceListChanges() {
  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        Serial.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
      }
    }
  }

  for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) {
    if (*hiddrivers[i] != hid_driver_active[i]) {
      if (hid_driver_active[i]) {
        Serial.printf("*** HID Device %s - disconnected ***\n", hid_driver_names[i]);
        hid_driver_active[i] = false;
      } else {
        Serial.printf("*** HID Device %s %x:%x - connected ***\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        hid_driver_active[i] = true;

        const uint8_t *psz = hiddrivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = hiddrivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = hiddrivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
      }
    }
  }
}
