#ifndef CAMERA_H
#define CAMERA_H

// Simple side-scrolling camera. Tracks a single horizontal offset (x).
class Camera {
public:
    Camera(float screenWidth);

    // Call once per frame with the player's world X and width.
    // Keeps the player at ~40% of the screen width once she has moved far enough right
    void update(float playerWorldX, float playerWidth);

    float getX() const;

    void reset();

private:
    float x;
    float screenWidth;
};

#endif
