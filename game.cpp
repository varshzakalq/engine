#include "game.h"

void Game::update()
{
    if (!initialized) {
        // Reserve space before creating instances to prevent vector reallocation
        scene.reserve(9);

        // Create multiple cubes at different positions
        float spacing = 5.0f;
        for (int x = -1; x <= 1; x++) {
            for (int z = 0; z <= 2; z++) {
                auto& cube = scene.create_instance(0);
                cube.translate(x * spacing, 0, z * spacing + 30);
                cubes.push_back(&cube);
            }
        }
        initialized = true;
    }

    float delta_time = timer.get_frame_time();

    // Rotate each cube at different rates and axes
    Vector3 axes[9] = {
        {1.0f, 0.0f, 0.0f}, // X-axis
        {0.0f, 1.0f, 0.0f}, // Y-axis
        {0.0f, 0.0f, 1.0f}, // Z-axis
        {1.0f, 1.0f, 0.0f}, // XY
        {1.0f, 0.0f, 1.0f}, // XZ
        {0.0f, 1.0f, 1.0f}, // YZ
        {1.0f, 1.0f, 1.0f}, // XYZ
        {1.0f, 0.5f, 0.0f}, // X+Y
        {0.5f, 1.0f, 0.0f}  // X+Y alt
    };

    for (size_t i = 0; i < cubes.size(); i++) {
        float rotation_speed = 0.5f + (i * 0.3f);
        float angle = rotation_speed * delta_time;
        cubes[i]->rotate(angle, axes[i]);
    }

    scene.update_transforms();
}











