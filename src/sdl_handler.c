void displayGraphicalPrayerTimesLocal(PrayerTimes pt) {
    SDL_Window* window = init_sdl_video(800, 600);
    if (!window) return;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        return;
    }

    // Custom font for Next Prayer
    TTF_Font* nextFont = TTF_OpenFont("assets/Basmala.ttf", 32);
    // Standard font for prayer list
    TTF_Font* listFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 28);

    if (!nextFont || !listFont) {
        fprintf(stderr, "Failed to load fonts!\n");
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
            render_text(renderer, nextFont, buffer, 170, 75, textColor);
        }

        // Prayer list with standard font
        int startX = 253; // shifted 7px left
        int startY = 170;
        int spacing = 40;

        for (int i = 0; i < 6; i++) {
            snprintf(buffer, sizeof(buffer), "%-8s %s", labels[i], values[i]);
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
