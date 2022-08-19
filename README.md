# 16bit_joysticks

# Features:
Intercepts all joystick inputs and can emulate any controller that's descriptor can be defined in 64 bytes. In our case it was used to give our controller 16 bits of precision through the driver's station by splitting the axis into two and then using raw joystick reads on the robot side and recostructing the inital value. 
While not currently implemented it could also be used an easy way to remap joysticks on the fly.

# Requirments:
Teensy 4 or above connected to a controller

# Usage:
Not really made for other people to use, but getting around the driver's station limitation was pretty cool. 
To use, replace the files found in "arduino-1.8.19/hardware/teensy/avr/cores/teensy4" with the usb_ files. Then run the .ino file. You also may want to change the controller descriptor. 
