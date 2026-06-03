#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include <time.h>
#include "api_handler.h"

// Structure to hold the API response
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function for libcurl to write received data
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

PrayerTimes fetch_prayer_times(const char *city, const char *country, int method) {
    CURL *curl_handle;
    CURLcode res;
    PrayerTimes pt = {"N/A", "N/A", "N/A", "N/A", "N/A", "N/A"};

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by realloc */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    char url[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char *encoded_city = curl_easy_escape(curl_handle, city, 0);
    char *encoded_country = curl_easy_escape(curl_handle, country, 0);

    if (!encoded_city || !encoded_country) {
        fprintf(stderr, "Failed to URL-encode city or country.\n");
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);
        curl_global_cleanup();
        return pt;
    }

    snprintf(url, sizeof(url), "https://api.aladhan.com/v1/timingsByCity/%02d-%02d-%d?city=%s&country=%s&method=%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, encoded_city, encoded_country, method);

    curl_free(encoded_city);
    curl_free(encoded_country);

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        /*
         * Now, parse the JSON response using cJSON
         */
        cJSON *json = cJSON_Parse(chunk.memory);
        if (json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
            goto end;
        }

        cJSON *data = cJSON_GetObjectItemCaseSensitive(json, "data");
        if (data == NULL) goto end;

        cJSON *timings = cJSON_GetObjectItemCaseSensitive(data, "timings");
        if (timings == NULL) goto end;

        cJSON *fajr = cJSON_GetObjectItemCaseSensitive(timings, "Fajr");
        if (cJSON_IsString(fajr) && (fajr->valuestring != NULL)) {
            strncpy(pt.fajr, fajr->valuestring, sizeof(pt.fajr) - 1);
            pt.fajr[sizeof(pt.fajr) - 1] = '\0';
        }
        cJSON *sunrise = cJSON_GetObjectItemCaseSensitive(timings, "Sunrise");
        if (cJSON_IsString(sunrise) && (sunrise->valuestring != NULL)) {
            strncpy(pt.sunrise, sunrise->valuestring, sizeof(pt.sunrise) - 1);
            pt.sunrise[sizeof(pt.sunrise) - 1] = '\0';
        }
        cJSON *dhuhr = cJSON_GetObjectItemCaseSensitive(timings, "Dhuhr");
        if (cJSON_IsString(dhuhr) && (dhuhr->valuestring != NULL)) {
            strncpy(pt.dhuhr, dhuhr->valuestring, sizeof(pt.dhuhr) - 1);
            pt.dhuhr[sizeof(pt.dhuhr) - 1] = '\0';
        }
        cJSON *asr = cJSON_GetObjectItemCaseSensitive(timings, "Asr");
        if (cJSON_IsString(asr) && (asr->valuestring != NULL)) {
            strncpy(pt.asr, asr->valuestring, sizeof(pt.asr) - 1);
            pt.asr[sizeof(pt.asr) - 1] = '\0';
        }
        cJSON *maghrib = cJSON_GetObjectItemCaseSensitive(timings, "Maghrib");
        if (cJSON_IsString(maghrib) && (maghrib->valuestring != NULL)) {
            strncpy(pt.maghrib, maghrib->valuestring, sizeof(pt.maghrib) - 1);
            pt.maghrib[sizeof(pt.maghrib) - 1] = '\0';
        }
        cJSON *isha = cJSON_GetObjectItemCaseSensitive(timings, "Isha");
        if (cJSON_IsString(isha) && (isha->valuestring != NULL)) {
            strncpy(pt.isha, isha->valuestring, sizeof(pt.isha) - 1);
            pt.isha[sizeof(pt.isha) - 1] = '\0';
        }

    end:
        cJSON_Delete(json);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    free(chunk.memory);

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();

    return pt;
}
