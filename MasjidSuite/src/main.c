#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api_handler.h"
#include <SDL2/SDL.h> // Required for SDL_Delay

#include "sdl_handler.h"

// Forward declaration for graphical display (for user's local implementation)
void displayGraphicalPrayerTimesLocal(PrayerTimes pt);

void displayMainMenu() {
    printf("\nMasjidSuite Main Menu\n");
    printf("1. View Prayer Times (Console)\n");
    printf("2. View Prayer Times (Graphical - Local Only)\n");
    printf("3. Test Azan\n");
    printf("4. Admin Settings\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
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

    do {
        displayMainMenu();
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            // Remove trailing newline character if present
            input_buffer[strcspn(input_buffer, "\n")] = 0;
            choice = atoi(input_buffer);
        } else {
            choice = 0; // Invalid input
        }

        switch (choice) {
            case 1:
                printf("Fetching and displaying prayer times (console)...\n");
                PrayerTimes console_pt = fetch_prayer_times("London", "United Kingdom", 2);
                printf("Fajr: %s\n", console_pt.fajr);
                printf("Sunrise: %s\n", console_pt.sunrise);
                printf("Dhuhr: %s\n", console_pt.dhuhr);
                printf("Asr: %s\n", console_pt.asr);
                printf("Maghrib: %s\n", console_pt.maghrib);
                printf("Isha: %s\n", console_pt.isha);
                break;
            case 2:
                printf("Launching graphical prayer time display... (This will only work on your local machine with SDL2 installed)\n");
                PrayerTimes graphical_pt = fetch_prayer_times("London", "United Kingdom", 2);
                displayGraphicalPrayerTimesLocal(graphical_pt);
                printf("Graphical display closed. Press Enter to continue to main menu.\n");
                getchar(); // Wait for user to press Enter
                break;
            case 3:
                printf("Testing Azan playback (simulating a prayer call in 30 seconds)...\n");
                printf("Note: Audio playback requires local execution with SDL2_mixer.\n");
                
                if (init_sdl_audio()) {
                    printf("Waiting 30 seconds...\n");
                    // In a real scenario, we wouldn't block the main thread like this,
                    // but for a simple test, we use SDL_Delay.
                    SDL_Delay(30000); 
                    
                    printf("Playing Azan...\n");
                    // For test, we play the standard Adhan. You could add logic to choose Fajr.
                    play_azan("assets/Adhan.mp3");
                    
                    // Wait for the audio to finish playing
                    while (is_azan_playing()) {
                        SDL_Delay(100);
                    }
                    printf("Azan playback finished.\n");
                    quit_sdl_audio();
                } else {
                    printf("Failed to initialize audio system.\n");
                }
                break;
            case 4:
                do {
                    displayAdminMenu();
                    if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
                        // Remove trailing newline character if present
                        input_buffer[strcspn(input_buffer, "\n")] = 0;
                        adminChoice = atoi(input_buffer);
                    } else {
                        adminChoice = 0; // Invalid input
                    }

                    switch (adminChoice) {
                        case 1:
                            printf("Configuring prayer times...\n");
                            // TODO: Implement configuration logic
                            break;
                        case 2:
                            printf("Managing Qariyah Database...\n");
                            // TODO: Implement Qariyah database management
                            break;
                        case 3:
                            printf("Backup/Restore Data...\n");
                            // TODO: Implement backup/restore logic
                            break;
                        case 4:
                            printf("Returning to Main Menu...\n");
                            break;
                        default:
                            printf("Invalid choice. Please try again.\n");
                    }
                } while (adminChoice != 4);
                break;
            case 5:
                printf("Exiting MasjidSuite. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);

    return 0;
}


