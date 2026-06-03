#include "sdl_handler.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "logger.h"

SDL_Window* init_sdl_video(int width, int height) {
    log_message("INFO", "Initializing SDL video...");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        char err_buf[256];
        snprintf(err_buf, sizeof(err_buf), "SDL Video could not initialize! SDL_Error: %s", SDL_GetError());
        log_message("ERROR", err_buf);
        fprintf(stderr, "%s\n", err_buf);
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("MasjidSuite - Prayer Times",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width, height,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        char err_buf[256];
        snprintf(err_buf, sizeof(err_buf), "Window could not be created! SDL_Error: %s", SDL_GetError());
        log_message("ERROR", err_buf);
        fprintf(stderr, "%s\n", err_buf);
        SDL_Quit();
        return NULL;
    }

    if (TTF_Init() == -1) {
        char err_buf[256];
        snprintf(err_buf, sizeof(err_buf), "SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
        log_message("ERROR", err_buf);
        fprintf(stderr, "%s\n", err_buf);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    if (!(IMG_Init(IMG_INIT_XPM) & IMG_INIT_XPM)) {
        char err_buf[256];
        snprintf(err_buf, sizeof(err_buf), "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
        log_message("ERROR", err_buf);
        fprintf(stderr, "%s\n", err_buf);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    return window;
}

int init_sdl_audio() {
    log_message("INFO", "Initializing SDL audio...");
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        char err_buf[256];
        snprintf(err_buf, sizeof(err_buf), "SDL Audio could not initialize! SDL_Error: %s", SDL_GetError());
        log_message("ERROR", err_buf);
        fprintf(stderr, "%s\n", err_buf);
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        char err_buf[256];
        snprintf(err_buf, sizeof(err_buf), "SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        log_message("ERROR", err_buf);
        fprintf(stderr, "%s\n", err_buf);
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
    log_message("INFO", "Stopping Azan playback...");
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

    // Load border image from assets
    SDL_Surface* borderSurface = IMG_Load("assets/border.xpm");
    if (borderSurface) {
        SDL_Texture* borderTexture = SDL_CreateTextureFromSurface(renderer, borderSurface);
        SDL_FreeSurface(borderSurface);
        SDL_RenderCopy(renderer, borderTexture, NULL, NULL);
        SDL_DestroyTexture(borderTexture);
    }

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return;
    }

    SDL_Color textColor = {0, 0, 0, 255}; // black text
    char buffer[128];
    int y_offset = 100;

    const char* labels[] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha"};
    const char* values[] = {pt.fajr, pt.sunrise, pt.dhuhr, pt.asr, pt.maghrib, pt.isha};

    for (int i = 0; i < 6; i++) {
        snprintf(buffer, sizeof(buffer), "%s: %s", labels[i], values[i]);
        render_text(renderer, font, buffer, 200, y_offset, textColor);
        y_offset += 50;
    }

    SDL_RenderPresent(renderer);

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }
        }
        SDL_Delay(50);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
