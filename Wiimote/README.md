Using a Nintendo Wii Remote (and optional a Nunchuk) as wireless input device for ESP32 (e.g. to control a robot).

On the ESP32, it reports easily at 100Hz:
- all regular button presses (A/B/C/Z/1/2/-/Home/+/D-Pad)
- the 3-dimensional acceleration/orientation of both Wiimote and Nunchuk
- the analog joystick of the Nunchuk

It uses my fork of the [Arduino-ESP32Wiimote](https://github.com/hrgraf/ESP32Wiimote) library, that has the following improvements:
- better output
- optional accelerometer read-out of Wiimote itself

