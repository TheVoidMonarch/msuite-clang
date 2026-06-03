#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "api_handler.h"
#include <SDL2/SDL.h>
#include "sdl_handler.h"
#include "logger.h"

// ANSI escape codes for colors
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define MAGENTA "\033[1;35m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

// Signal handler for Ctrl+C
void handle_sigint(int sig) {
    printf("\nCTRL+C pressed — stopping Azan playback...\n");
    stop_azan();
    quit_sdl_audio();
    // Do not exit program, just stop audio and return to menu
}

void displayMainMenu() {
    printf("\n%s%s==============================%s\n", BOLD, CYAN, RESET);
    printf("%s%s     MASJIDSUITE MAIN MENU        %s\n", BOLD, CYAN, RESET);
    printf("%s%s==============================%s\n", BOLD, CYAN, RESET);

    printf("%s-Basics of Programming Mini Project-%s\n", YELLOW, RESET);
    printf("%s  Group A7X: Haziq, Imran, Zuan%s\n\n", YELLOW, RESET);

    printf("%s1.%s View Prayer Times (Console)\n", RESET, CYAN);
    printf("%s2.%s View Prayer Times (Graphical - Local Only)\n", RESET, YELLOW);
    printf("%s3.%s Test Azan\n", RESET, BLUE);
    printf("%s4.%s Admin Settings\n", RESET, GREEN);
    printf("%s5.%s SysLog\n", RESET, MAGENTA);
    printf("%s6.%s Exit\n", RESET, RED);

    printf("%sEnter your choice: %s", BOLD, RESET);
}

void displaySysLog() {
    FILE* file = fopen("syslog.txt", "r");
    if (file == NULL) {
        printf("No log file found.\n");
        return;
    }

    printf("\n--- System Log ---\n");
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    printf("--- End of Log ---\n");
    fclose(file);
}

void displayAdminMenu() {
    printf("\nAdmin Settings\n");
    printf("1. Configure Prayer Times (City/Country/Method)\n");
    printf("2. Manage Qariyah Database\n");
    printf("3. Backup/Restore Data\n");
    printf("4. Back to Main Menu\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;
    int adminChoice;
    char input_buffer[10]; // Buffer for user input

    signal(SIGINT, handle_sigint);   // register Ctrl+C handler
    log_message("USAGE", "Program started");

    do {
        displayMainMenu();
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            input_buffer[strcspn(input_buffer, "\n")] = 0;
            choice = atoi(input_buffer);
            char log_buf[256];
            snprintf(log_buf, sizeof(log_buf), "User selected main menu option: %d", choice);
            log_message("USAGE", log_buf);
        } else {
            choice = 0; // Invalid input
            log_message("WARNING", "Failed to read main menu choice or EOF reached");
        }

        switch (choice) {
            case 1:
                printf("Fetching and displaying prayer times for your location (console)...\n");
                // ✅ Use method 17 here
                PrayerTimes console_pt = fetch_prayer_times(NULL, NULL, 17);
                printf("Fajr: %s\n", console_pt.fajr);
                printf("Sunrise: %s\n", console_pt.sunrise);
                printf("Dhuhr: %s\n", console_pt.dhuhr);
                printf("Asr: %s\n", console_pt.asr);
                printf("Maghrib: %s\n", console_pt.maghrib);
                printf("Isha: %s\n", console_pt.isha);
                break;
            case 2:
                printf("Launching graphical prayer time display...\n");
                // ✅ Use method 17 here
                PrayerTimes graphical_pt = fetch_prayer_times(NULL, NULL, 17);
                displayGraphicalPrayerTimesLocal(graphical_pt);
                break;
            case 3:
                printf("Testing Azan playback (press Ctrl+C to stop)...\n");
                if (init_sdl_audio()) {
                    play_azan("assets/Adhan.mp3");
                    while (is_azan_playing()) {
                        SDL_Delay(100);
                    }
                    quit_sdl_audio();
                } else {
                    printf("Failed to initialize audio system.\n");
                }
                break;
            case 4:
                do {
                    displayAdminMenu();
                    if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
                        input_buffer[strcspn(input_buffer, "\n")] = 0;
                        adminChoice = atoi(input_buffer);
                    } else {
                        adminChoice = 0; // Invalid input
                    }

                    switch (adminChoice) {
                        case 1: printf("Configuring prayer times...\n"); break;
                        case 2: printf("Managing Qariyah Database...\n"); break;
                        case 3: printf("Backup/Restore Data...\n"); break;
                        case 4: printf("Returning to Main Menu...\n"); break;
                        default:
                            printf("Invalid choice. Please try again.\n");
                            log_message("WARNING", "Invalid admin menu choice entered");
                    }
                    char log_buf[256];
                    snprintf(log_buf, sizeof(log_buf), "User selected admin menu option: %d", adminChoice);
                    log_message("USAGE", log_buf);
                } while (adminChoice != 4);
                break;
            case 5:
                displaySysLog();
                break;
            case 6:
                printf("Exiting MasjidSuite. Goodbye!\n");
                log_message("USAGE", "Program exited by user");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                log_message("WARNING", "Invalid main menu choice entered");
        }
    } while (choice != 6);

    return 0;
}
