/**
 * A Legoino example to emulate a Boost Move Hub with an Atom Matrix ESP32 device.
 * 
 * (c) Copyright 2020 - Hans-Rudolf Graf
 * 
 * Released under MIT License
 * 
 */

#include "M5Atom.h"
#include "Lpf2HubEmulation.h"
#include "LegoinoCommon.h"

// Emulated hub instance
Lpf2HubEmulation hub("ESP32 Move Hub", HubType::POWERED_UP_HUB); // BOOST_MOVE_HUB);
static const byte portA    = (byte)MoveHubPort::A;
static const byte portB    = (byte)MoveHubPort::B;
static const byte portC    = (byte)MoveHubPort::C;
static const byte portD    = (byte)MoveHubPort::D;
static const byte portTilt = (byte)MoveHubPort::TILT;

// lego colors
static CRGB lego_rgb[NUM_COLOR] =
{
  CRGB::Black,
  CRGB::Pink,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::LightBlue,
  CRGB::Cyan,
  CRGB::Green,
  CRGB::Yellow,
  CRGB::Orange,
  CRGB::Red,
  CRGB::White
};

void writeValueCallback(byte port, byte value)
{
    int svalue = ((int8_t)value);
    CRGB crgb = CRGB::Black;
    
    switch ((MoveHubPort)port)
    {
    case MoveHubPort::A:
        Serial.print("Drive cmd for Port A: ");
        Serial.println(svalue);
        break;

    case MoveHubPort::B:
        Serial.print("Drive cmd for Port B: ");
        Serial.println(svalue);
        break;

    case MoveHubPort::AB:
        Serial.print("Drive cmd for Port AB: ");
        Serial.println(svalue);
        break;

    case MoveHubPort::C:
        Serial.print("Drive cmd for Port C: ");
        Serial.println(svalue);
        break;

    case MoveHubPort::D:
        Serial.print("Drive cmd for Port D: ");
        Serial.println(svalue);
        break;

    case MoveHubPort::LED:
        Serial.print("LED cmd for color: ");
        if (value < NUM_COLOR)
        {
            Serial.println(COLOR_STRING[value]);
            crgb = lego_rgb[value];
        }
        else
            Serial.println(value);
        M5.dis.drawpix(2, 2, crgb);
        break;
  
    default:
        Serial.print("Unhandled cmd for Port: 0x");
        Serial.print(port, HEX);
        Serial.print(", value: 0x");
        Serial.println(value, HEX);
    }
}

// initialization
void setup()
{
  M5.begin(true, true, true);
  M5.IMU.Init();
  M5.dis.fillpix(CRGB::Blue);

  // define the callback function if a write message event on the characteristic occurs
  hub.setWritePortCallback(&writeValueCallback); 
  hub.start();
  Serial.println("Started ESP32 Move Hub...");
}

// main loop
void loop()
{
  // if an app is connected, attach some devices on the ports to signalize 
  // the app that values could be received/written to that ports
  if (hub.isConnected && !hub.isPortInitialized)
  {
    Serial.println("Connected");
    M5.dis.fillpix(CRGB::Black);

    delay(1000);
    hub.isPortInitialized = true;
    hub.attachDevice((byte)MoveHubPort::A, DeviceType::MOVE_HUB_MEDIUM_LINEAR_MOTOR);
    delay(1000);
    hub.attachDevice((byte)MoveHubPort::LED, DeviceType::HUB_LED);
    delay(1000);
    hub.attachDevice((byte)MoveHubPort::B, DeviceType::MOVE_HUB_MEDIUM_LINEAR_MOTOR);
    delay(1000);

    M5.dis.fillpix(CRGB::Green);
  }

  if (!hub.isConnected && hub.isPortInitialized)
  {
    Serial.println("Disconnected");
    hub.isPortInitialized = false;
    M5.dis.fillpix(CRGB::Red);
  }

} // End of loop
