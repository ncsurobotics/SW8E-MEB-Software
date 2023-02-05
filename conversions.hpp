
#pragma once

#include <Arduino.h>


class Conversions{
public:
    static void checkBigEndian();

    static void convertInt32ToData(int32_t input, uint8_t *outBuffer, bool littleEndian);

    static int32_t convertDataToInt32(uint8_t *data, bool littleEndian);

    static void convertInt16ToData(int16_t input, uint8_t *outBuffer, bool littleEndian);

    static int16_t convertDataToInt16(uint8_t *data, bool littleEndian);

    static void convertFloatToData(float input, uint8_t *outBuffer, bool littleEndian);

    static float convertDataToFloat(uint8_t *data, bool littleEndian);

    static bool isBigEndian;
};
