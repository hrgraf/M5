// This example creates a bridge between Serial and Classical Bluetooth (SPP).
// What is received from the Serial is sent byte-wise to the BT.
// What is received from the BT is shown in HEX and ASCII on the Serial.

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define BUF_LEN 80 // set to 1 for byte-wise, larger for multi-byte

BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(115200);
  Serial.println("BTSerial monitor");


  SerialBT.begin("ESP32_BT"); // Bluetooth device name
  SerialBT.setTimeout(10); // ms
  Serial.println("ESP32_BT started...");
}

void loop() 
{
  if (Serial.available())
  {
    int ch = Serial.read();
    SerialBT.write(ch);
    SerialBT.print(" : ");
    SerialBT.println(ch, HEX);
  }

  if (SerialBT.available()) 
  {
    if (BUF_LEN == 1) // byte-wise
    {
      int ch = SerialBT.read();
      Serial.write(ch);
      Serial.print(" : ");
      Serial.println(ch, HEX);
    }
    else // multi-byte data/commands with timeout
    {
      static byte buf[BUF_LEN];
      int len = SerialBT.readBytes(buf, BUF_LEN);
    
      Serial.print("HEX: ");
      for (int i=0; i<len; i++)
      {
        Serial.print(buf[i], HEX);
        Serial.print(" ");
      }
      Serial.print(", ASCII: ");
      Serial.write(buf, len);
      Serial.println("");
    }
  }
  
  //delay(10); // ms
}
