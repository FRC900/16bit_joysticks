/* Complete USB Joystick Example
   Teensy becomes a USB joystick with 16 or 32 buttons and 6 axis input

   You must select Joystick from the "Tools > USB Type" menu

   Pushbuttons should be connected between the digital pins and ground.
   Potentiometers should be connected to analog inputs 0 to 5.

   This example code is in the public domain.
*/


void setup() {
  // you can print to the serial monitor while the joystick is active!
  Serial.begin(9600);
  Joystick.useManualSend(true);
  Serial.println("Begin Complete Joystick Test");
}


uint16_t loop_count = 0;
void loop() {
  loop_count++;
  // read 6 analog inputs and use them for the joystick axis
  Joystick.X(loop_count);
  Joystick.Y(loop_count + 1);
  Joystick.Z(loop_count + 2);
  Joystick.Xrotate(loop_count + 3);
  Joystick.Yrotate(loop_count + 4);
  Joystick.Zrotate(loop_count + 5);
  for (uint8_t i = 1; i < 15; i++) Joystick.slider(i, loop_count + 0x1000 + i);
  for (uint8_t i = 1; i < 13; i++) Joystick.hat(i, ((loop_count + i) & 7) * 45);
  for (uint8_t i = 1; i < 129; i++) Joystick.button(i, (loop_count + i) & 1);
  Joystick.send_now();

  // a brief delay, so this runs "only" 200 times per second
  delay(500);
}
