SYSTEM_THREAD(ENABLED);
#include <blynk.h>
#include "oled-wing-adafruit.h"

OledWingAdafruit display;
#define TMP36 A0

boolean reverseColor = false;
double ypos = 0;
double counter = 0;
bool countersubtracting = true;
signed int xpos = 0;
int displaystate = 3;
int yetanothercounter = 0;
bool counting = false;

BLYNK_WRITE(V0)
{
  Serial.println("Button Tapped");
  if (param.asInt() == 1)
  {
    Blynk.notify("Hey, Blynk push here!");
  }
}

void doDisplayStuff(int xcounter, int ycounter, double fahrenheit)
{
  int color = reverseColor ? WHITE : BLACK;
  display.setCursor(20 + xcounter, 4 + ycounter);
  display.drawFastHLine(15 + xcounter, 0 + ycounter, 96, color);
  display.drawFastHLine(15 + xcounter, 30 + ycounter, 96, color);
  display.drawFastVLine(15 + xcounter, 0 + ycounter, 31, color);
  display.drawFastVLine(111 + xcounter, 0 + ycounter, 31, color);
  // display.drawTriangle(15 + xcounter, 0, 15 + xcounter, 30, 0 + xcounter, 15, WHITE);
  // display.drawTriangle(111 + xcounter, 0, 111 + xcounter, 30, 126 + xcounter, 15, WHITE);
  // //if (lastReading > round(temp)) {
  //     display.fillTriangle(15 + xcounter, 0, 15 + xcounter, 30, 0 + xcounter, 15, WHITE);
  // //} else if (lastReading < round(temp)) {
  //     display.fillTriangle(111 + xcounter, 0, 111 + xcounter, 30, 126 + xcounter, 15, WHITE);
  //}
  // lastReading = round(temp);
  switch (displaystate)
  {
  case 3:
    display.println(fahrenheit);
    break;
  case 1:
    display.println("PUSH");
    break;
  case 2:
    display.println("SENT");
    break;
  }
}
void doMoreDisplayStuff(int xpos, int ypos, double fahrenheit)
{
  doDisplayStuff(xpos, ypos, fahrenheit);
  doDisplayStuff(xpos - 100, ypos, fahrenheit);
  doDisplayStuff(xpos + 100, ypos, fahrenheit);
  doDisplayStuff(xpos + (100 * 2), ypos, fahrenheit);
  doDisplayStuff(xpos - (100 * 2), ypos, fahrenheit);
}

// setup() runs once, when the device is first turned on.
void setup()
{
  // Put initialization like pinMode and begin functions here.
  Blynk.begin("QnAwxbXEtUAXJ_pHkObTkMagiLke5zMZ", IPAddress(167, 172, 234, 162), 8080);
  pinMode(A5, INPUT_PULLDOWN);
  display.setup();
  display.clearDisplay();
  display.display();
  pinMode(TMP36, INPUT);
  // Serial.begin(115200);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  // The core of your code will likely live here.
  display.loop();
  double reading = analogRead(TMP36);
  double millivolts = (reading / 4095) * 3300; // I looked up the formula before you shared it with the class lol
  double temp = (millivolts - 500) / 10;
  double fahrenheit = (temp * 1.8) + 32;
  reverseColor ? display.clearDisplay() : display.fillScreen(WHITE);
  display.setTextWrap(false);
  display.setTextSize(3);
  reverseColor ? display.setTextColor(WHITE) : display.setTextColor(BLACK);
  int ytemppos = round(ypos);
  doMoreDisplayStuff(xpos, ytemppos, fahrenheit);
  doMoreDisplayStuff(xpos, ytemppos + 31, fahrenheit);
  doMoreDisplayStuff(xpos, ytemppos - 33, fahrenheit);
  doMoreDisplayStuff(xpos, ytemppos - (33 * 2), fahrenheit);
  doMoreDisplayStuff(xpos, ytemppos + (31 * 2), fahrenheit);

  xpos += 2;
  if (counting) {
    yetanothercounter++;
    if (yetanothercounter > 50) {
      yetanothercounter = 0;
      displaystate++;
      if (displaystate == 3) {
        counting = false;
      }
    }
  }
  counter += countersubtracting ? -1 : 1;
  if (counter > 400 || counter < -400)
  {
    countersubtracting = !countersubtracting;
    reverseColor = !reverseColor;
  }
  ypos -= counter / (double)100;
  if (xpos >= 100)
  {
    xpos = 0;
  }
  if (ypos >= 31)
  {
    ypos = 0;
  }
  if (ypos < 0)
  {
    ypos = 31;
  }
  display.display();
  Blynk.run();
  if (digitalRead(A5))
  {
    Blynk.notify("THIS IS A PUSH NOTIFICATION");
    displaystate = 1;
    counting = true;
  }
}