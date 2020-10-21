#include "ESP32Wiimote.h"

ESP32Wiimote wiimote;

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32Wiimote");
    
    wiimote.init();
//  wiimote.addFilter(ACTION_IGNORE, FILTER_NUNCHUK_ACCEL);
    
    Serial.println("Started");
}

void loop()
{
  wiimote.task();
  if (wiimote.available() > 0) 
  {
      ButtonState  button  = wiimote.getButtonState();
      NunchukState nunchuk = wiimote.getNunchukState();

      char ca     = (button & BUTTON_A)     ? 'A' : '.';
      char cb     = (button & BUTTON_B)     ? 'B' : '.';
      char cc     = (button & BUTTON_C)     ? 'C' : '.';
      char cz     = (button & BUTTON_Z)     ? 'Z' : '.';
      char c1     = (button & BUTTON_ONE)   ? '1' : '.';
      char c2     = (button & BUTTON_TWO)   ? '2' : '.';
      char cminus = (button & BUTTON_MINUS) ? '-' : '.';
      char cplus  = (button & BUTTON_PLUS)  ? '+' : '.';
      char chome  = (button & BUTTON_HOME)  ? 'H' : '.';
      char cleft  = (button & BUTTON_LEFT)  ? '<' : '.';
      char cright = (button & BUTTON_RIGHT) ? '>' : '.';
      char cup    = (button & BUTTON_UP)    ? '^' : '.';
      char cdown  = (button & BUTTON_DOWN)  ? 'v' : '.';

      Serial.printf("button: %05x = ", (int)button);
      Serial.print(ca);
      Serial.print(cb);
      Serial.print(cc);
      Serial.print(cz);
      Serial.print(c1);
      Serial.print(c2);
      Serial.print(cminus);
      Serial.print(chome);
      Serial.print(cplus);
      Serial.print(cleft);
      Serial.print(cright);
      Serial.print(cup);
      Serial.print(cdown);
      Serial.printf(", nunchuck.stick: %3d/%3d", nunchuk.xStick, nunchuk.yStick);
      Serial.printf(", nunchuck.axis: %3d/%3d/%3d\n", nunchuk.xAxis, nunchuk.yAxis, nunchuk.zAxis);
  }
  delay(10);
}
