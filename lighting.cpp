#include "lighting.h"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

Vector3 subtract(array<double,4>v1,array<double,4>v2){
    return {v1[0]-v2[0],v1[1]-v2[1],v1[2]-v2[2]};
}
Vector3 cross(Vector3 v1,Vector3 v2){
    return {
        v1.y*v2.z-v1.z*v2.y,
        -v1.x*v2.z+ v1.z*v2.x,
        v1.x*v2.y - v1.y*v2.x
    };
}
double dot(Vector3 v1,Vector3 v2){
    return v1.x*v2.x +v1.y*v2.y + v1.z*v2.z;
}
uint32_t lighting:: flat_shader(array<double,4>v1,array<double,4>v2,array<double,4>v3,uint32_t base_color){
    base_color= 0xFFFFA500;
    Vector3 e1 = subtract(v2,v1);
    Vector3 e2 = subtract(v3,v1);
    Vector3 e2_cross_e1= cross(e2,e1);
    double length = sqrt(e2_cross_e1.x*e2_cross_e1.x +e2_cross_e1.y*e2_cross_e1.y + e2_cross_e1.z*e2_cross_e1.z);
    e2_cross_e1.x/= length;
    e2_cross_e1.y/= length;
    e2_cross_e1.z/= length;
    double intensity = dot(e2_cross_e1,light_pos);
    intensity = std::max(ambient_light, std::min(1.0, intensity));
    // Temporary debug test inside your flat_shader function:
    // 0xFFFFA500

    uint8_t r = (base_color >> 16) & 0xFF; // Should print 255
    uint8_t g = (base_color >> 8)  & 0xFF; // Should print 165
    uint8_t b =  base_color        & 0xFF; // Should print 0


    r = static_cast<uint8_t>(r * intensity);
    g = static_cast<uint8_t>(g * intensity);
    b = static_cast<uint8_t>(b * intensity);

    uint32_t shaded_color = (0xFF << 24) | (r << 16) | (g << 8) | b;
        
    return shaded_color;

}