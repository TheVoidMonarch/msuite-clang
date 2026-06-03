#include "sdl_handler.h"
#include <stdio.h>
#include <string.h>
#include <time.h> // For time functions
#include <stdlib.h> // For atoi

SDL_Window* init_sdl_video(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Video could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("MasjidSuite - Prayer Times", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    return window;
}

int init_sdl_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL Audio could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        SDL_Quit();
        return 0;
    }
    return 1;
}

void quit_sdl_audio() {
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    if (font == NULL) {
        fprintf(stderr, "Font is NULL in render_text!\n");
        return;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface == NULL) {
        fprintf(stderr, "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        fprintf(stderr, "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

void play_azan(const char* audio_path) {
    static Mix_Music* current_azan = NULL;
    if (current_azan != NULL) {
        Mix_FreeMusic(current_azan);
    }
    current_azan = Mix_LoadMUS(audio_path);
    if (current_azan == NULL) {
        fprintf(stderr, "Failed to load azan music (%s)! SDL_mixer Error: %s\n", audio_path, Mix_GetError());
        return;
    }
    Mix_PlayMusic(current_azan, 1);
}

int is_azan_playing() {
    return Mix_PlayingMusic();
}

static int parse_time_string(const char* time_str, struct tm* tm_out) {
    int hour, minute;
    if (sscanf(time_str, "%d:%d", &hour, &minute) == 2) {
        tm_out->tm_hour = hour;
        tm_out->tm_min = minute;
        tm_out->tm_sec = 0;
        return 1;
    }
    return 0;
}

void displayGraphicalPrayerTimesLocal(PrayerTimes pt) {
    SDL_Window* window = init_sdl_video(800, 600);
    if (window == NULL) {
        fprintf(stderr, "Failed to initialize SDL window for graphical display.\n");
        return;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Failed to create SDL renderer! SDL Error: %s\n", SDL_GetError());
        close_sdl_video(window, NULL, NULL);
        return;
    }

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24); // Use a default font
    if (font == NULL) {
        fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        close_sdl_video(window, renderer, NULL);
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255}; // White color

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1; // Allow escape to exit graphical display
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        char buffer[100];
        int y_offset = 50;

        // Display all prayer times
        sprintf(buffer, "Fajr: %s", pt.fajr);
        render_text(renderer, font, buffer, 50, y_offset, textColor);
        y_offset += 50;
        sprintf(buffer, "Sunrise: %s", pt.sunrise);
        render_text(renderer, font, buffer, 50, y_offset, textColor);
        y_offset += 50;
        sprintf(buffer, "Dhuhr: %s", pt.dhuhr);
        render_text(renderer, font, buffer, 50, y_offset, textColor);
        y_offset += 50;
        sprintf(buffer, "Asr: %s", pt.asr);
        render_text(renderer, font, buffer, 50, y_offset, textColor);
        y_offset += 50;
        sprintf(buffer, "Maghrib: %s", pt.maghrib);
        render_text(renderer, font, buffer, 50, y_offset, textColor);
        y_offset += 50;
        sprintf(buffer, "Isha: %s", pt.isha);
        render_text(renderer, font, buffer, 50, y_offset, textColor);
        y_offset += 50;

        // Countdown logic
        time_t rawtime;
        struct tm *info;
        time(&rawtime);
        info = localtime(&rawtime);

        const char* prayer_names[] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha"};
        const char* prayer_times_str[] = {pt.fajr, pt.sunrise, pt.dhuhr, pt.asr, pt.maghrib, pt.isha};

        time_t current_time_sec = mktime(info);
        time_t next_prayer_time_sec = 0;
        const char* next_prayer_name = "N/A";

        for (int i = 0; i < 6; ++i) {
            struct tm prayer_tm = *info; // Copy current date/year/month
            if (parse_time_string(prayer_times_str[i], &prayer_tm)) {
                time_t prayer_sec = mktime(&prayer_tm);
                if (prayer_sec > current_time_sec) {
                    next_prayer_time_sec = prayer_sec;
                    next_prayer_name = prayer_names[i];
                    break;
                }
            }
        }

        // If no prayer found for today, check for tomorrow's Fajr
        if (next_prayer_time_sec == 0) {
            struct tm tomorrow_info = *info;
            tomorrow_info.tm_mday += 1; // Advance to next day
            mktime(&tomorrow_info); // Normalize tm_mday, tm_mon, tm_year

            struct tm fajr_tomorrow_tm = tomorrow_info;
            if (parse_time_string(pt.fajr, &fajr_tomorrow_tm)) {
                next_prayer_time_sec = mktime(&fajr_tomorrow_tm);
                next_prayer_name = "Fajr (Tomorrow)";
            }
        }

        if (next_prayer_time_sec != 0) {
            double diff_sec = difftime(next_prayer_time_sec, current_time_sec);
            int hours = (int)(diff_sec / 3600);
            int minutes = (int)((diff_sec - (hours * 3600)) / 60);
            int seconds = (int)(diff_sec - (hours * 3600) - (minutes * 60));

            sprintf(buffer, "Next Prayer: %s", next_prayer_name);
            render_text(renderer, font, buffer, 50, y_offset + 50, textColor);
            sprintf(buffer, "Countdown: %02d:%02d:%02d", hours, minutes, seconds);
            render_text(renderer, font, buffer, 50, y_offset + 100, textColor);
        } else {
            sprintf(buffer, "Next Prayer: N/A");
            render_text(renderer, font, buffer, 50, y_offset + 50, textColor);
            sprintf(buffer, "Countdown: --:--:--");
            render_text(renderer, font, buffer, 50, y_offset + 100, textColor);
        }

        SDL_RenderPresent(renderer);
    }

    close_sdl_video(window, renderer, font);
}

void close_sdl_video(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font) {
    if (font != NULL) {
        TTF_CloseFont(font);
    }
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
