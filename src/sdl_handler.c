#include "sdl_handler.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "logger.h"

// Helper to parse "HH:MM" into a struct tm
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

SDL_Window* init_sdl_video(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Video could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("MasjidSuite - Prayer Times",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width, height,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
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

    if (IMG_Init(0) < 0) {
        fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
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
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }
    return 1;
}

void quit_sdl_audio() {
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void stop_azan() {
    Mix_HaltMusic();
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text,
                 int x, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

void play_azan(const char* audio_path) {
    static Mix_Music* current_azan = NULL;
    if (current_azan) {
        Mix_FreeMusic(current_azan);
    }
    current_azan = Mix_LoadMUS(audio_path);
    if (!current_azan) {
        fprintf(stderr, "Failed to load azan: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayMusic(current_azan, 1);
}

int is_azan_playing() {
    return Mix_PlayingMusic();
}

void displayGraphicalPrayerTimesLocal(PrayerTimes pt) {
    SDL_Window* window = init_sdl_video(800, 600);
    if (!window) return;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        return;
    }

    // Load custom fonts
    TTF_Font* nextFont = TTF_OpenFont("assets/Basmala.ttf", 32); // Next Prayer
    TTF_Font* listFont = TTF_OpenFont("assets/Yulltan.ttf", 36);  // Prayer list
    if (!nextFont || !listFont) {
        fprintf(stderr, "Failed to load custom fonts!\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return;
    }

    SDL_Color textColor = {0, 0, 0, 255};
    char buffer[128];

    const char* labels[] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha"};
    const char* values[] = {pt.fajr, pt.sunrise, pt.dhuhr, pt.asr, pt.maghrib, pt.isha};

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }
        }

        SDL_RenderClear(renderer);

        // Border background
        SDL_Surface* borderSurface = IMG_Load("assets/border.xpm");
        if (borderSurface) {
            SDL_Texture* borderTexture = SDL_CreateTextureFromSurface(renderer, borderSurface);
            SDL_FreeSurface(borderSurface);
            SDL_RenderCopy(renderer, borderTexture, NULL, NULL);
            SDL_DestroyTexture(borderTexture);
        }

        // Countdown logic
        time_t rawtime;
        struct tm *info;
        time(&rawtime);
        info = localtime(&rawtime);

        time_t current_time_sec = mktime(info);
        time_t next_prayer_time_sec = 0;
        const char* next_prayer_name = "N/A";

        for (int i = 0; i < 6; ++i) {
            struct tm prayer_tm = *info;
            if (parse_time_string(values[i], &prayer_tm)) {
                time_t prayer_sec = mktime(&prayer_tm);
                if (prayer_sec > current_time_sec) {
                    next_prayer_time_sec = prayer_sec;
                    next_prayer_name = labels[i];
                    break;
                }
            }
        }

        if (next_prayer_time_sec == 0) {
            struct tm tomorrow_info = *info;
            tomorrow_info.tm_mday += 1;
            mktime(&tomorrow_info);
            struct tm fajr_tomorrow_tm = tomorrow_info;
            if (parse_time_string(pt.fajr, &fajr_tomorrow_tm)) {
                next_prayer_time_sec = mktime(&fajr_tomorrow_tm);
                next_prayer_name = "Fajr (Tomorrow)";
            }
        }

        if (next_prayer_time_sec != 0) {
            double diff_sec = difftime(next_prayer_time_sec, current_time_sec);
            int hours = (int)(diff_sec / 3600);
            int minutes = (int)((diff_sec - hours * 3600) / 60);
            int seconds = (int)(diff_sec - hours * 3600 - minutes * 60);

            snprintf(buffer, sizeof(buffer), "Next Prayer : %s ( %02d:%02d:%02d left )",
                     next_prayer_name, hours, minutes, seconds);
            
            render_text(renderer, nextFont, buffer, 10, 75, textColor);
        }

        
        int startX = 260;
        int startY = 170;
        int spacing = 40;

        for (int i = 0; i < 6; i++) {
            snprintf(buffer, sizeof(buffer), "%-8s\t%s", labels[i], values[i]);
            render_text(renderer, listFont, buffer, startX, startY + i * spacing, textColor);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1000);
    }

    TTF_CloseFont(nextFont);
    TTF_CloseFont(listFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
