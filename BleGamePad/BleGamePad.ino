/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_14* 
 * 
 * Possible DPAD/HAT switch position values are: 
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, 
 * DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * 
 * bleGamepad.setAxes takes the following signed char parameters: 
 * (Left Thumb X, Left Thumb Y, Right Thumb X, Right Thumb Y, Left Trigger, Right Trigger, Hat switch position);
 */
 
#include "M5Atom.h"
#include <BleGamepad.h> 

// HRG added to BleGamePad.h
//   void setButtons(uint16_t bmask) { _buttons = bmask; }; // call before setAxes()
 

#define LIMIT 127
inline double max(double a, double b) { return ((a >= b) ? a : b); }
inline double min(double a, double b) { return ((a >= b) ? b : a); }

static BleGamepad bleGamepad;
static bool active = false;

static long last_ms = 0;
static int num_run = 0;

void setup() 
{
    M5.begin(true, true, true);
    M5.IMU.Init();
    M5.dis.fillpix(CRGB::Blue);

    Serial.println("Starting BLE GamePad");
    bleGamepad.begin();

    last_ms = millis();
}

void loop() 
{
    M5.update();

    num_run++;

    uint8_t pressed = M5.Btn.isPressed();

    double pitch, roll;
    M5.IMU.getAttitude(&roll, &pitch);

    // convert to coords
    int x = -8*roll;
    int y = -8*pitch;

    // clamp the coords
    x = max(-LIMIT, min(x, LIMIT));
    y = max(-LIMIT, min(y, LIMIT));

    if (millis() > last_ms)
    {
        Serial.printf("FPS:%4d, IMU pitch: %.2f, roll: %.2f => x:%4d, y:%4d\n", num_run, pitch, roll, x, y);
        num_run = 0;
        last_ms += 1000;
    }

    if(bleGamepad.isConnected()) 
    {
        if (!active)
        {
            Serial.println("Connected");
            M5.dis.fillpix(CRGB::Green);
            active = true;
        }

        // update game pad
        bleGamepad.setButtons(pressed ? BUTTON_1 : 0);
        bleGamepad.setAxes(x, y, 0, 0, 0, 0, DPAD_CENTERED);
    }
    else
    {
        if (active)
        {
            Serial.println("Disconnected");
            M5.dis.fillpix(CRGB::Red);
            active = false;
        }
    }

    delay(15);
}
