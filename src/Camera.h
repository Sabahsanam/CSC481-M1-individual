#ifndef CAMERA_H
#define CAMERA_H

// Simple side-scrolling camera. Tracks a single horizontal offset (x).
// Entities keep their true WORLD position always; only at render time
// do we compute screenX = worldX - camera.getX().
class Camera {
public:
    Camera(float screenWidth);

    // Call once per frame with the player's world X and width.
    // Keeps the player at ~40% of the screen width once she has
    // moved far enough right; never scrolls left past world 0.
    void update(float playerWorldX, float playerWidth);

    float getX() const;

    void reset();

private:
    float x;
    float screenWidth;
};

#endif
