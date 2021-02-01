#include <SPI.h>
#include <EEPROM.h>
#include "filter.h"
#define MCP_WRITE 0b00010001 //MCP41100 SPI write command.

//Pinout definition
const int X1  = A4;
const int Y1  = A5;
const int X2  = A6;
const int Y2  = A7;
const int BT1 = A0;
const int BT2 = A1;
const int BT3 = A2;
const int BT4 = A3;
const int CX1 = 5;
const int CY1 = 4;
const int CX2 = 2;
const int CY2 = 3;
const int CB1 = 6;
const int CB2 = 7;
const int CB3 = 8;
const int CB4 = 9;

int X1val  = 0;
int Y1val  = 0;
int X2val  = 0;
int Y2val  = 0;
int BT1val = 0;
int BT2val = 0;
int BT3val = 0;
int BT4val = 0;
int axRst  = 0;
int axSet  = 0;

typedef struct {
  int X1max;
  int X1mid;
  int X1min;
  int Y1max;
  int Y1mid;
  int Y1min;
  int X2max;
  int X2min;
  int Y2max;
  int Y2mid;
  int Y2min;
} ranges;

ranges axRange;

EMA_Filter filterX1(0.5);
EMA_Filter filterY1(0.5);
EMA_Filter filterX2(0.5);
EMA_Filter filterY2(0.5);

Axis_Remap axMap(255, 1.2);

void spiWrite(int cmd, int data, int csPin) {
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  SPI.transfer(cmd);
  SPI.transfer(data);
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  delay(10);
}

void autoRange(int RX1, int RY1, int RX2, int RY2) {
  if (RX1 < axRange.X1min) {
    axRange.X1min = RX1;
  }
  if (RX1 > axRange.X1max) {
    axRange.X1max = RX1;
  }
  if (RY1 < axRange.Y1min) {
    axRange.Y1min = RY1;
  }
  if (RY1 > axRange.Y1max) {
    axRange.Y1max = RY1;
  }
  if (RX2 < axRange.X2min) {
    axRange.X2min = RX2;
  }
  if (RX2 > axRange.X2max) {
    axRange.X2max = RX2;
  }
  if (RY2 < axRange.Y2min) {
    axRange.Y2min = RY2;
  }
  if (RY2 > axRange.Y2max) {
    axRange.Y2max = RY2;
  }
}

int32_t autoCenter(uint8_t pin) {
  int32_t t = 0;
  for (int i = 0; i < 100; i++) {
    delay(10);
    t += analogRead(pin);
  }
  return (t / 100);
}

int jMap(int val, int vmin, int vmid, int vmax, int jmin, int jmid, int jmax) {
  if (val == vmid) {
    return jmid;
  }
  else if (val < vmid) {
    return map(val, vmin, vmid, jmin, jmid);
  }
  else {
    return map(val, vmid, vmax, jmid, jmax);
  }
}

void btState() {
  BT1val = digitalRead(BT1);
  BT2val = digitalRead(BT2);
  BT3val = digitalRead(BT3);
  BT4val = digitalRead(BT4);
}

void axState(int VX1, int VY1, int VX2, int VY2) {
  X1val = jMap((VX1 < axRange.X1min) ? axRange.X1min : ((VX1 > axRange.X1max) ? axRange.X1max : VX1),
              axRange.X1min, axRange.X1mid, axRange.X1max, 0, 127, 255);
  Y1val = jMap((VY1 < axRange.Y1min) ? axRange.Y1min : ((VY1 > axRange.Y1max) ? axRange.Y1max : VY1),
              axRange.Y1min, axRange.Y1mid, axRange.Y1max, 255, 127, 0);
  X2val = map((VX2 < axRange.X2min) ? axRange.X2min : ((VX2 > axRange.X2max) ? axRange.X2max : VX2),
              axRange.X2min, axRange.X2max, 255, 0);
  Y2val = jMap((VY2 < axRange.Y2min) ? axRange.Y2min : ((VY2 > axRange.Y2max) ? axRange.Y2max : VY2),
              axRange.Y2min, axRange.Y2mid, axRange.Y2max, 255, 127, 0);
}

void setup() {
  pinMode(X1, INPUT);   //X input
  pinMode(Y1, INPUT);   //Y input
  pinMode(X2, INPUT);   //Throttle input
  pinMode(Y2, INPUT);   //Rudder   input
  pinMode(BT1, INPUT);  //Button 1 input
  pinMode(BT2, INPUT);  //Button 2 input
  pinMode(BT3, INPUT);  //Button 3 input
  pinMode(BT4, INPUT);  //Button 4 input
  pinMode(CX1, OUTPUT); //X output
  pinMode(CY1, OUTPUT); //Y output
  pinMode(CX2, OUTPUT); //Throttle output
  pinMode(CY2, OUTPUT); //Rudder   output
  pinMode(CB1, OUTPUT); //Button 1 output
  pinMode(CB2, OUTPUT); //Button 2 output
  pinMode(CB3, OUTPUT); //Button 3 output
  pinMode(CB4, OUTPUT); //Button 4 output
  digitalWrite(CX1, HIGH);
  digitalWrite(CY1, HIGH);
  digitalWrite(CX2, HIGH);
  digitalWrite(CY2, HIGH);
  SPI.begin();
  EEPROM.get(0, axRange);
  delay(250);
}

void loop() {
  btState();
  digitalWrite(CB1, BT1val);
  digitalWrite(CB2, BT2val);
  digitalWrite(CB3, BT3val);
  digitalWrite(CB4, BT4val);
  axState(filterX1.Get_EMA(analogRead(X1)),
          filterY1.Get_EMA(analogRead(Y1)),
          filterX2.Get_EMA(analogRead(X2)),
          filterY2.Get_EMA(analogRead(Y2)));
  spiWrite(MCP_WRITE, axMap.output(X1val), CX1);
  spiWrite(MCP_WRITE, axMap.output(Y1val), CY1);
  spiWrite(MCP_WRITE, axMap.output(X2val), CX2);
  spiWrite(MCP_WRITE, axMap.output(Y2val), CY2);

  if (BT1val && BT2val && BT3val && BT4val && !axSet) {++axRst;}
  else {axRst = 0;}

  if (axRst > 200) {
    axRange.X1min = 1023;
    axRange.X1mid = 511;
    axRange.X1max = 0;
    axRange.Y1min = 1023;
    axRange.Y1mid = 511;
    axRange.Y1max = 0;
    axRange.X2min = 1023;
    axRange.X2max = 0;
    axRange.Y2min = 1023;
    axRange.Y2mid = 511;
    axRange.Y2max = 0;
    axSet = 1;
    axRst = 0;

    //Wait for all the buttons to be released.
    while (BT1val || BT2val || BT3val || BT4val) {
      btState();
      delay(50);
    }
  }

  if (axSet) {
    autoRange(filterX1.Get_EMA(analogRead(X1)),
              filterY1.Get_EMA(analogRead(Y1)),
              filterX2.Get_EMA(analogRead(X2)),
              filterY2.Get_EMA(analogRead(Y2)));

    if (!BT1val && !BT2val && BT3val && BT4val) {
      axRange.X1mid = autoCenter(X1);
      axRange.Y1mid = autoCenter(Y1);
      axRange.Y2mid = autoCenter(Y2);
      EEPROM.put(0, axRange);
      delay(250);
      axSet = 0;
    }

    if (BT1val && BT2val && !BT3val && !BT4val) {
      EEPROM.get(0, axRange);
      delay(250);
      axSet = 0;
    }
  }
}
