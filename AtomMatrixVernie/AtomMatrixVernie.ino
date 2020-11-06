/**
 * This example uses an ESP32 AtomMatrix (https://docs.m5stack.com/#/en/core/atom_matrix) as a remote control 
 * for the Lego Boost Robot Vernie using the legoino (https://github.com/corneliusmunz/legoino) library.
 *
 * It allows to drive the Lego Boost robot by tilting the AtomMatrix device using its built-in accelerometer.
 * A visualization on the matrix shows how the tilting angles would drive the 2 motors/tracks.
 * Pushing the AtomMatrix button toggles between active motor drive and idel (default).
 * 
 * Additionally, the distance sensor is used to control the LED on the BoostHub, while the detected color
 * is shown on the AtomMatrix.
 * 
 * (c) Copyright 2020 - Hans-Rudolf Graf
 *
 * Released under MIT License
 *
 */

#include "M5Atom.h"
#include "Boost.h"

#define LIMIT 50 // motor speed limit
inline double max(double a, double b) { return ((a >= b) ? a : b); }
inline double min(double a, double b) { return ((a >= b) ? b : a); }

// Boost hub instance
static Boost hub;
static const byte portA    = (byte)MoveHubPort::A;
static const byte portB    = (byte)MoveHubPort::B;
static const byte portC    = (byte)MoveHubPort::C;
static const byte portD    = (byte)MoveHubPort::D;
static const byte portTILT = (byte)MoveHubPort::TILT;

// variables that get updated in the callbacks
static int rssi = 0; // in dB
static uint8_t batteryLevel = 0; // in percent
static int tiltX = 0, tiltY = 0; // in degrees
static int rotation = 0; // in degrees
static double distance = 0; // in cm (??)
static int color = 0; // index

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

static bool active = false; // press AtomMatrix button to toggle
static long last_ms = 0;
static int num_run = 0, num_updates = 0;

// callback function  to handle updates of the hub properties
void hubPropertyChangeCallback(void *hub, HubPropertyReference hubProperty, uint8_t *pData)
{
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  Version version;
  ButtonState button;

  num_updates++;

  switch (hubProperty)
  {
  case HubPropertyReference::RSSI:
    rssi = myHub->parseRssi(pData);
    //Serial.print("RSSI: ");
    //Serial.println(rssi);
    break;

  case HubPropertyReference::BATTERY_VOLTAGE:
    batteryLevel = myHub->parseBatteryLevel(pData);
    //Serial.print("BatteryLevel: ");
    //Serial.println(batteryLevel);
    break;

  case HubPropertyReference::BUTTON:
    button = myHub->parseHubButton(pData);
    //Serial.print("Button: ");
    //Serial.println((int)button);
    if (button == ButtonState::PRESSED)
    {
        Serial.println("Button pressed: shut down HUB");
        myHub->shutDownHub();
    }
    break;

  case HubPropertyReference::ADVERTISING_NAME:
    Serial.print("Advertising Name: ");
    Serial.println(myHub->parseHubAdvertisingName(pData).c_str());
    break;

  case HubPropertyReference::FW_VERSION:
    version = myHub->parseVersion(pData);
    Serial.print("FWVersion: ");
    Serial.print(version.Major);
    Serial.print("-");
    Serial.print(version.Minor);
    Serial.print("-");
    Serial.print(version.Bugfix);
    Serial.print(" Build: ");
    Serial.println(version.Build);
    break;

  case HubPropertyReference::HW_VERSION:
    version = myHub->parseVersion(pData);
    Serial.print("HWVersion: ");
    Serial.print(version.Major);
    Serial.print("-");
    Serial.print(version.Minor);
    Serial.print("-");
    Serial.print(version.Bugfix);
    Serial.print(" Build: ");
    Serial.println(version.Build);
    break;

  default:
    Serial.print("Unhandled HubProperty: ");
    Serial.println((byte)hubProperty);
  }
}

// callback function to handle updates of sensor values
void portValueChangeCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  CRGB crgb = CRGB::Black;

  num_updates++;

  switch (deviceType)
  {
  case DeviceType::MOVE_HUB_TILT_SENSOR:
    tiltX = myHub->parseBoostTiltSensorX(pData);
    tiltY = myHub->parseBoostTiltSensorY(pData);
    //Serial.print("Tilt X: ");
    //Serial.print(tiltX);
    //Serial.print(", Y: ");
    //Serial.println(tiltY);
    break;

  case DeviceType::COLOR_DISTANCE_SENSOR:
    color = myHub->parseColor(pData);
    distance = myHub->parseDistance(pData);
    //Serial.print("Port: ");
    //Serial.print(portNumber);
    //Serial.print(", Color: ");
    //if ((color >= 0) && (color < NUM_COLORS))
    //    Serial.print(COLOR_STRING[color]);
    //else
    //    Serial.print("unknown");
    //Serial.print(", Distance: ");
    //Serial.println(distance);

    // set hub LED color dependent of the distance sensor
    // red    -- short distance
    // orange -- medium distance
    // green  -- large distance
    if (distance < 50.0)
        myHub->setLedColor(RED);
    else if (distance < 100.0)
        myHub->setLedColor(ORANGE);
    else
        myHub->setLedColor(GREEN);

    // show color on Atom Matrix center pixel
    if ((color >= 0) && (color < NUM_COLORS))
        crgb = lego_rgb[color];
    M5.dis.drawpix(2, 2, crgb);
    break;

  case DeviceType::MEDIUM_LINEAR_MOTOR:
    rotation = myHub->parseTachoMotor(pData);
    Serial.print("Port: ");
    Serial.print(portNumber);
    Serial.print(", Rotation: ");
    Serial.println(rotation);
    break;

  default:
    Serial.print("Unhandled DeviceType: ");
    Serial.println((byte)deviceType);
  }
}

