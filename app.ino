#include <TFT_eSPI.h>

TFT_eSPI tft;

void setup()
{
  tft.init();
  tft.setCursor(0, 0, 1);
  tft.println("Hello, World!");
}

void loop()
{
	
}
