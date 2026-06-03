# MasjidSuite: A C-based Mosque Management System

## What it is
MasjidSuite is a group project developed for a first-semester course in Basic Programming in C language. It aims to provide a comprehensive system for mosque management, focusing on automating prayer time displays and integrating administrative functionalities. The system is designed to be modular and extensible, serving as a robust foundation for further development.

## Project Structure
The project is organized into the following directories:

*   `src/`: Contains all the C source code (`.c` and `.h` files).
*   `assets/`: Stores multimedia assets such as Azan audio files.
*   `README.md`: This documentation file.
*   `masjidsuite`: The compiled executable program.

## Libraries Used
The following external libraries are integral to the functionality of MasjidSuite:

*   **libcurl** [1]: A client-side URL transfer library, used for making HTTP requests to fetch prayer times from an external API.
*   **cJSON** [2]: An ultralightweight JSON parser in ANSI C, used for parsing the JSON responses received from the prayer time API.
*   **SDL2 (Simple DirectMedia Layer)** [3]: A cross-platform development library designed to provide low-level access to audio, keyboard, mouse, joystick, and graphics hardware. It is used for creating the graphical display window.
*   **SDL2_ttf** [4]: An add-on library for SDL2 that provides TrueType font rendering capabilities, used for displaying text (prayer times) on the graphical interface.
*   **SDL2_mixer** [5]: An add-on library for SDL2 that provides audio mixing capabilities, intended for playing Azan audio.

## Explanation for the Source Code and How it Diverges from Original Planning
The MasjidSuite project is structured into three primary C source files, located in the `src/` directory:

*   `main.c`: This file contains the main program logic, including the console-based menu system for user interaction and administrative tasks. It orchestrates calls to the `api_handler` for data retrieval and the `sdl_handler` for graphical presentation.
*   `api_handler.c`: This module is responsible for all external API interactions. It uses `libcurl` to construct and send HTTP requests to the Aladhan Prayer Times API and `cJSON` to parse the returned JSON data, extracting relevant prayer times. It returns a `PrayerTimes` struct containing the fetched times.
*   `sdl_handler.c`: This module encapsulates all SDL2-related functionalities. It handles the initialization and shutdown of SDL2, SDL2_ttf, and SDL2_mixer. It provides functions for creating a graphical window, rendering text, and playing audio (Azan).

### Divergence from Original Planning
Initially, the project planning included the implementation of astronomical algorithms in C to calculate prayer times. However, to ensure **accuracy** and **feasibility** within the scope of a first-semester project, a significant divergence was made: the system now relies on the [Aladhan Prayer Times API](https://aladhan.com/prayer-times-api) for fetching prayer times. This approach simplifies the core logic, allowing the team to focus on system integration, user interface design, and other rubric requirements. The graphical display, while fully coded, is intended for local execution on the user's machine due to the limitations of the sandbox environment, which cannot render graphical windows or play audio directly.

## Features

*   **Dynamic Prayer Times**: Fetches accurate, up-to-date prayer times for any specified city and country using a reliable external API.
*   **Console-Based Menu System**: Provides a clear, interactive menu for users to view prayer times and access administrative settings.
*   **Graphical Display (Local)**: Offers an optional graphical interface to display prayer times, suitable for external displays in a mosque. This feature leverages SDL2 for rendering.
*   **Azan Audio Playback (Planned)**: Integrated `SDL2_mixer` for playing Azan audio at designated prayer times, enhancing the system's automation capabilities.
*   **Modular Architecture**: The codebase is organized into distinct modules (`main`, `api_handler`, `sdl_handler`), promoting maintainability and scalability.
*   **Robust Input Handling**: Implements safe input mechanisms to prevent common programming errors and improve user experience.
*   **Live Countdown Timer**: The graphical display now includes a live countdown to the next prayer, dynamically updating every second.
*   **Test Azan Feature**: A menu option to test Azan playback, simulating a prayer call 30 seconds from the current time.

## How to Use It

To compile and run the MasjidSuite project on your local Linux system (or Windows Subsystem for Linux - WSL), follow these steps:

1.  **Extract the Project Files**: Unzip the provided `MasjidSuite.zip` file to your desired directory.

2.  **Install Dependencies**: Open your terminal and install the necessary development libraries. These commands are for Debian/Ubuntu-based systems:

    ```bash
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev libcjson-dev libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev build-essential
    ```

3.  **Compile the Project**: Navigate to the project's root directory (where `src/` and `assets/` are located) and compile the source files using `gcc`. Ensure all `.c` files are included and the libraries are linked correctly, specifying the `src` directory for includes:

    ```bash
    gcc -o masjidsuite src/main.c src/api_handler.c src/sdl_handler.c src/logger.c -Isrc -lcurl -lcjson -lSDL2 -lSDL2_ttf -lSDL2_mixer -I/usr/include/cjson
    ```

4.  **Run the Program**: Execute the compiled program from the project's root directory:

    ```bash
    ./masjidsuite
    ```

    The program will start with a console-based menu. You can choose to view prayer times in the console or launch the graphical display.

### Important Note on Graphical Display and Audio

The graphical display and audio playback features (using SDL2, SDL2_ttf, and SDL2_mixer) are designed to run on a local machine with a graphical environment and audio output. These features **will not function** within a headless or terminal-only environment like the sandbox where this documentation was generated. To experience the full functionality, including the visual prayer time display and Azan audio, please run the compiled `masjidsuite` executable on your personal computer.

## References
[1] [libcurl - C library for client-side URL transfers](https://curl.se/libcurl/)

[2] [cJSON - Ultralightweight JSON parser in ANSI C](https://github.com/DaveGamble/cJSON)

[3] [SDL2 - Simple DirectMedia Layer](https://www.libsdl.org/)

[4] [SDL_ttf - TrueType Font Rendering Library for SDL](https://www.libsdl.org/projects/SDL_ttf/)

[5] [SDL_mixer - Audio Mixer Library for SDL](https://www.libsdl.org/projects/SDL_mixer/)
