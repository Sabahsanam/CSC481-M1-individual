#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <vector>
#include <SDL3/SDL.h>
#include "Entity.h"

// A patrolling enemy needs a bit more state than a plain Entity.
struct PatrolEnemy {
    Entity entity;
    float minX;
    float maxX;
    float speed;
    int direction; // 1 = right, -1 = left
};

// A coin needs a "collected" flag.
struct Coin {
    Entity entity;
    bool collected;
};

// One ground segment: a solid strip of floor from startX to endX.
// Gaps between segments are pits.
struct GroundSegment {
    float startX;
    float endX;
};

// Holds every texture WorldGenerator needs to hand to the entities it creates.
struct WorldTextures {
    SDL_Texture* brick;
    SDL_Texture* crate;
    SDL_Texture* platform;
    SDL_Texture* coin;
    SDL_Texture* bush;
    SDL_Texture* totem;
    SDL_Texture* orb;
    SDL_Texture* skull;
    SDL_Texture* nyx;
};

class WorldGenerator {
public:
    WorldGenerator(float groundY, WorldTextures textures);

    // Call once per frame. Generates new chunks as the camera approaches
    // the end of what's already built, and removes entities that have
    // scrolled far behind the camera.
    void update(float cameraX, float screenWidth, float deltaTime);

    // Clears everything and generates a safe starting area.
    void reset();

    //  Accessors for main.cpp to use for collision/rendering 
    std::vector<GroundSegment>& getGroundSegments();
    std::vector<Entity>& getPlatforms();
    std::vector<Entity>& getObstacles();
    std::vector<PatrolEnemy>& getEnemies();
    std::vector<Entity>& getHazards();
    std::vector<Coin>& getCoins();
    std::vector<Entity>& getBushes();

private:
    float groundY;
    WorldTextures textures;
    float nextChunkX;

    std::vector<GroundSegment> groundSegments;
    std::vector<Entity> platforms;
    std::vector<Entity> obstacles;
    std::vector<PatrolEnemy> enemies;
    std::vector<Entity> hazards;
    std::vector<Coin> coins;
    std::vector<Entity> bushes;

    static const float CHUNK_WIDTH;

    void generateChunk(float chunkStartX);
    void cleanupBehind(float cameraX);
};

#endif
