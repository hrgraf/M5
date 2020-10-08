/**
 * BoostHub example to connect to Lego Vernie robot from M5Atom Matrix.
 * 
 */

#include "M5Atom.h"
#include "BoostHub.h"

static bool active = false;

#define LIMIT 50
inline double max(double a, double b) { return ((a >= b) ? a : b); }
inline double min(double a, double b) { return ((a >= b) ? b : a); }

// create a hub instance
static BoostHub hub;

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

void setup()
{
    M5.begin(true, true, true);
    M5.IMU.Init();
    M5.dis.fillpix(CRGB::Blue);

    Serial.println("Connect to BoostHub...");
    hub.init(); // scan for 10s
    
    delay(50);
}

// main loop
void loop()
{
    M5.update();

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
    }
    if (! active) // stop movements
        radius = 0;

    // try to connect
    if (hub.isConnecting())
    {
        hub.connectHub();
        if (hub.isConnected())
        {
            Serial.print("Firmware Version: ");
            Serial.print(hub.getFirmwareVersionMajor(), DEC);
            Serial.print("-");
            Serial.print(hub.getFirmwareVersionMinor(), DEC);
            Serial.print("-");
            Serial.print(hub.getFirmwareVersionBugfix(), DEC);
            Serial.print("-");
            Serial.println(hub.getFirmwareVersionBuild(), DEC);
        
            Serial.print("Hardware Version: ");
            Serial.print(hub.getHardwareVersionMajor(), DEC);
            Serial.print("-");
            Serial.print(hub.getHardwareVersionMinor(), DEC);
            Serial.print("-");
            Serial.print(hub.getHardwareVersionBugfix(), DEC);
            Serial.print("-");
            Serial.println(hub.getHardwareVersionBuild(), DEC);

            // activate built-in tilt sensor
            hub.activatePortDevice(BoostHub::Port::TILT, MOVE_HUB_TILT_SENSOR);

            // connect color/distance sensor to port c, activate sensor for updates
            hub.activatePortDevice(BoostHub::Port::C, COLOR_DISTANCE_SENSOR);

            // connect boost tacho motor to port d, activate sensor for updates
            hub.activatePortDevice(BoostHub::Port::D, MEDIUM_LINEAR_MOTOR);

            M5.dis.fillpix(CRGB::Black);
        }
        else
        {
            Serial.println("Failed to connect to HUB");
        }
    }

    // if connected, get distance
    if (hub.isConnected())
    {
        // read distance value of color/distance sensor
        double distance = hub.getDistance();

        // read color value of sensor
        int color = hub.getColor();

        // set hub LED color dependent on the distance of the sensor to an object
        // red -- short distance
        // orange -- medium distance
        // green -- large distance
        if (distance < 50.0)
            hub.setLedColor(RED);
        else if (distance < 100.0)
            hub.setLedColor(ORANGE);
        else
            hub.setLedColor(GREEN);

//      Serial.print("Rssi: ");
//      Serial.print(hub.getRssi(), DEC);
//      Serial.print(", BatteryLevel [%]: ");
//      Serial.print(hub.getBatteryLevel(), DEC);
//      Serial.print(", Tilt [x/y]: ");
//      Serial.print(hub.getTiltX(), DEC);
//      Serial.print("/");
//      Serial.print(hub.getTiltY(), DEC);
//      Serial.print(", Distance: ");
//      Serial.print(distance);
//      Serial.print(", Color: ");
//      if (color < NUM_COLOR)
//          Serial.println(COLOR_STRING[color]);
//      else
//          Serial.println("unknown");

        CRGB col_center = CRGB::Black;
        if (color < NUM_COLOR)
            col_center = lego_rgb[color];
        M5.dis.drawpix(2, 2,col_center);

        if (radius > 10) // minimal threshold to move
        {
            hub.setMotorSpeed(BoostHub::Port::A, left);
            hub.setMotorSpeed(BoostHub::Port::B, right);
        }
        else
        {
            hub.stopMotor(BoostHub::Port::A);
            hub.stopMotor(BoostHub::Port::B);
        }
    }

    // visualize track movements on matrix
    CRGB col_left  = (left  > 0) ? CRGB::Green : CRGB::Red;
    CRGB col_right = (right > 0) ? CRGB::Green : CRGB::Red;
    int amp_left   = abs(left ) / 5;
    int amp_right  = abs(right) / 5;
    for (int i=0; i<5; i++)
    {
        M5.dis.drawpix(0, 4-i, (amp_left  > i) ? col_left  : CRGB::Black);
        M5.dis.drawpix(4, 4-i, (amp_right > i) ? col_right : CRGB::Black);
    }

    delay(20);
} // End of loop
