#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
#define LM35 A0

#define NTCC A1
float signal;
int TempVals[16] = {-10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140};
int ResiVals[16] = {9397, 5896, 3792, 2500, 1707, 1175, 834, 596, 439, 323, 243, 187, 144, 113, 89, 71};
float m, b;
float adc_graus, tempC;

float res_ntc, vout_ntc, graus, vout_lm, adjust;


void setup() {

  pinMode(LM35, INPUT);
  pinMode(NTCC, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);
  
}

void loop(){
  display.clearDisplay();
  TempNTC();
  TempLM();
  display.display();
}

void TempLM(){
  
  vout_lm = analogRead(LM35);
  adjust = (vout_lm/1023)*5; // VARIÁVEL CRIADA PARA VER V.OUT DO LM35 EM mV
  Serial.println(adjust*1000);
  tempC = (500 * vout_lm) / 1023;
  
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 15);
  display.print("Air Temp: ");
  display.setCursor(70, 15);
  display.setTextSize(1);
  display.print(tempC);
  display.print("C");
  delay(500);
}
void TempNTC(){
  
  float  y0, y1, x0, x1;
  int i;
  

  ////  NTC
  signal = analogRead(NTCC);
  vout_ntc = (signal/1023)*5;
  res_ntc = 2000 * vout_ntc;

  for (i = 0; i <= 16; i++) {
    if (res_ntc >= ResiVals[i]) break;
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
  
