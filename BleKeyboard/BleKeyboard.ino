/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
 
#include "M5Atom.h"
#include <BleKeyboard.h>

static BleKeyboard bleKeyboard;
static bool active = false;

void setup() 
{
    M5.begin(true, true, true);
    M5.IMU.Init();
    M5.dis.fillpix(CRGB::Blue);

    Serial.println("Starting BLE Keyboard");
    bleKeyboard.begin();
}

void loop() 
{
    M5.update();

    uint8_t pressed = M5.Btn.wasPressed();

    if(bleKeyboard.isConnected()) 
    {
        if (!active)
        {
            Serial.println("Connected");
            M5.dis.fillpix(CRGB::Green);
            active = true;
        }

        if (pressed) // send key
        {
            static int pos = 0;

            switch (++pos)
            {
            case 1:
                Serial.println("Sending 'Hello world'...");
                bleKeyboard.print("Hello world");
                break;

            case 2:
                Serial.println("Sending Enter key...");
                bleKeyboard.write(KEY_RETURN);
                break;

            case 3:
                Serial.println("Sending Play/Pause media key...");
                bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
                delay(1000);
                break;

            case 4:
                Serial.println("Sending Ctrl+Alt+Delete...");
                bleKeyboard.press(KEY_LEFT_CTRL);
                bleKeyboard.press(KEY_LEFT_ALT);
                bleKeyboard.press(KEY_DELETE);
                delay(100);
                bleKeyboard.releaseAll();
                break;

            default:
                Serial.println("Sending Enter key...");
                bleKeyboard.write(KEY_RETURN);
            }
        }

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
