#ifndef SDL_HANDLER_H
#define SDL_HANDLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include "api_handler.h"

// Video functions
SDL_Window* init_sdl_video(int width, int height);
void close_sdl_video(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);

// Audio functions
int init_sdl_audio();
void quit_sdl_audio();
void play_azan(const char* audio_path);
int is_azan_playing();
void stop_azan();   // NEW

// Text rendering
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text,
                 int x, int y, SDL_Color color);

// Graphical prayer times
void displayGraphicalPrayerTimesLocal(PrayerTimes pt);

#endif // SDL_HANDLER_H
