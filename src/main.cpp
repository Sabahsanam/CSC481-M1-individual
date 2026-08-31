#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "Entity.h"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Could not initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create window and renderer
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer(
            "Game Engine",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE,
            &window,
            &renderer)) {

        SDL_Log("Could not create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create one generic entity
    Entity player(100.0f, 100.0f, 200.0f, 200.0f);

    // Load the player's sprite texture
    SDL_Texture* playerTexture =
        IMG_LoadTexture(renderer, "../assets/darkworld_enemy_nyx_idle.png");

    // Check if the texture loaded correctly
    if (!playerTexture) {
        SDL_Log("Could not load texture: %s", SDL_GetError());
    }
    else {
        player.setTexture(playerTexture);

        // Tell the entity how the sprite sheet is arranged
        player.setSpriteSheet(8, 128, 128);
    }

    bool running = true;
    SDL_Event event;

    // Main game loop
    while (running) {

        // Check if user closes window
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Set background color to sage green
        SDL_SetRenderDrawColor(renderer, 169, 186, 157, 255);

        // Clear previous frame
        SDL_RenderClear(renderer);

        // Update the sprite animation
        player.updateAnimation();

        // Render the entity
        player.render(renderer);

        // Show the frame
        SDL_RenderPresent(renderer);
    }

    // Clean up
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}