#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "Entity.h"
#include "Physics.h"
#include "Input.h"

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

    const float START_X = 100.0f;
    const float START_Y = 100.0f;

    // Create one generic entity
    Entity player(START_X, START_Y, 200.0f, 200.0f);


    // Create a Physics instance
    Physics physics;

    // Enable gravity for the player
    player.setGravityEnabled(true);
    physics.setGravity(900.0f); // Set gravity strength (pixels per second squared)

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

    // Load the brick
    SDL_Texture* brickTexture =
        IMG_LoadTexture(
            renderer,
            "../assets/brick.png"
        );

    if (!brickTexture) {
        SDL_Log(
            "Could not load brick texture: %s",
            SDL_GetError()
        );
    }

    int actualWidth;
    int actualHeight;

    SDL_GetWindowSize(
        window,
        &actualWidth,
        &actualHeight
    );

    const float GROUND_Y = actualHeight - 150.0f;

    const float BRICK_WIDTH = 96.0f;
    const float BRICK_HEIGHT = 32.0f;

    // Area with no bricks
    const float GAP_START = 900.0f;
    const float GAP_END = 1200.0f;

    bool running = true;
    SDL_Event event;

    Uint64 lastTime = SDL_GetTicks();
    // Main game loop
    while (running) {

        // Check if user closes window
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // A: walk left
        // D: walk right
        // W: jump
        // S: crouch
        // Space: Attack
        // W + A: jump left
        // W + D: jump right
        // Shift + A: run left
        // Shift + D: run right


        // Input for Jumping (W key)
        if (Input::isKeyPressed(SDL_SCANCODE_W)) {
            physics.jump(player, 650.0f);
        }

        const float WALK_SPEED = 300.0f;
        const float RUN_SPEED = 550.0f;
        float moveSpeed = WALK_SPEED;

        // Input for moving left and right (A and D keys)
        if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ||
            Input::isKeyPressed(SDL_SCANCODE_RSHIFT)) {
            moveSpeed = RUN_SPEED;
        }

        if (Input::isKeyPressed(SDL_SCANCODE_A)) {
            player.move(-moveSpeed * deltaTime, 0.0f);
        }

        if (Input::isKeyPressed(SDL_SCANCODE_D)) {
            player.move(moveSpeed * deltaTime, 0.0f);
        }

        // Input for crouching (S key)
        if (Input::isKeyPressed(SDL_SCANCODE_S)) {
            SDL_Log("S pressed - down/crouch action");
        }

        // Input for attacking (Space key)
        if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
            SDL_Log("Attack!");
        }

        // Update physics
        physics.update(player, deltaTime);

        // Ground check
        float playerLeft = player.getX();

        float playerRight = player.getX() + player.getWidth();

        bool overGap = playerRight > GAP_START && playerLeft < GAP_END;

        // If player is NOT over the gap,
        // let them land on the brick ground
        if (
            !overGap &&
            player.getY() + player.getHeight() >= GROUND_Y &&
            player.getVelocityY() >= 0.0f
        ) {

            player.setPosition(
                player.getX(),
                GROUND_Y - player.getHeight()
            );

            player.setVelocityY(0.0f);

            player.setGrounded(true);
        }
        else {
            player.setGrounded(false);
        }

        // Fail/Reset
        if (player.getY() > actualHeight) {

            SDL_Log(
                "Player fell! Resetting to spawn point."
            );

            player.setPosition(
                START_X,
                START_Y
            );

            player.setVelocity(
                0.0f,
                0.0f
            );

            player.setGrounded(false);
        }

        // Set background color to sage green
        SDL_SetRenderDrawColor(renderer, 169, 186, 157, 255);

        // Clear previous frame
        SDL_RenderClear(renderer);

        // Render the brick ground
        if (brickTexture) {

            for (
                float x = 0.0f;
                x < actualWidth;
                x += BRICK_WIDTH
            ) {

                // Leave a gap in the ground
                if (
                    x + BRICK_WIDTH > GAP_START &&
                    x < GAP_END
                ) {
                    continue;
                }

                SDL_FRect brickRect = {
                    x,
                    GROUND_Y,
                    BRICK_WIDTH,
                    BRICK_HEIGHT
                };

                SDL_RenderTexture(
                    renderer,
                    brickTexture,
                    nullptr,
                    &brickRect
                );
            }
        }

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
    if (brickTexture) {
        SDL_DestroyTexture(brickTexture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}