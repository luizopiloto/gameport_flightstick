/*
 * Joystick input noise reduction and axis smoothing.
 * This file is distributed under the terms of the GNU license, (c) Luiz o Piloto 2021
 */

#ifndef _FILTER_H
#define _FILTER_H
#include <math.h>
#include <stdint.h>

class EMA_Filter {
  private:
    float alpha;     //Smoothing Factor.
    int   sVal = 0;  //Stored value.
    int   iVal = 0;  //Input  value.

  public:
    EMA_Filter(float al);
    float Get_EMA(int iV);
};

class Axis_Remap {
  private:
    float    factor; //Curve Factor.
    uint8_t  range;  //Axis range.
    uint8_t* table = new uint8_t[range]; //Stores the lookup table created on class initialization. / size = (1 byte * range) / OBS: ATmega328P have 2Kb SRAM.

  public:
    Axis_Remap(uint8_t r, float f);
    ~Axis_Remap();
    void remap(float f);
    uint8_t output(uint8_t x);
};

#endif // _FILTER_H
