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

unsigned int test = 0;
//=============================================================================
// loop
//=============================================================================
void loop()
{
  myusb.Task();
  PrintDeviceListChanges();
  //printAxis(); 
  
  // signed 16 bit integers
  uint16_t  lftx = joysticks[0].getAxis(0);
  uint16_t  lfty = joysticks[0].getAxis(1);
  uint16_t  rhtx = joysticks[0].getAxis(2);
  uint16_t  rhty = joysticks[0].getAxis(5);

  test += 1;
  std::bitset<16> a = lftx;
  std::bitset<16> b = lfty;
  std::bitset<16> c = rhtx;
  
  
  uint8_t siglftx = lftx >> 8;
  uint8_t lwrlftx = lftx & 0xFF;
  Serial.println();
  Serial.println();
  Serial.printf("Sig lftx %x ", siglftx);
  Serial.printf("Lwr lftx %x ", lwrlftx);
  Serial.println();
  
  uint8_t siglfty = lfty >> 8;
  uint8_t lwrlfty = lfty & 0xFF;
  Serial.printf("Sig Lfty %x ", siglfty);
  Serial.printf("Lwr Lfty %x ", lwrlfty);
  Serial.println();
  uint8_t sigrhtx = rhtx >> 8;
  uint8_t lwrrhtx = rhtx & 0xFF;
  Serial.printf("Sig Rhtx %x ", sigrhtx);
  Serial.printf("Lwr Rhtx %x ", lwrrhtx);
  Serial.println();
  
  /*
  double lftxs = (lftx > 0) ? lftx / 32767.0 : lftx / 32768.0;
  double lftys = (lfty > 0) ? lfty / 32767.0 : lfty / 32768.0;
  double rhtxs = (rhtx > 0) ? rhtx / 32767.0 : rhtx / 32768.0;
  Serial.print("Left x scaled=");
  Serial.print(lftxs);
  Serial.println();
   Serial.print("Left y scaled=");
  Serial.print(lftys);
  Serial.println();
    Serial.print("Right x scaled=");
  Serial.print(rhtxs);
  Serial.println();
  Serial.println();
*/
  
  Serial.printf("Lftx input:  %d\n", (int16_t) lftx);
  //printBitset(a);
  
  Serial.printf("Lfty input:  %d\n", (int16_t) lfty);
  //printBitset(b);
  Serial.printf("Rhtx input:  %d\n", (int16_t) rhtx);
  //printBitset(c);

  Joystick.X(siglftx);
  Joystick.Xr(lwrlftx);
  
  Joystick.Y(siglfty);
  Joystick.Yr(lwrlfty);
  Joystick.Z(sigrhtx);
  Joystick.Zr(lwrrhtx); 
  
  // DO NOT FORGET THIS
  Joystick.send_now();
  joysticks[0].joystickDataClear();
  delay(100);
}

template <size_t bitsetsize>
void printBitset(const std::bitset<bitsetsize> &a) {
    for (auto i = 15; i >=0; i--) {
    int temp = a[i];
    Serial.print(temp);
   }
   Serial.println();
   
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
