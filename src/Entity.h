#ifndef ENTITY_H
#define ENTITY_H

#include <SDL3/SDL.h>

class Entity {
public:
    Entity(float x, float y, float width, float height);

    void render(SDL_Renderer* renderer);

    void setPosition(float x, float y);
    void move(float dx, float dy);

    void setTexture(SDL_Texture* texture);

    void setSpriteSheet(int frameCount, int frameWidth, int frameHeight);
    void updateAnimation();

    // Getters
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

private:
    float x;
    float y;
    float width;
    float height;

    SDL_Texture* texture;

    int frameCount;
    int frameWidth;
    int frameHeight;
    int currentFrame;
    int animationCounter;
};

#endif