#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "Entity.h"
#include "Physics.h"
#include "Input.h"
#include "Collision.h"
#include "Scaling.h"

// ============================================================
// SABAH'S INDIVIDUAL GAME
// Uses the shared team engine (Entity, Physics, Input,
// Collision, Scaling) but is a distinct game from the
// team demo: a platforming level built around a moving
// platform you must ride/jump across, plus a static crate
// that blocks your path.
// ============================================================

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// -----------------------------------------------------------
// TODO: Replace these with YOUR actual sprite sheet layouts.
// Frame counts/sizes below are placeholders — open each of
// your PNGs and fill in the real frame width/height and how
// many frames are laid out horizontally.
// -----------------------------------------------------------
const int PLAYER_FRAME_COUNT = 1;   // e.g. 8 if it's an 8-frame walk cycle
const int PLAYER_FRAME_W = 128;     // width of ONE frame, not the whole sheet
const int PLAYER_FRAME_H = 128;

const int CRATE_FRAME_COUNT = 1;    // static object, usually just 1 frame
const int CRATE_FRAME_W = 96;
const int CRATE_FRAME_H = 96;

const int PLATFORM_FRAME_COUNT = 1; // moving platform, usually just 1 frame
const int PLATFORM_FRAME_W = 160;
const int PLATFORM_FRAME_H = 32;

