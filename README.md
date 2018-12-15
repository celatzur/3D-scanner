# 3D-scanner
Turntable for a 3D scanner controlled with arduino, which also shoots, via bluettoth, a phone camera to reconstruct the object with photogrametry.
 
| <img align="center" src="https://github.com/celatzur/3D-scanner/blob/master/images/3DScanner_v05x.jpg" width="312" height="251" />  | <img align="center" src="https://github.com/celatzur/3D-scanner/blob/master/images/Screenshot_colmap_02.png" width="426" height="166" /> |
| ------------- | ------------- |

Components:

- Arduino Nano
- 2 push buttons (play, shoot)
- On/off switch for bluetooth
- Cheap Bluetooth device
- 28BYJ-48 Stepper Motor 
- ULN2003 driver for the motor

More details at: https://github.com/celatzur/3D-scanner/wiki

To control the stepper motor uses the library of Adafruit, found at: https://github.com/adafruit/AccelStepper
