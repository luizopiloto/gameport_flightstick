#include <SPI.h>
#include <EEPROM.h>
#include "filter.h"
#define MCP_WRITE 0b00010001

//Pinout definition
const int X1 = A4;
const int Y1 = A5;
const int X2 = A6;
const int Y2 = A7;
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

int X1val = 0;
int Y1val = 0;
int X2val = 0;
int Y2val = 0;
int BT1val = 0;
int BT2val = 0;
int BT3val = 0;
int BT4val = 0;
int axRst = 0;
int axSet = 0;

typedef struct {
  int X1max;
  int X1min;
  int Y1max;
  int Y1min;
  int X2max;
  int X2min;
  int Y2max;
  int Y2min;
} ranges;

ranges axRange;

EMA_Filter filterX1(0.4);
EMA_Filter filterY1(0.4);
EMA_Filter filterX2(0.5);
EMA_Filter filterY2(0.4);

Axis_Remap axMap(255);

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
  if (RX1 < axRange.X1min) {axRange.X1min = RX1;}
  if (RX1 > axRange.X1max) {axRange.X1max = RX1;}
  if (RY1 < axRange.Y1min) {axRange.Y1min = RY1;}
  if (RY1 > axRange.Y1max) {axRange.Y1max = RY1;}
  if (RX2 < axRange.X2min) {axRange.X2min = RX2;}
  if (RX2 > axRange.X2max) {axRange.X2max = RX2;}
  if (RY2 < axRange.Y2min) {axRange.Y2min = RY2;}
  if (RY2 > axRange.Y2max) {axRange.Y2max = RY2;}
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
  BT1val = digitalRead(BT1);
  BT2val = digitalRead(BT2);
  BT3val = digitalRead(BT3);
  BT4val = digitalRead(BT4);
  digitalWrite(CB1, BT1val);
  digitalWrite(CB2, BT2val);
  digitalWrite(CB3, BT3val);
  digitalWrite(CB4, BT4val);
  X1val = map(filterX1.Get_EMA(analogRead(X1)), axRange.X1min, axRange.X1max, 0, 255);
  Y1val = map(filterY1.Get_EMA(analogRead(Y1)), axRange.Y1min, axRange.Y1max, 255, 0);
  X2val = map(filterX2.Get_EMA(analogRead(X2)), axRange.X2min, axRange.X2max, 255, 0);
  Y2val = map(filterY2.Get_EMA(analogRead(Y2)), axRange.Y2min, axRange.Y2max, 255, 0);
  spiWrite(MCP_WRITE, axMap.output(X1val), CX1);
  spiWrite(MCP_WRITE, axMap.output(Y1val), CY1);
  spiWrite(MCP_WRITE, axMap.output(X2val), CX2);
  spiWrite(MCP_WRITE, axMap.output(Y2val), CY2);
  
  if (BT1val && BT2val && BT3val && BT4val && !axSet) {++axRst;}
  else {axRst = 0;}
  
  if (axRst > 200) {
    axRange.X1min = 1023;
    axRange.X1max = 0;
    axRange.Y1min = 1023;
    axRange.Y1max = 0;
    axRange.X2min = 1023;
    axRange.X2max = 0;
    axRange.Y2min = 1023;
    axRange.Y2max = 0;
    axRst = 0;
    axSet = 1;
  }
  if (axSet) {
    autoRange(filterX1.Get_EMA(analogRead(X1)),
              filterY1.Get_EMA(analogRead(Y1)),
              filterX2.Get_EMA(analogRead(X2)),
              filterY2.Get_EMA(analogRead(Y2)));
    if (!BT1val && !BT2val && BT3val && BT4val) {
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
