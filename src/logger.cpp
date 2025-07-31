#include "logger.h"

void log(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Serial.print(buffer);
    
    File logFile = SD.open("/log.txt", FILE_WRITE);
    if (logFile) {
        #ifdef LOGGER_TIMESTAMP
        unsigned long ms = millis();
        unsigned long hours = ms / 3600000;
        unsigned long minutes = (ms % 3600000) / 60000;
        unsigned long seconds = (ms % 60000) / 1000;
        unsigned long millis_part = ms % 1000;
        char timeBuffer[16];
        snprintf(timeBuffer, sizeof(timeBuffer), "%02lu:%02lu:%02lu:%03lu", hours, minutes, seconds, millis_part);
        logFile.print(timeBuffer);
        #endif
        logFile.print(buffer);
        logFile.close();
    } else {
        Serial.println("Failed to open log file on SD card.");
    }
}