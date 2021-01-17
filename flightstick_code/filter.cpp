/*
 * Joystick input noise reduction and axis smoothing.
 * This file is distributed under the terms of the GNU license, (c) Luiz o Piloto 2021
 */

#include "filter.h"

/*
 * =============================================================================
 * (Exponential Moving Average) EMA_Filter Calss.
 * 
 * Used to reduce noise when reading the sensors data.
 * 
 * float al is the smoothing factor, must be a value between 0.0 and 1.0.
 * Less than 0.5 increases smoothing but have some delay.
 * More than 0.5 is more fast but can still retain some noise.
 * =============================================================================
 */
EMA_Filter::EMA_Filter(float al) {
  alpha = al;
  sVal  = Get_EMA(0);
}

float EMA_Filter::Get_EMA(int iV) {
  iVal = iV;
  sVal = round(alpha * iVal) + round((1 - alpha) * sVal); //Run the EMA and store the result.
  return sVal;
}


/*
 * =============================================================================
 * Axis_Remap Calss.
 * 
 * It creates a lookup table to convert a linear analog input map into a more
 * smooth "cubic curve" map, with slightly less sensitivity close to the center.
 * 
 * uint8_t r - The maximum axis range, usually 127 or 255, but can be anything
 * between 0 to 255.
 * 
 * float f - The smoothing curve factor, must be a value greater than 0.
 * OBS: Anything above 10 results in an almost linear map.
 * 
 * remap(float f) - Updates the lookup table using the provided curve factor.
 * 
 * output(uint8_t x) - Returns the "smoothed" value from the lookup table.
 * =============================================================================
 */
Axis_Remap::Axis_Remap(uint8_t r, float f) {
  range = r;
  remap(f);
}

Axis_Remap::~Axis_Remap() {
  delete[] table; //Frees the memory used by the lookup table.
}

uint8_t Axis_Remap::output(uint8_t x) {
  return table[x];
}

void Axis_Remap::remap(float f) {
  factor = (f < 0) ? 0 : f;
  uint8_t midr = round(range / 2);
  int16_t y;
  for(int x = 0; x <= range; x++) {
    y = round((((pow((x - midr), 3) / pow(midr, 2)) + midr) + (x * factor)) / (1 + factor));
    table[x] = (y < 0) ? 0 : ((y > range) ? range : y);
  }
}
