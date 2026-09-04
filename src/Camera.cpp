#include "Camera.h"

Camera::Camera(float screenWidth)
{
    this->screenWidth = screenWidth;
    x = 0.0f;
}

void Camera::update(float playerWorldX, float playerWidth)
{
    // Keep the player at 40% of the screen width once the camera
    // needs to start following her.
    float targetCameraX = playerWorldX + playerWidth / 2.0f - screenWidth * 0.4f;

    // Never scroll left past the start of the world.
    if (targetCameraX < 0.0f) {
        targetCameraX = 0.0f;
    }

    x = targetCameraX;
}

float Camera::getX() const
{
    return x;
}

void Camera::reset()
{
    x = 0.0f;
}
