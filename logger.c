//Student: Joshua Gaynor ID: 20549366

#include "logger.h"
#include "utilities.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

int count = 0;

void logger_start() {
}

void logger_stop() {
}

void logger_write(char const* message) {
    time_t now;
    struct tm * timeinfo;
    time( &now );
    timeinfo = localtime( &now );
    printf("%d : %02d:%02d:%02d : %s\n", count, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);
    count++;
}
