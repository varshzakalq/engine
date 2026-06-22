#include "game.h"

void Game::update()
{
    if (!initialized) {
        // Reserve space for just 1 instance
        scene.reserve(1);
        
        // Lights
        point_light p1 = {50,{10,20,50},{1,0,1}};
        lights.push_back(p1);
        point_light p2 = {150,{1,50,50},{1,0,1}};
        lights.push_back(p2);
        
        // Create only a single cube instance
        auto& cube = scene.create_instance(0);
        
        // Position it centered horizontally (X=0, Y=0) and 30 units forward (Z=30)
        cube.translate(0.0f, 0.0f, 30.0f);
        cubes.push_back(&cube);
        
        initialized = true;
    }

    float delta_time = timer.get_frame_time();

    // Rotate your single cube
    // Adjust rotation_speed or the rotation axis vector here as needed
    if (!cubes.empty()) {
        float rotation_speed = 1.0f; 
        float angle = rotation_speed * delta_time;
        
        // Rotating around a clean diagonal axis (X and Y) to test all sides
        Vector3 rotation_axis = {1.0f, 1.0f, 0.0f}; 
        cubes[0]->rotate(angle, rotation_axis);
    }

    scene.update_transforms();
}




