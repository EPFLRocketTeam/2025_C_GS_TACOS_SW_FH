#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <cstdarg>

#define DEBUG_SERIAL SerialUSB
#define DEBUG_SERIAL_BAUD 115200

#define SD_CARD_NSS BUILTIN_SDCARD
#define SD_INIT_MAX 10
void log(const char* format, ...);

#endif