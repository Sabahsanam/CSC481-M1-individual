#include "Entity.h"

Entity::Entity(float x, float y, float width, float height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    texture = nullptr;

    frameCount = 1;
    frameWidth = 0;
    frameHeight = 0;
    currentFrame = 0;
    animationCounter = 0;
}

void Entity::render(SDL_Renderer* renderer)
{
    SDL_FRect destinationRect = {
        x,
        y,
        width,
        height
    };

    if (texture != nullptr) {

        // Animated sprite sheet
        if (frameCount > 1 && frameWidth > 0 && frameHeight > 0) {

            SDL_FRect sourceRect = {
                (float)(currentFrame * frameWidth),
                0.0f,
                (float)frameWidth,
                (float)frameHeight
            };

            SDL_RenderTexture(
                renderer,
                texture,
                &sourceRect,
                &destinationRect
            );
        }

        // Normal static texture
        else {
            SDL_RenderTexture(
                renderer,
                texture,
                nullptr,
                &destinationRect
            );
        }
    }

    // No texture: draw a white rectangle
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &destinationRect);
    }
}

void Entity::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}

void Entity::move(float dx, float dy)
{
    x += dx;
    y += dy;
}

void Entity::setTexture(SDL_Texture* texture)
{
    this->texture = texture;
}

void Entity::setSpriteSheet(int frameCount, int frameWidth, int frameHeight)
{
    this->frameCount = frameCount;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
}

void Entity::updateAnimation()
{
    // Static images do not need animation
    if (frameCount <= 1) {
        return;
    }

    animationCounter++;

    if (animationCounter >= 100) {
        currentFrame = (currentFrame + 1) % frameCount;
        animationCounter = 0;
    }
}

float Entity::getX() const
{
    return x;
}

float Entity::getY() const
{
    return y;
}

float Entity::getWidth() const
{
    return width;
}

float Entity::getHeight() const
{
    return height;
}