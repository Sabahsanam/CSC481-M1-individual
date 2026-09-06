#include "WorldGenerator.h"
#include <cstdlib>
#include <algorithm>

const float WorldGenerator::CHUNK_WIDTH = 900.0f;

// Asset frame info 
static const int CRATE_FRAMES = 1, CRATE_W = 64, CRATE_H = 64;
static const int PLATFORM_FRAMES = 1, PLATFORM_W = 192, PLATFORM_H = 22;
static const int COIN_FRAMES = 1, COIN_W = 64, COIN_H = 64;
static const int BUSH_FRAMES = 1, BUSH_W = 120, BUSH_H = 90;
static const int TOTEM_FRAMES = 8, TOTEM_W = 64, TOTEM_H = 192;
static const int ORB_FRAMES = 4, ORB_W = 128, ORB_H = 128;
static const int SKULL_FRAMES = 6, SKULL_W = 32, SKULL_H = 32;
static const int NYX_FRAMES = 8, NYX_W = 128, NYX_H = 128;

WorldGenerator::WorldGenerator(float groundY, WorldTextures textures)
{
    this->groundY = groundY;
    this->textures = textures;
    nextChunkX = 0.0f;
}

void WorldGenerator::reset()
{
    groundSegments.clear();
    platforms.clear();
    obstacles.clear();
    enemies.clear();
    hazards.clear();
    coins.clear();
    bushes.clear();

    // Safe starting area: solid ground, nothing dangerous near spawn.
    groundSegments.push_back({ 0.0f, 900.0f });
    nextChunkX = 900.0f;

    // Pre-fill a few chunks ahead so the world isn't empty at launch.
    for (int i = 0; i < 3; i++) {
        generateChunk(nextChunkX);
        nextChunkX += CHUNK_WIDTH;
    }
}

void WorldGenerator::generateChunk(float startX)
{
    int pattern = rand() % 6;

    if (pattern == 0) {
        // PATTERN A: ground, ground, small pit, ground
        groundSegments.push_back({ startX, startX + 350.0f });
        groundSegments.push_back({ startX + 500.0f, startX + CHUNK_WIDTH });
    }
    else if (pattern == 1) {
        // PATTERN B: ground, crate, raised platform, ground
        groundSegments.push_back({ startX, startX + CHUNK_WIDTH });

        Entity crate(startX + 300.0f, groundY - CRATE_H, (float)CRATE_W, (float)CRATE_H);
        crate.setGravityEnabled(false);
        if (textures.crate) {
            crate.setTexture(textures.crate);
            crate.setSpriteSheet(CRATE_FRAMES, CRATE_W, CRATE_H);
        }
        obstacles.push_back(crate);

        Entity plat(startX + 550.0f, groundY - 180.0f, (float)PLATFORM_W, (float)PLATFORM_H);
        plat.setGravityEnabled(false);
        if (textures.platform) {
            plat.setTexture(textures.platform);
            plat.setSpriteSheet(PLATFORM_FRAMES, PLATFORM_W, PLATFORM_H);
        }
        platforms.push_back(plat);
    }
    else if (pattern == 2) {
        // PATTERN C: ground, coin, enemy (skull), ground
        groundSegments.push_back({ startX, startX + CHUNK_WIDTH });

        Entity coinEntity(startX + 300.0f, groundY - 220.0f, (float)COIN_W, (float)COIN_H);
        coinEntity.setGravityEnabled(false);
        if (textures.coin) {
            coinEntity.setTexture(textures.coin);
            coinEntity.setSpriteSheet(COIN_FRAMES, COIN_W, COIN_H);
        }
        coins.push_back({ coinEntity, false });

        Entity skull(startX + 550.0f, groundY - 150.0f, (float)SKULL_W * 2, (float)SKULL_H * 2);
        skull.setGravityEnabled(false);
        if (textures.skull) {
            skull.setTexture(textures.skull);
            skull.setSpriteSheet(SKULL_FRAMES, SKULL_W, SKULL_H);
            skull.setAnimationSpeed(8.0f);
        }
        enemies.push_back({ skull, startX + 500.0f, startX + 750.0f, 120.0f, 1 });
    }
    else if (pattern == 3) {
        // PATTERN D: ground, pit, platform (bridges the pit), coin, ground
        groundSegments.push_back({ startX, startX + 250.0f });
        groundSegments.push_back({ startX + 500.0f, startX + CHUNK_WIDTH });

        Entity plat(startX + 300.0f, groundY - 120.0f, (float)PLATFORM_W, (float)PLATFORM_H);
        plat.setGravityEnabled(false);
        if (textures.platform) {
            plat.setTexture(textures.platform);
            plat.setSpriteSheet(PLATFORM_FRAMES, PLATFORM_W, PLATFORM_H);
        }
        platforms.push_back(plat);

        Entity coinEntity(startX + 370.0f, groundY - 200.0f, (float)COIN_W, (float)COIN_H);
        coinEntity.setGravityEnabled(false);
        if (textures.coin) {
            coinEntity.setTexture(textures.coin);
            coinEntity.setSpriteSheet(COIN_FRAMES, COIN_W, COIN_H);
        }
        coins.push_back({ coinEntity, false });
    }
    else if (pattern == 4) {
        // PATTERN E: ground, enemy (nyx), crate, ground
        groundSegments.push_back({ startX, startX + CHUNK_WIDTH });

        Entity nyx(startX + 250.0f, groundY - NYX_H, (float)NYX_W * 0.6f, (float)NYX_H * 0.6f);
        nyx.setGravityEnabled(false);
        if (textures.nyx) {
            nyx.setTexture(textures.nyx);
            nyx.setSpriteSheet(NYX_FRAMES, NYX_W, NYX_H);
            nyx.setAnimationSpeed(8.0f);
        }
        enemies.push_back({ nyx, startX + 200.0f, startX + 450.0f, 100.0f, 1 });

        Entity crate(startX + 650.0f, groundY - CRATE_H, (float)CRATE_W, (float)CRATE_H);
        crate.setGravityEnabled(false);
        if (textures.crate) {
            crate.setTexture(textures.crate);
            crate.setSpriteSheet(CRATE_FRAMES, CRATE_W, CRATE_H);
        }
        obstacles.push_back(crate);
    }
    else {
        // PATTERN F: ground, magical orb hazard, pit, ground
        groundSegments.push_back({ startX, startX + 400.0f });
        groundSegments.push_back({ startX + 600.0f, startX + CHUNK_WIDTH });

        Entity orb(startX + 280.0f, groundY - ORB_H * 0.6f, (float)ORB_W * 0.6f, (float)ORB_H * 0.6f);
        orb.setGravityEnabled(false);
        if (textures.orb) {
            orb.setTexture(textures.orb);
            orb.setSpriteSheet(ORB_FRAMES, ORB_W, ORB_H);
            orb.setAnimationSpeed(6.0f);
        }
        hazards.push_back(orb);
    }

    // Occasional decorative bush on this chunk (no collision)
    if (rand() % 2 == 0) {
        Entity bush(startX + 100.0f + (rand() % 600), groundY - BUSH_H, (float)BUSH_W, (float)BUSH_H);
        bush.setGravityEnabled(false);
        if (textures.bush) {
            bush.setTexture(textures.bush);
            bush.setSpriteSheet(BUSH_FRAMES, BUSH_W, BUSH_H);
        }
        bushes.push_back(bush);
    }
}

