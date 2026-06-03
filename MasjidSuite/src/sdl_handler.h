#ifndef SDL_HANDLER_H
#define SDL_HANDLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "api_handler.h"

// Function to initialize SDL video and create a window
SDL_Window* init_sdl_video(int width, int height);

// Function to initialize SDL audio
int init_sdl_audio();

// Function to quit SDL audio
void quit_sdl_audio();

// Function to render text on the SDL window
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color);

// Function to play Azan audio
void play_azan(const char* audio_path);

// Function to check if azan is currently playing
int is_azan_playing();

// Function to display prayer times graphically (for local execution)
void displayGraphicalPrayerTimesLocal(PrayerTimes pt);

// Function to close SDL video
void close_sdl_video(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);

#endif // SDL_HANDLER_H
