#include "Collision.h"
#include <SDL3/SDL.h>

bool Collision::checkCollision(const Entity& entityA, const Entity& entityB) {
    // Creates rectangular bounds for each entity
    SDL_FRect rectA = { entityA.getX(), entityA.getY(), entityA.getWidth(), entityA.getHeight() };
    SDL_FRect rectB = { entityB.getX(), entityB.getY(), entityB.getWidth(), entityB.getHeight() };
    
    return SDL_HasRectIntersectionFloat(&rectA, &rectB);
}