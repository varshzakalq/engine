#include "engine.h"
#include "basic.h"
#include <utility>
using namespace std;
#include "basic.h"
#include "engine.h"
#include <cmath> // Required for std::abs
#include"texture.h"

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

array<float, 3> getweight_z(pair<int, int> v1, pair<int, int> v2, pair<int, int> v3, float px, float py) {
    array<float, 3> weights;
    
    float denom = static_cast<float>((v2.second - v3.second) * (v1.first - v3.first) + (v3.first - v2.first) * (v1.second - v3.second));
    
    // Avoid division by zero on degenerate triangles
    if (std::abs(denom) < 1e-6f) {
        return {0.0f, 0.0f, 0.0f};
    }

    weights[0] = ((v2.second - v3.second) * (px - v3.first) + (v3.first - v2.first) * (py - v3.second)) / denom;
    weights[1] = ((v3.second - v1.second) * (px - v3.first) + (v1.first - v3.first) * (py - v3.second)) / denom;
    weights[2] = 1.0f - weights[0] - weights[1];
    return weights;
}

pair<int, int> project::onscreen(double x1, double y1, double z1, float focal_length) {
    // Avoid division by zero if geometry passes behind camera
    if (std::abs(z1) < 1e-4) z1 = 1e-4; 

    float x = static_cast<float>(focal_length / z1 * x1 * size);
    float y = static_cast<float>(focal_length / z1 * y1 * size);
    
    return { static_cast<int>(std::round(x)), static_cast<int>(std::round(y)) };
}

void project::fill_color(
     array<float, 3> z_values,
     array<array<float,2>,3> uv,
     Texture &texture,
     float light_intensity,
     pair<int, int> v1, 
     pair<int, int> v2, 
     pair<int, int> v3, 
     vector<float>& z_buffer, 
     uint32_t &color, 
     int width, 
     int height, 
     Engine &engine
    ) {
    
    // 1. Compute and safely clamp bounding box to screen coordinates (accounting for center shifting)
    int minX = max(-width / 2,     min({v1.first, v2.first, v3.first}));
    int minY = max(-height / 2,    min({v1.second, v2.second, v3.second}));
    int maxX = min(width / 2 - 1,  max({v1.first, v2.first, v3.first}));
    int maxY = min(height / 2 - 1, max({v1.second, v2.second, v3.second}));

    float x1 = v1.first,  y1 = v1.second;
    float x2 = v2.first,  y2 = v2.second;
    float x3 = v3.first,  y3 = v3.second;

    uint8_t r; 
    uint8_t g ;
    uint8_t b ;
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // Sample at pixel center
            float px = x + 0.5f;
            float py = y + 0.5f;

            // 2. Edge functions (2D cross products)
            float e0 = (x2 - x1) * (py - y1) - (px - x1) * (y2 - y1);
            float e1 = (x3 - x2) * (py - y2) - (px - x2) * (y3 - y2);
            float e2 = (x1 - x3) * (py - y3) - (px - x3) * (y1 - y3);

            // 3. Winding validation: Allow both front-facing and back-facing geometry
            bool is_inside = (e0 <= margin_of_error && e1 <= margin_of_error && e2 <= margin_of_error) || (e0 >= margin_of_error && e1 >= margin_of_error && e2 >= margin_of_error);
            
            if (is_inside) {
                // Compute barycentric weights
                array<float, 3> weight = getweight_z(v1, v2, v3, px, py);
                
               // 1. Calculate the reciprocal of depth for each vertex before the loop
                float inv_z1 = 1.0f / z_values[0];
                float inv_z2 = 1.0f / z_values[1];
                float inv_z3 = 1.0f / z_values[2];

                // ... inside your pixel loop, where you find a valid pixel inside the triangle ...

                // 2. Interpolate the inverted depths linearly using your barycentric weights
                float inv_z = inv_z1 * weight[0] + inv_z2 * weight[1] + inv_z3 * weight[2];
                float u_by_z = uv[0][0]* inv_z1 * weight[0] + uv[1][0]*inv_z2 * weight[1] + uv[2][0]*inv_z3 * weight[2];
                float v_by_z = uv[0][1]* inv_z1 * weight[0] + uv[1][1]*inv_z2 * weight[1] + uv[2][1]*inv_z3 * weight[2];
                float u = u_by_z/inv_z;
                float v = v_by_z /inv_z;
                // 3. Recover the perspective-correct Z value for the Z-buffer test
                float perspective_correct_z = 1.0f / inv_z;
                int texel_x = static_cast<int>(u*(texture.width-1));
                int texel_y = static_cast <int>(v*(texture.height-1));
                int screen_x = x + width / 2;
                int screen_y = y + height / 2;
                int buffer_idx = screen_y * width + screen_x;

                // 4. Run the depth test using the corrected Z
                if (perspective_correct_z < z_buffer[buffer_idx]) {
                    z_buffer[buffer_idx] = perspective_correct_z;
                    uint32_t texture_color = texture.texels[texel_y*texture.width +texel_x];
                   
                    r = (texture_color >> 16) & 0xFF; 
                    g= (texture_color >> 8)  & 0xFF; 
                    b=  texture_color        & 0xFF; 

                    r = static_cast<uint8_t>(r * light_intensity);
                    g = static_cast<uint8_t>(g * light_intensity);
                    b = static_cast<uint8_t>(b * light_intensity);

                    uint32_t shaded_color = (0xFF << 24) | (r << 16) | (g << 8) | b;
        
                    engine.put_pixel(x, y,shaded_color);
                }
                
            }
        }
    }
}