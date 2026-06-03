#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

void log_message(const char* level, const char* message) {
    FILE* file = fopen("syslog.txt", "a");
    if (file == NULL) {
        perror("Could not open syslog.txt");
        return;
    }

    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char date[26];
    if (tm_info == NULL || strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm_info) == 0) {
        strncpy(date, "N/A", sizeof(date));
    }

    fprintf(file, "[%s] [%s] %s\n", date, level, message);
    fclose(file);
}
