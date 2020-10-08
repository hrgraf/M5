#include "M5Atom.h"
#include "letters.h"

#pragma GCC diagnostic warning "-fpermissive"

void setup()
{
    M5.begin(true, false, true);
    delay(50);
}

void loop()
{
    static uint8_t FSM = 0;

    FSM++;
    switch (FSM)
    {
    case 1:
        M5.dis.displaybuff(image_A);
        break;
    case 2:
        M5.dis.displaybuff(image_T);
        break;
    case 3:
        M5.dis.displaybuff(image_O);
        break;
    case 4:
        M5.dis.displaybuff(image_M);
        break;
    default:
        M5.dis.displaybuff(image_black);
        FSM = 0;
        break;
    }

    M5.update();
    delay(FSM ? 250: 1000);
}