void WorldGenerator::update(float cameraX, float screenWidth, float deltaTime)
{
    // Generate more world as the camera approaches the edge of what exists
    const float GENERATION_BUFFER = 600.0f;
    if (cameraX + screenWidth + GENERATION_BUFFER > nextChunkX) {
        generateChunk(nextChunkX);
        nextChunkX += CHUNK_WIDTH;
    }

    // Move patrolling enemies
    for (auto& e : enemies) {
        e.entity.move(e.speed * e.direction * deltaTime, 0.0f);
        if (e.entity.getX() >= e.maxX) e.direction = -1;
        if (e.entity.getX() <= e.minX) e.direction = 1;
        e.entity.updateAnimation(deltaTime);
    }

    for (auto& h : hazards) {
        h.updateAnimation(deltaTime);
    }

    cleanupBehind(cameraX);
}

void WorldGenerator::cleanupBehind(float cameraX)
{
    const float CLEANUP_MARGIN = 500.0f;
    float cutoff = cameraX - CLEANUP_MARGIN;

    groundSegments.erase(
        std::remove_if(groundSegments.begin(), groundSegments.end(),
            [cutoff](const GroundSegment& g) { return g.endX < cutoff; }),
        groundSegments.end());

    platforms.erase(
        std::remove_if(platforms.begin(), platforms.end(),
            [cutoff](Entity& e) { return e.getX() + e.getWidth() < cutoff; }),
        platforms.end());

    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [cutoff](Entity& e) { return e.getX() + e.getWidth() < cutoff; }),
        obstacles.end());

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [cutoff](PatrolEnemy& e) { return e.maxX < cutoff; }),
        enemies.end());

    hazards.erase(
        std::remove_if(hazards.begin(), hazards.end(),
            [cutoff](Entity& e) { return e.getX() + e.getWidth() < cutoff; }),
        hazards.end());

    coins.erase(
        std::remove_if(coins.begin(), coins.end(),
            [cutoff](Coin& c) { return c.entity.getX() + c.entity.getWidth() < cutoff; }),
        coins.end());

    bushes.erase(
        std::remove_if(bushes.begin(), bushes.end(),
            [cutoff](Entity& e) { return e.getX() + e.getWidth() < cutoff; }),
        bushes.end());
}

std::vector<GroundSegment>& WorldGenerator::getGroundSegments() { return groundSegments; }
std::vector<Entity>& WorldGenerator::getPlatforms() { return platforms; }
std::vector<Entity>& WorldGenerator::getObstacles() { return obstacles; }
std::vector<PatrolEnemy>& WorldGenerator::getEnemies() { return enemies; }
std::vector<Entity>& WorldGenerator::getHazards() { return hazards; }
std::vector<Coin>& WorldGenerator::getCoins() { return coins; }
std::vector<Entity>& WorldGenerator::getBushes() { return bushes; }
