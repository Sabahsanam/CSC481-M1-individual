#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "Entity.h"
#include "Physics.h"
#include "Input.h"
#include "Collision.h"
#include "Scaling.h"
#include "Camera.h"
#include "WorldGenerator.h"

// ============================================================
// SABAH'S INDIVIDUAL GAME
// Endless cute-princess side-scrolling platformer.
// Engine (Entity/Physics/Input/Collision/Scaling) untouched.
// Game-specific: Camera, WorldGenerator, and everything below.
// ============================================================

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const int PLAYER_FRAME_COUNT = 8;
const int PLAYER_FRAME_W = 96;
const int PLAYER_FRAME_H = 156;

const float BRICK_TILE_WIDTH = 96.0f;
const float BRICK_TILE_HEIGHT = 32.0f;

enum class GameState {
    PLAYING,
    GAME_OVER
};

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Could not initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer(
            "Sabah's Game - Endless Princess Platformer",
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE,
            &window, &renderer)) {
        SDL_Log("Could not create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int actualWidth = 0, actualHeight = 0;
    SDL_GetWindowSize(window, &actualWidth, &actualHeight);
    Scaling::setReferenceResolution(actualWidth, actualHeight);
    bool scaleKeyWasPressed = false;

    const float GROUND_Y = actualHeight - 150.0f;

    // ---- Load textures ----
    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "../assets/background.png");
    SDL_Texture* cloudTexture = IMG_LoadTexture(renderer, "../assets/cloud.png");
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, "../assets/my_player.png");
    SDL_Texture* brickTexture = IMG_LoadTexture(renderer, "../assets/brick.png");
    SDL_Texture* crateTexture = IMG_LoadTexture(renderer, "../assets/my_crate.png");
    SDL_Texture* platformTexture = IMG_LoadTexture(renderer, "../assets/my_platform.png");
    SDL_Texture* coinTexture = IMG_LoadTexture(renderer, "../assets/coin.png");
    SDL_Texture* bushTexture = IMG_LoadTexture(renderer, "../assets/bush.png");
    SDL_Texture* totemTexture = IMG_LoadTexture(renderer, "../assets/totem.png");
    SDL_Texture* orbTexture = IMG_LoadTexture(renderer, "../assets/swirlingorb.png");
    SDL_Texture* skullTexture = IMG_LoadTexture(renderer, "../assets/enemy_skull.png");
    SDL_Texture* nyxTexture = IMG_LoadTexture(renderer, "../assets/darkworld_enemy_nyx_idle.png");

    if (!backgroundTexture) SDL_Log("Missing background.png: %s", SDL_GetError());
    if (!playerTexture) SDL_Log("Missing my_player.png: %s", SDL_GetError());

    WorldTextures worldTextures;
    worldTextures.brick = brickTexture;
    worldTextures.crate = crateTexture;
    worldTextures.platform = platformTexture;
    worldTextures.coin = coinTexture;
    worldTextures.bush = bushTexture;
    worldTextures.totem = totemTexture;
    worldTextures.orb = orbTexture;
    worldTextures.skull = skullTexture;
    worldTextures.nyx = nyxTexture;

    // ---- Player ----
    Entity player(100.0f, 100.0f, 80.0f, 130.0f);
    player.setGravityEnabled(true);
    if (playerTexture) {
        player.setTexture(playerTexture);
        player.setSpriteSheet(PLAYER_FRAME_COUNT, PLAYER_FRAME_W, PLAYER_FRAME_H);
        player.setAnimationSpeed(8.0f);
    }

    Physics physics;
    physics.setGravity(900.0f);

    Camera camera((float)actualWidth);
    WorldGenerator world(GROUND_Y, worldTextures);

    // ---- Decorative clouds (independent of world/camera, screen-space loop) ----
    float cloudX[3] = { 200.0f, 900.0f, 1500.0f };
    float cloudY[3] = { 100.0f, 220.0f, 60.0f };
    float cloudSpeed[3] = { 40.0f, 25.0f, 55.0f };
    float cloudScale[3] = { 1.0f, 0.7f, 1.3f };

    // ---- Game state ----
    GameState state = GameState::PLAYING;
    int collectibles = 0;
    float furthestX = 0.0f;

    auto triggerGameOver = [&]() {
        if (state == GameState::GAME_OVER) return;
        state = GameState::GAME_OVER;
        SDL_Log("GAME OVER");
        SDL_Log("Distance: %.0f", furthestX);
        SDL_Log("Stars collected: %d", collectibles);
        SDL_Log("Press R to restart, Q to quit.");
    };

    auto resetGame = [&]() {
        player.setPosition(100.0f, 100.0f);
        player.setVelocity(0.0f, 0.0f);
        player.setGrounded(false);
        camera.reset();
        world.reset();
        collectibles = 0;
        furthestX = 0.0f;
        state = GameState::PLAYING;
        SDL_Log("New run started.");
    };

    resetGame();

    bool running = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks();
    bool rKeyWasPressed = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (Input::isKeyPressed(SDL_SCANCODE_ESCAPE)) {
            running = false;
        }

        // Clouds always animate, regardless of game state
        for (int i = 0; i < 3; i++) {
            cloudX[i] += cloudSpeed[i] * deltaTime;
            if (cloudX[i] > actualWidth + 220.0f) cloudX[i] = -220.0f;
        }

        if (state == GameState::PLAYING) {
            // ---- Controls ----
            const float WALK_SPEED = 300.0f;
            const float RUN_SPEED = 550.0f;
            float moveSpeed = WALK_SPEED;

            if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) || Input::isKeyPressed(SDL_SCANCODE_RSHIFT)) {
                moveSpeed = RUN_SPEED;
            }
            if (Input::isKeyPressed(SDL_SCANCODE_A)) {
                player.move(-moveSpeed * deltaTime, 0.0f);
            }
            if (Input::isKeyPressed(SDL_SCANCODE_D)) {
                player.move(moveSpeed * deltaTime, 0.0f);
            }
            if (Input::isKeyPressed(SDL_SCANCODE_W)) {
                physics.jump(player, 650.0f);
            }

            // Never scroll/walk left past world start
            if (player.getX() < 0.0f) {
                player.setPosition(0.0f, player.getY());
            }

            bool scaleKeyIsPressed = Input::isKeyPressed(SDL_SCANCODE_T);
            if (scaleKeyIsPressed && !scaleKeyWasPressed) {
                Scaling::toggleMode();
                SDL_Log("Scaling mode: %s",
                    (Scaling::getMode() == ScalingMode::PROPORTIONAL) ? "PROPORTIONAL" : "PIXEL");
            }
            scaleKeyWasPressed = scaleKeyIsPressed;

            // ---- Physics ----
            physics.update(player, deltaTime);

            // ---- Ground collision: find if she's over a solid segment ----
            float centerX = player.getX() + player.getWidth() / 2.0f;
            bool onSolidGround = false;
            for (auto& seg : world.getGroundSegments()) {
                if (centerX >= seg.startX && centerX <= seg.endX) {
                    onSolidGround = true;
                    break;
                }
            }

            if (onSolidGround && player.getY() + player.getHeight() >= GROUND_Y && player.getVelocityY() >= 0.0f) {
                player.setPosition(player.getX(), GROUND_Y - player.getHeight());
                player.setVelocityY(0.0f);
                player.setGrounded(true);
            } else {
                player.setGrounded(false);
            }

            // ---- Platform collision (land on top only) ----
            for (auto& plat : world.getPlatforms()) {
                bool horizontallyOver =
                    player.getX() + player.getWidth() > plat.getX() &&
                    player.getX() < plat.getX() + plat.getWidth();
                bool fallingOnto =
                    player.getY() + player.getHeight() <= plat.getY() + 12.0f &&
                    player.getY() + player.getHeight() + player.getVelocityY() * deltaTime >= plat.getY();

                if (horizontallyOver && fallingOnto && player.getVelocityY() >= 0.0f) {
                    player.setPosition(player.getX(), plat.getY() - player.getHeight());
                    player.setVelocityY(0.0f);
                    player.setGrounded(true);
                }
            }

            // ---- Obstacles block movement like a wall ----
            for (auto& obs : world.getObstacles()) {
                if (Collision::checkCollision(player, obs)) {
                    if (Input::isKeyPressed(SDL_SCANCODE_A)) {
                        player.move(moveSpeed * deltaTime, 0.0f);
                    }
                    if (Input::isKeyPressed(SDL_SCANCODE_D)) {
                        player.move(-moveSpeed * deltaTime, 0.0f);
                    }
                }
            }

            // ---- Coins ----
            for (auto& coin : world.getCoins()) {
                if (!coin.collected && Collision::checkCollision(player, coin.entity)) {
                    coin.collected = true;
                    collectibles++;
                    SDL_Log("Star collected! Total: %d", collectibles);
                }
            }

            // ---- Enemies: touching = game over ----
            for (auto& enemy : world.getEnemies()) {
                if (Collision::checkCollision(player, enemy.entity)) {
                    triggerGameOver();
                }
            }

            // ---- Hazards: touching = game over ----
            for (auto& hazard : world.getHazards()) {
                if (Collision::checkCollision(player, hazard)) {
                    triggerGameOver();
                }
            }

            // ---- Fall into a pit = game over (no auto-teleport) ----
            if (player.getY() > actualHeight) {
                triggerGameOver();
            }

            if (player.getX() > furthestX) {
                furthestX = player.getX();
            }

            camera.update(player.getX(), player.getWidth());
            world.update(camera.getX(), (float)actualWidth, deltaTime);
            player.updateAnimation(deltaTime);
        }
        else { // GAME_OVER
            bool rIsPressed = Input::isKeyPressed(SDL_SCANCODE_R);
            if (rIsPressed && !rKeyWasPressed) {
                resetGame();
            }
            rKeyWasPressed = rIsPressed;

            if (Input::isKeyPressed(SDL_SCANCODE_Q)) {
                running = false;
            }
        }

        float cameraX = camera.getX();

        // ---- Render ----
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_FRect bgRect = { 0, 0, (float)actualWidth, (float)actualHeight };
            SDL_RenderTexture(renderer, backgroundTexture, nullptr, &bgRect);
        }

        if (cloudTexture) {
            for (int i = 0; i < 3; i++) {
                float cw = 220.0f * cloudScale[i];
                float ch = 110.0f * cloudScale[i];
                SDL_FRect cloudRect = { cloudX[i], cloudY[i], cw, ch };
                SDL_RenderTexture(renderer, cloudTexture, nullptr, &cloudRect);
            }
        }

        // Ground segments (tiled brick), shifted by camera
        if (brickTexture) {
            for (auto& seg : world.getGroundSegments()) {
                for (float x = seg.startX; x < seg.endX; x += BRICK_TILE_WIDTH) {
                    float screenX = x - cameraX;
                    if (screenX < -BRICK_TILE_WIDTH || screenX > actualWidth) continue;
                    SDL_FRect r = { screenX, GROUND_Y, BRICK_TILE_WIDTH, BRICK_TILE_HEIGHT };
                    SDL_RenderTexture(renderer, brickTexture, nullptr, &r);
                }
            }
        }

        // Helper-style inline rendering: shift each entity by camera, draw, shift back
        auto drawShifted = [&](Entity& e) {
            float realX = e.getX();
            e.setPosition(realX - cameraX, e.getY());
            e.render(renderer);
            e.setPosition(realX, e.getY());
        };

        for (auto& bush : world.getBushes()) drawShifted(bush);
        for (auto& obs : world.getObstacles()) drawShifted(obs);
        for (auto& plat : world.getPlatforms()) drawShifted(plat);
        for (auto& coin : world.getCoins()) {
            if (!coin.collected) {
                coin.entity.updateAnimation(deltaTime);
                drawShifted(coin.entity);
            }
        }
        for (auto& hazard : world.getHazards()) drawShifted(hazard);
        for (auto& enemy : world.getEnemies()) drawShifted(enemy.entity);

        drawShifted(player);

        // Simple game-over dim overlay (no SDL_ttf needed, per design decision)
        if (state == GameState::GAME_OVER) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
            SDL_FRect overlay = { 0, 0, (float)actualWidth, (float)actualHeight };
            SDL_RenderFillRect(renderer, &overlay);
        }

        SDL_RenderPresent(renderer);
    }

    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (cloudTexture) SDL_DestroyTexture(cloudTexture);
    if (playerTexture) SDL_DestroyTexture(playerTexture);
    if (brickTexture) SDL_DestroyTexture(brickTexture);
    if (crateTexture) SDL_DestroyTexture(crateTexture);
    if (platformTexture) SDL_DestroyTexture(platformTexture);
    if (coinTexture) SDL_DestroyTexture(coinTexture);
    if (bushTexture) SDL_DestroyTexture(bushTexture);
    if (totemTexture) SDL_DestroyTexture(totemTexture);
    if (orbTexture) SDL_DestroyTexture(orbTexture);
    if (skullTexture) SDL_DestroyTexture(skullTexture);
    if (nyxTexture) SDL_DestroyTexture(nyxTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}