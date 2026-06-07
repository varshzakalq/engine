#ifndef ENGINE_H
#define ENGINE_H
#include "objloader.h"
#include <stdint.h>
#include "camera.h"
#include "basic.h"
#include "lighting.h"
#include "texture.h"
// Forward declaration instead of #include "line.h"
// This tells the compiler "a class named 'line' exists" without needing its file yet.

class Engine {
private:
    uint32_t* pixels;
    int width;
    int height;
    double curr_angle;
    line myLine;
    camera cam1;
    project projection;
    OBJ obj;
    uint32_t color;
    clipper clip;
    lighting light1;
    float deapth = 100000.0f;
    std::vector<float> z_buffer;
    std::vector<vertex_data>transformed_vertices;
    Texture crate_texture; //this will make sure it only created once
public:
    // Constructor / Destructor
    Engine();
    ~Engine();

    // Initializes the engine with the array from main.cpp
    void init(uint32_t* pixel_array, int w, int h);

    // Clears the entire screen with a solid color
    void clear(uint32_t color);

    // Your core PutPixel function
    void put_pixel(int x, int y, uint32_t color);

    // A function where you will write your rendering logic (lines, shapes, etc.)
    void update_and_render();

    // Getters so other classes (like line) can safely look at your screen bounds/pixels
    uint32_t* get_pixels() const { return pixels; }
    int get_width() const { return width; }
    int get_height() const { return height; }
};

#endif // ENGINE_H