int main(int argc, char* argv[])
{
    // ---- Task 1 (engine): initialize SDL ----
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Could not initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer(
            "Sabah's Game - Individual Submission",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE,
            &window,
            &renderer)) {
        SDL_Log("Could not create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int actualWidth = 0;
    int actualHeight = 0;
    SDL_GetWindowSize(window, &actualWidth, &actualHeight);

    // ---- Task 6: scaling reference resolution ----
    Scaling::setReferenceResolution(actualWidth, actualHeight);
    bool scaleKeyWasPressed = false;

    const float GROUND_Y = actualHeight - 150.0f;
    const float START_X = 100.0f;
    const float START_Y = 100.0f;

    // ------------------------------------------------------
    // Task 2: three specific entities
    // ------------------------------------------------------

    // 1) Controllable object: the player
    Entity player(START_X, START_Y, 100.0f, 100.0f);
    player.setGravityEnabled(true);

    // 2) Static object: a crate blocking the path
    Entity crate(600.0f, GROUND_Y - 100.0f, 100.0f, 100.0f);
    crate.setGravityEnabled(false);

    // 3) Auto-moving object: a vertically patrolling platform
    //    following a continuous predefined path (up/down loop)
    Entity movingPlatform(1000.0f, GROUND_Y - 300.0f, 200.0f, 40.0f);
    movingPlatform.setGravityEnabled(false);
    const float PLATFORM_TOP_Y = GROUND_Y - 500.0f;
    const float PLATFORM_BOTTOM_Y = GROUND_Y - 150.0f;
    float platformSpeed = 120.0f;
    int platformDirection = -1; // start moving up

    // ---- Task 3: physics/gravity ----
    Physics physics;
    physics.setGravity(900.0f); // configurable, not hardcoded into Entity/Physics

    const float RESPAWN_X = START_X;
    const float RESPAWN_Y = START_Y;

    // ------------------------------------------------------
    // Load YOUR textures.
    // TODO: replace these paths with your actual asset files.
    // If a texture fails to load, Entity::render() automatically
    // falls back to drawing a plain rectangle, so the game still
    // runs (with placeholder boxes) even before your art is wired up.
    // ------------------------------------------------------
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, "../assets/my_player.png");
    if (!playerTexture) {
        SDL_Log("Could not load player texture: %s", SDL_GetError());
    } else {
        player.setTexture(playerTexture);
        player.setSpriteSheet(PLAYER_FRAME_COUNT, PLAYER_FRAME_W, PLAYER_FRAME_H);
        player.setAnimationSpeed(8.0f);
    }

    SDL_Texture* crateTexture = IMG_LoadTexture(renderer, "../assets/my_crate.png");
    if (!crateTexture) {
        SDL_Log("Could not load crate texture: %s", SDL_GetError());
    } else {
        crate.setTexture(crateTexture);
        crate.setSpriteSheet(CRATE_FRAME_COUNT, CRATE_FRAME_W, CRATE_FRAME_H);
    }

    SDL_Texture* platformTexture = IMG_LoadTexture(renderer, "../assets/my_platform.png");
    if (!platformTexture) {
        SDL_Log("Could not load platform texture: %s", SDL_GetError());
    } else {
        movingPlatform.setTexture(platformTexture);
        movingPlatform.setSpriteSheet(PLATFORM_FRAME_COUNT, PLATFORM_FRAME_W, PLATFORM_FRAME_H);
    }

    bool running = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks();

    while (running) {
        // ---- Task 1: handle window close ----
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // ------------------------------------------------------
        // Task 4: controls (all via Input::isKeyPressed, no raw
        // SDL key-down events other than the quit event above)
        //   A: move left
        //   D: move right
        //   W: jump (only works while grounded)
        // ------------------------------------------------------
        const float MOVE_SPEED = 350.0f;

        if (Input::isKeyPressed(SDL_SCANCODE_A)) {
            player.move(-MOVE_SPEED * deltaTime, 0.0f);
        }
        if (Input::isKeyPressed(SDL_SCANCODE_D)) {
            player.move(MOVE_SPEED * deltaTime, 0.0f);
        }
        if (Input::isKeyPressed(SDL_SCANCODE_W)) {
            physics.jump(player, 600.0f);
        }

        // Keep the player from walking off the edges of the window
        if (player.getX() < 0.0f) {
            player.setPosition(0.0f, player.getY());
        }
        if (player.getX() + player.getWidth() > actualWidth) {
            player.setPosition(actualWidth - player.getWidth(), player.getY());
        }
        // Toggle scaling mode with T (Task 6)
        bool scaleKeyIsPressed = Input::isKeyPressed(SDL_SCANCODE_T);
        if (scaleKeyIsPressed && !scaleKeyWasPressed) {
            Scaling::toggleMode();
            SDL_Log(
                "Scaling mode: %s",
                (Scaling::getMode() == ScalingMode::PROPORTIONAL) ? "PROPORTIONAL" : "PIXEL"
            );
        }
        scaleKeyWasPressed = scaleKeyIsPressed;

        // ---- Task 3: apply physics to the player only ----
        physics.update(player, deltaTime);

        // ---- Ground collision ----
        if (player.getY() + player.getHeight() >= GROUND_Y && player.getVelocityY() >= 0.0f) {
            player.setPosition(player.getX(), GROUND_Y - player.getHeight());
            player.setVelocityY(0.0f);
            player.setGrounded(true);
        } else {
            player.setGrounded(false);
        }

        // ------------------------------------------------------
        // Auto-moving object: platform patrols between top/bottom
        // ------------------------------------------------------
        float platformDeltaY = platformSpeed * platformDirection * deltaTime;
        movingPlatform.move(0.0f, platformDeltaY);
        if (movingPlatform.getY() <= PLATFORM_TOP_Y) {
            platformDirection = 1;
        }
        if (movingPlatform.getY() >= PLATFORM_BOTTOM_Y) {
            platformDirection = -1;
        }

        // ------------------------------------------------------
        // Task 5: collision responses
        // ------------------------------------------------------

        // 1) Static crate blocks the player like a wall
        if (Collision::checkCollision(player, crate)) {
            if (Input::isKeyPressed(SDL_SCANCODE_A)) {
                player.move(MOVE_SPEED * deltaTime, 0.0f);
            }
            if (Input::isKeyPressed(SDL_SCANCODE_D)) {
                player.move(-MOVE_SPEED * deltaTime, 0.0f);
            }
        }

        // 2) Moving platform: if the player is standing on top of it
        //    (falling onto it from above), carry them along with it
        //    and let them stand instead of falling through.
        bool playerAboveOrOnPlatform =
            (player.getY() + player.getHeight() <= movingPlatform.getY() + 10.0f) &&
            (player.getY() + player.getHeight() + platformDeltaY >= movingPlatform.getY());

        bool horizontallyOverPlatform =
            player.getX() + player.getWidth() > movingPlatform.getX() &&
            player.getX() < movingPlatform.getX() + movingPlatform.getWidth();

        if (horizontallyOverPlatform && playerAboveOrOnPlatform && player.getVelocityY() >= 0.0f) {
            player.setPosition(player.getX(), movingPlatform.getY() - player.getHeight());
            player.setVelocityY(0.0f);
            player.setGrounded(true);
            // Ride the platform: apply its vertical movement to the player too
            player.move(0.0f, platformDeltaY);
        }

        // Fail/reset: fall off the bottom of the world
        if (player.getY() > actualHeight) {
            SDL_Log("Fell off the level! Respawning.");
            player.setPosition(RESPAWN_X, RESPAWN_Y);
            player.setVelocity(0.0f, 0.0f);
        }

        // ---- Render ----
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255); // dark blue-purple background
        SDL_RenderClear(renderer);

        player.updateAnimation(deltaTime);
        crate.updateAnimation(deltaTime);
        movingPlatform.updateAnimation(deltaTime);

        crate.render(renderer);
        movingPlatform.render(renderer);
        player.render(renderer);

        SDL_RenderPresent(renderer);
    }

    if (playerTexture) SDL_DestroyTexture(playerTexture);
    if (crateTexture) SDL_DestroyTexture(crateTexture);
    if (platformTexture) SDL_DestroyTexture(platformTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}