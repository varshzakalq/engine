#include "engine.h"
#include "basic.h"
#include <utility>
using namespace std;
#include "basic.h"
#include "engine.h"
#include <cmath> // Required for std::abs

line::line(Engine &eng): e(eng){

}
void line::draw(pair<int,int>v1,pair<int,int>v2, Engine &engine) {
    // 1. Calculate absolute distances
    int x1 = v1.first;
    int y1 = v1.second;
    int x2 = v2.first;
    int y2 = v2.second;
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    
    // 2. Track step direction (-1 if moving backward, 1 if moving forward)
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    
    int x = x1;
    int y = y1;
    uint32_t green = 0x00FF00FF; // Your line color

    // 3. Shallow lines (horizontal movement is dominant)
    if (dx > dy) {
        int p = 2 * dy - dx;
        while (x != x2) {
            engine.put_pixel(x, y, green);
            x += sx; // Moves forward or backward automatically!
            
            if (p >= 0) {
                y += sy; // Moves up or down automatically!
                p += 2 * (dy - dx);
            } else {
                p += 2 * dy;
            }
        }
    } 
    // 4. Steep lines (vertical movement is dominant)
    else {
        int p = 2 * dx - dy;
        while (y != y2) {
            engine.put_pixel(x, y, green);
            y += sy; // Moves up or down automatically!
            
            if (p >= 0) {
                x += sx; // Moves forward or backward automatically!
                p += 2 * (dx - dy);
            } else {
                p += 2 * dx;
            }
        }
    }
    
    // 5. Always catch the very last pixel
    engine.put_pixel(x2, y2, green);
}

pair <int,int> project::onscreen(double x1, double y1, double z1, float focal_lenght){
    
    float x = (float)focal_lenght/z1*x1*size;
    float y =  (float)focal_lenght/z1*y1*size;
    x = (int) x;
    y = (int) y;
    return {static_cast<int>(x),static_cast<int>(y)};
}

void project::fill_color(pair<int,int> v1, pair<int,int> v2, pair<int,int> v3, uint32_t color, int width, int height, Engine &engine) {
    // 1. Calculate bounding box and safely clamp to screen boundaries
    int minX = min({v1.first, v2.first, v3.first});
    int minY = min({v1.second, v2.second, v3.second});
    int maxX = min(width - 1, max({v1.first, v2.first, v3.first}));
    int maxY = min(height - 1, max({v1.second, v2.second, v3.second}));

    // Convert vertices to float once to avoid repetitive casting in the loop
    float x1 = v1.first, y1 = v1.second;
    float x2 = v2.first, y2 = v2.second;
    float x3 = v3.first, y3 = v3.second;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // Sample at pixel center
            float px = x + 0.5f;
            float py = y + 0.5f;

            // 2. Edge functions (2D cross products)
            float e0 = (x2 - x1) * (py - y1) - (px - x1) * (y2 - y1);
            float e1 = (x3 - x2) * (py - y2) - (px - x2) * (y3 - y2);
            float e2 = (x1 - x3) * (py - y3) - (px - x3) * (y1 - y3);

            // 3. Check if pixel is inside, supporting both Counter-Clockwise and Clockwise orders
            
            bool is_cw  = (e0 <= 0 && e1 <= 0 && e2 <= 0);

            if (is_cw) {
                engine.put_pixel(x, y, color);
            }
            
        }
    }
}