// initialization
void setup()
{
    M5.begin(true, true, true);
    M5.IMU.Init();
    M5.dis.fillpix(CRGB::Blue);

    Serial.println("Connect to BoostHub...");
    hub.init(1); // scan for 1s
    
    delay(50);
    last_ms = millis();
}

// main loop
void loop()
{
    M5.update();
    num_run++;

    double pitch, roll;
    M5.IMU.getAttitude(&roll, &pitch);

    // convert to polar
    double angle = atan2(roll, pitch);
    double radius = sqrt(pitch * pitch + roll * roll);
    //Serial.printf("IMU pitch: %.2f, roll: %.2f, angle: %.2f, radius: %.2f\n", pitch, roll, angle*180/PI, radius);

    // rotate by 45 degrees
    angle += PI / 4;

    // back to cartesian
    double left  = radius * cos(angle);
    double right = radius * sin(angle);

    // rescale the coords
    left  *= 3; //sqrt(2);
    right *= 3; //sqrt(2);

    // clamp the coords
    left  = max(-LIMIT, min(left, LIMIT));
    right = max(-LIMIT, min(right, LIMIT));
    //Serial.printf("Output %.2f, left: %.2f, right: %.2f\n", radius, left, right);

    if (M5.Btn.wasPressed())
    {
        active = !active;
        M5.dis.fillpix(active ? CRGB::Black : CRGB::Red);

        if (! active) // stop movements
        {
            hub.stopBasicMotor(portA);
            hub.stopBasicMotor(portB);
        }
    }

    // try to connect
    if (hub.isConnecting())
    {
        hub.connectHub();
        if (hub.isConnected())
        {
            Serial.println("Connected to HUB");
            M5.dis.fillpix(CRGB::Black);

            delay(200); //needed because otherwise the message is to fast after the connection procedure and the message will get lost
            hub.activateHubPropertyUpdate(HubPropertyReference::HW_VERSION, hubPropertyChangeCallback);
            hub.activateHubPropertyUpdate(HubPropertyReference::FW_VERSION, hubPropertyChangeCallback);
            hub.activateHubPropertyUpdate(HubPropertyReference::ADVERTISING_NAME, hubPropertyChangeCallback);
            delay(100);
            hub.requestHubPropertyUpdate(HubPropertyReference::HW_VERSION, hubPropertyChangeCallback);
            hub.requestHubPropertyUpdate(HubPropertyReference::FW_VERSION, hubPropertyChangeCallback);
            delay(100);

            hub.activateHubPropertyUpdate(HubPropertyReference::RSSI, hubPropertyChangeCallback);
            hub.activateHubPropertyUpdate(HubPropertyReference::BATTERY_VOLTAGE, hubPropertyChangeCallback);
            hub.activateHubPropertyUpdate(HubPropertyReference::BUTTON, hubPropertyChangeCallback);
            delay(100);

            hub.activatePortDevice(portTILT, portValueChangeCallback);
            delay(100);

            byte portInfoReq[3] = {(byte)MessageType::PORT_INFORMATION_REQUEST, portTILT, 1};
            hub.WriteValue(portInfoReq, 3);
            delay(100);

            byte portModeInfoReq[4] = {(byte)MessageType::PORT_MODE_INFORMATION_REQUEST, portTILT, 1};
            hub.WriteValue(portModeInfoReq, 4);
            delay(100);

            hub.activatePortDevice(portC, (byte)DeviceType::COLOR_DISTANCE_SENSOR, portValueChangeCallback);
            hub.activatePortDevice(portD, (byte)DeviceType::MEDIUM_LINEAR_MOTOR, portValueChangeCallback);
            delay(100);

            if (!active)
                M5.dis.fillpix(CRGB::Red); // inactive
        }
        else
        {
            Serial.println("Failed to connect to HUB");
        }
    }

    if (hub.isConnected() && active)
    {
        if (radius > 10) // minimal threshold to move
        {
            hub.setBasicMotorSpeed(portA, left);
            hub.setBasicMotorSpeed(portB, right);
        }
        else
        {
            hub.stopBasicMotor(portA);
            hub.stopBasicMotor(portB);
        }
    }

    // visualize track movements on matrix 
    CRGB crgb_left  = (left  > 0) ? CRGB::Green : CRGB::Red;
    CRGB crgb_right = (right > 0) ? CRGB::Green : CRGB::Red;
    int amp_left   = abs(left ) / 5;
    int amp_right  = abs(right) / 5;
    for (int i=0; i<5; i++)
    {
        M5.dis.drawpix(0, 4-i, (amp_left  > i) ? crgb_left  : CRGB::Black);
        M5.dis.drawpix(4, 4-i, (amp_right > i) ? crgb_right : CRGB::Black);
    }

    long ms = millis();
    if (ms - last_ms >= 1000)
    {
        //Serial.printf("Run %d times per second with %d updates\n", num_run, num_updates);
        num_run = num_updates = 0;
        last_ms += 1000;

        if (hub.isConnected() && active)
        {
            Serial.print("RSSI: ");
            Serial.print(rssi);
            Serial.print(", BatteryLevel: ");
            Serial.print(batteryLevel);
            Serial.print(", Tilt X: ");
            Serial.print(tiltX);
            Serial.print(" Y: ");
            Serial.print(tiltY);
            Serial.print(", Distance: ");
            Serial.println(distance);
        }
    }

    delay(10);
}
