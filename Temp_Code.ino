#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX31855.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

#define MAXSO  12
#define MAXCS  10
#define MAXCLK 13
#define NTC1 A1
#define NTC2 A2
#define NTC3 A3
#define NTC4 A4

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXSO);
float signal_ntc;
float TempVals[9] = { -20, 0, 20, 40, 60, 80, 100, 120, 140};
float ResiVals[9] = {92.16, 100, 107.79, 115.54, 123.24, 130.9, 138.51, 146.07, 153.58};
float m, b;
float adc_graus, tempC;

float res_ntc, vout_ntc, graus, vout_lm, adjust;


void setup() {

  pinMode(MAXSO, INPUT);
  pinMode(MAXCLK, INPUT);
  pinMode(MAXCS, INPUT);
  pinMode(NTC1, INPUT);
  pinMode(NTC2, INPUT);
  pinMode(NTC3, INPUT);
  pinMode(NTC4, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);

}

void loop() {
  display.clearDisplay();
  float temp1 = TempNTC(NTC1);
  delay(100);
  float temp2 = TempNTC(NTC2);
  delay(100);
  float temp3 = TempNTC(NTC3);
  delay(100);
  float temp4 = TempNTC(NTC4);
  delay(100);
  Serial.print("Air Temp ------");
  Serial.println(thermocouple.readCelsius());
  Serial.print("Sensor1 ------");
  Serial.println(temp1);
  Serial.print("Sensor2 ------");
  Serial.println(temp2);
  Serial.print("Sensor3 ------");
  Serial.println(temp3);
  Serial.print("Sensor4 ------");
  Serial.println(temp4);
  Serial.println("         ");
  delay(2000);
}

float TempNTC(float NTCC) {

  float  y0, y1, x0, x1;
  int i;


  ////  NTC
  signal_ntc = analogRead(NTCC);
  vout_ntc = (signal_ntc / 1023) * 5;
  res_ntc = 200 * (1 - (vout_ntc / 5));

  for (i = 0; i <= 8; i++) {
    if (res_ntc <= ResiVals[i]) break;
  }

  y0 = ResiVals[i - 1];
  x0 = TempVals[i - 1];
  y1 = ResiVals[i];
  x1 = TempVals[i];

  m = (y1 - y0) / (x1 - x0);
  b = y0 - (x0 * m);

  graus = (res_ntc - b) / m;

  if (graus >= 140) {
    graus = 140 ;
  }
  else if (graus <= -20) {
    graus = -20;
  }
  return graus;
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 40);
  display.print("Engine Temp: ");
  display.setCursor(85, 40);
  display.setTextSize(1);
  display.print(graus);
  display.print("C");
  delay(500);
}
