This Arduino example for the the legoino (https://github.com/corneliusmunz/legoino) library uses 
an [Atom Matrix](https://docs.m5stack.com/#/en/core/atom_matrix) ESP32 device to emulate 
a [Lego Boost](https://www.lego.com/en-ch/product/boost-creative-toolbox-17101)  Move Hub.

Still work in progress, but already supports:
- connection with [Lego PoweredUp](https://play.google.com/store/apps/details?id=com.lego.common.poweredup) app
- LED command (to change color)
- motor drive commands (speed)

As the Lego Powered Up allows recently to build custom GUIs, the hub emulation can serve as a 
frame work to control any ESP32 project from the mobile phone over BLE.
