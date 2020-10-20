#include "ESP32Wiimote.h"

ESP32Wiimote wiimote;

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32Wiimote");
    
    wiimote.init();
    wiimote.addFilter(ACTION_IGNORE, FILTER_NUNCHUK_ACCEL);
    
    Serial.println("Started");
}

void loop()
{
  wiimote.task();
  if (wiimote.available() > 0) 
  {
      uint16_t button = wiimote.getButtonState();
      Serial.printf("%04x\n", button);

      NunchukState nunchuk = wiimote.getNunchukState();
      Serial.printf("nunchuk:");
      Serial.printf(" X-Stick: %d", nunchuk.xStick);
      Serial.printf(" Y-Stick: %d", nunchuk.yStick);
      Serial.printf(" X-Axis: %d", nunchuk.xAxis);
      Serial.printf(" Y-Axis: %d", nunchuk.yAxis);
      Serial.printf(" Z-Axis: %d", nunchuk.zAxis);
      Serial.printf(" C-Button: %02x", nunchuk.cBtn);
      Serial.printf(" Z-Button: %02x", nunchuk.zBtn);
      Serial.printf("\n");
      
  }
  delay(10);
}
