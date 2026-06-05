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
    int size = 100;
    float x = (float)focal_lenght/z1*x1*size;
    float y =  (float)focal_lenght/z1*y1*size;
    x = (int) x;
    y = (int) y;
    return {static_cast<int>(x),static_cast<int>(y)};
}
