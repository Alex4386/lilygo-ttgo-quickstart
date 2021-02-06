#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "screen.hpp"

int getRotation(int degrees) {
  return (degrees % 360) / 90;
}

void initScreenForCLI(TFT_eSPI *tft, int degrees) {
  tft->init();
  tft->setRotation(getRotation(degrees));
  clearScreenForCLI(tft);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
}

void clearScreenForCLI(TFT_eSPI *tft) {
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE);

  tft->setTextSize(1);
  tft->setCursor(0, 0, 1);
}

void printLog(TFT_eSPI *tft, LogLevel level, const char *str) {
  uint16_t color = TFT_WHITE;
  char *head = "";

  switch (level) {
    case info:
      color = TFT_BLUE;
      head = "[INFO]";
      break;
    case ok:
      color = TFT_GREEN;
      head = "[ OK ]";
      break;
    case warn:
      color = TFT_YELLOW;
      head = "[WARN]";
      break;
    case error:
      color = TFT_RED;
      head = "[EROR]";
      break;
  }

  if (tft != nullptr) {
    tft->setTextColor(color);
    tft->print(head);
    tft->setTextColor(TFT_WHITE);
    tft->print(" ");
    tft->println(str);
  }

  Serial.print(head);
  Serial.print(" ");
  Serial.println(str);
}

void printLog(TFT_eSPI *tft, LogLevel level, String str) {
  printLog(tft, level, str.c_str());
}

void printStellaIT(TFT_eSPI *tft) {
  tft->setTextColor(TFT_SKYBLUE);
  tft->print("St");
  tft->setTextColor(TFT_BLUE);
  tft->print("el");
  tft->setTextColor(TFT_VIOLET);
  tft->print("la ");
  tft->setTextColor(TFT_MAGENTA);
  tft->print("IT");

}

void printStellaITInc(TFT_eSPI *tft) {
  tft->setTextColor(TFT_SKYBLUE);
  tft->print("Ste");
  tft->setTextColor(TFT_BLUE);
  tft->print("lla ");
  tft->setTextColor(TFT_VIOLET);
  tft->print("IT I");
  tft->setTextColor(TFT_MAGENTA);
  tft->print("nc.");
}

void showSplash(fs::FS fs, SketchCallback callback) {
  bool splashStarted = false;
  int frame = 0;

  TJpgDec.setCallback(callback);

  while (true) {
    char fileName[40] = {0,};
    sprintf(fileName, "/splash/frame_%05d.jpg", frame);

    if (fs.exists(fileName)) {
      splashStarted = true;
      TJpgDec.drawJpg(0, 0, fileName);
    } else {
      if (splashStarted) {
        break;
      }
    }

    frame++;
  }
}
