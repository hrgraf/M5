/**
 * A Legoino example to emulate a Boost Move Hub with an Atom Matrix ESP32 device.
 * 
 * (c) Copyright 2020 - Hans-Rudolf Graf
 * 
 * Released under MIT License
 * 
 */


#define USE_M5_ATOM 1

#if USE_M5_ATOM
#include "M5Atom.h"
#endif
#include "Lpf2HubEmulation.h"
#include "LegoinoCommon.h"

// Emulated hub instance
Lpf2HubEmulation hub("ESP32 Move Hub", HubType::BOOST_MOVE_HUB);
static const byte portA    = (byte)MoveHubPort::A;
static const byte portB    = (byte)MoveHubPort::B;
static const byte portAB   = (byte)MoveHubPort::AB;
static const byte portC    = (byte)MoveHubPort::C;
static const byte portD    = (byte)MoveHubPort::D;
static const byte portLED  = (byte)MoveHubPort::LED;
static const byte portTILT = (byte)MoveHubPort::TILT;

#if USE_M5_ATOM
// lego colors
static CRGB lego_rgb[NUM_COLORS] =
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
#endif

static bool active = false; // press AtomMatrix button to toggle
static long last_ms = 0;
static int num_run = 0, num_updates = 0;

void writeValueCallback(byte port, byte value)
{
    int svalue = ((int8_t)value);
#if USE_M5_ATOM
    CRGB crgb = CRGB::Black;
#endif
    
    num_updates++;

    switch (port)
    {
    case portA:
        Serial.print("Drive cmd for Port A: ");
        Serial.println(svalue);
        break;

    case portB:
        Serial.print("Drive cmd for Port B: ");
        Serial.println(svalue);
        break;

    case portAB:
        Serial.print("Drive cmd for Port AB: ");
        Serial.println(svalue);
        break;

    case portC:
        Serial.print("Drive cmd for Port C: ");
        Serial.println(svalue);
        break;

    case portD:
        Serial.print("Drive cmd for Port D: ");
        Serial.println(svalue);
        break;

    case portLED:
        Serial.print("LED cmd for color: ");
        if (value < NUM_COLORS)
        {
            Serial.println(COLOR_STRING[value]);
#if USE_M5_ATOM
            crgb = lego_rgb[value];
#endif
        }
        else
            Serial.println(value);
#if USE_M5_ATOM
        M5.dis.drawpix(2, 2, crgb);
#endif
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
#if USE_M5_ATOM
    M5.begin(true, true, true);
    M5.IMU.Init();
    M5.dis.fillpix(CRGB::Blue);
#else
    Serial.begin(115200);
#endif
    // define the callback function if a write message event on the characteristic occurs    
    hub.setWritePortCallback(&writeValueCallback); 
    hub.start();
    Serial.println("Started ESP32 Move Hub...");

    delay(50);
    last_ms = millis();
}

// main loop
void loop()
{
    int x=20, y=30; // default tilt

#if USE_M5_ATOM
    M5.update();

    double pitch, roll;
    M5.IMU.getAttitude(&roll, &pitch);
    //Serial.printf("IMU pitch: %.2f, roll: %.2f\n", pitch, roll);

    // update tilt
    x = -roll;
    y = pitch;
#endif
    num_run++;

    // if an app connects, attach some devices on the ports to signalize 
    // the app that values could be received/written to those ports
    if (hub.isConnected && !hub.isPortInitialized)
    {
        Serial.println("Connected");
#if USE_M5_ATOM
        M5.dis.fillpix(CRGB::Black);
#endif

        delay(2000);
        hub.setHubBatteryLevel(99);
        hub.isPortInitialized = true;
        delay(1000);
        hub.attachDevice(portA, DeviceType::MOVE_HUB_MEDIUM_LINEAR_MOTOR);
        delay(1000);
        hub.attachDevice(portB, DeviceType::MOVE_HUB_MEDIUM_LINEAR_MOTOR);
        delay(1000);
//      hub.attachDevice(portC, DeviceType::COLOR_DISTANCE_SENSOR);
//      delay(1000);
//      hub.attachDevice(portD, DeviceType::MEDIUM_LINEAR_MOTOR);
//      delay(1000);
        hub.attachDevice(portLED, DeviceType::HUB_LED);
        delay(1000);
        hub.attachDevice(portTILT, DeviceType::MOVE_HUB_TILT_SENSOR);
        delay(1000);
        
#if USE_M5_ATOM
        if (!active)
            M5.dis.fillpix(CRGB::Red); // inactive
#endif
        last_ms = millis();
    }

    // app disconnects
    if (!hub.isConnected && hub.isPortInitialized)
    {
        Serial.println("Disconnected");
        hub.isPortInitialized = false;
#if USE_M5_ATOM
        M5.dis.fillpix(CRGB::Blue);
#endif
    } 

    // app connected
    if (hub.isConnected && hub.isPortInitialized)
    {
#if USE_M5_ATOM
        // report button press/release
        if (M5.Btn.wasPressed())
        {
            active = !active;
            M5.dis.fillpix(active ? CRGB::Black : CRGB::Red);
            hub.setHubButton(true);
        }
        if (M5.Btn.wasReleased())
          hub.setHubButton(false);
#endif
        if (active)
            hub.setHubTilt(portTILT, x, y);
    }

    long ms = millis();
    if (ms - last_ms >= 1000)
    {
        //Serial.printf("Run %d times per second with %d updates\n", num_run, num_updates);
        num_run = num_updates = 0;
        last_ms += 1000;
    }

    delay(20);
}
