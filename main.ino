#include <TFT_eSPI.h>
#include "screen.hpp"

TFT_eSPI tft;

void setup()
{
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(getRotation(90));

  tft.setCursor(0, 0, 1);
  tft.println("Hello, World!");
}

void loop()
{
	
}
