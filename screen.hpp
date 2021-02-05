#ifndef _SCREEN_HPP

#define _SCREEN_HPP
enum LogLevel {info, ok, warn, error, none};

int getRotation(int degrees);
void initScreenForCLI(TFT_eSPI *tft, int degrees);
void logScreen(TFT_eSPI *tft, LogLevel level, const char *str);
void printLog(TFT_eSPI *tft, LogLevel level, String str);
void printStellaIT(TFT_eSPI *tft);
void printStellaITInc(TFT_eSPI *tft);
void showSplash();

#endif