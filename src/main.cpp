#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Player.h"
#include "World.h"
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iostream>

int main(int, char*[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL2 не смог запуститься! Ошибка: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Wario",                  // Заголовок окна
        SDL_WINDOWPOS_CENTERED,           // Положение X (по центру)
        SDL_WINDOWPOS_CENTERED,           // Положение Y (по центру)
        1000, 704,                        // Ширина и высота окна
        SDL_WINDOW_SHOWN                  // Показать окно сразу
    );

    if (!window) {
        std::cout << "Окно не создалось! Ошибка: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cout << "Рендерер не создался! Ошибка: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_Image не запустилась" << IMG_GetError() << '\n';
        SDL_Quit();
        return EXIT_FAILURE;
    }
    //Window
    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    //World
    World world;
    const std::string levelPath = "assets/level1.txt";
    if (!world.load(levelPath)) {
        std::string msg = "Не найден: " + levelPath;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Ошибка", msg.c_str(), window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    //Textures
    world.loadTextures(renderer);
    //Player
    Player player(30, 30);
    player.loadTexture(renderer);
    //DeltaTime
    Uint64 lastTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    //isRunning
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(currentTime - lastTime) / static_cast<float>(frequency);
        lastTime = currentTime;

        if (dt > 0.1f) dt = 0.1f;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            player.handleEvent(event);
        }
        player.update(dt, winW, winH, world.getSolids(), world.getPlatforms());

        SDL_SetRenderDrawColor(renderer, 107, 140, 255, 255);
        SDL_RenderClear(renderer);
        world.draw(renderer);

        player.draw(renderer);
        SDL_RenderPresent(renderer);
    }
    world.freeTextures();
    player.freeTexture();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
