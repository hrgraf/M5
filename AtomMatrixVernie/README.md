This example uses an  [AtomMatrix](https://docs.m5stack.com/#/en/core/atom_matrix) ESP32 device as a remote control for the 
[Lego Boost Robot Vernie](https://www.lego.com/en-ch/product/boost-creative-toolbox-17101) 
using the legoino (https://github.com/corneliusmunz/legoino) library.

It allows to drive the Lego Boost robot by tilting the AtomMatrix device using its built-in accelerometer.

A visualization on the matrix shows how the tilting angles would drive the 2 motors/tracks.
Pushing the AtomMatrix button toggles between active motor drive and idel (default).
 
Additionally, the distance sensor is used to control the LED on the BoostHub, while the detected color
is shown on the AtomMatrix.
