#include "OpenT12.h"

int fputc(int c, FILE* f)
{
    Serial.write(c);
    //SerialBT.write(c);
    return c;
}