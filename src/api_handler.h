#ifndef API_HANDLER_H
#define API_HANDLER_H

typedef struct {
    char fajr[6];
    char sunrise[6];
    char dhuhr[6];
    char asr[6];
    char maghrib[6];
    char isha[6];
} PrayerTimes;

// Function to fetch and parse prayer times from the API
// If city and country are NULL, it will attempt to fetch by IP address
PrayerTimes fetch_prayer_times(const char *city, const char *country, int method);

#endif // API_HANDLER_H
