
#include "lighting.h"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;



double lighting:: flat_shader(array<double,4>v1,array<double,4>v2,array<double,4>v3){
    
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

    
    return intensity;

}
float lighting::smooth_shader(Vector3 normal ){
    
    double intensity = dot(normal,smooth_light);
    intensity = std::max(ambient_light, std::min(1.0, intensity));
    //this turns off the main light _________________??>><>?<<>>><<>>??<<
    
    return static_cast<float>(intensity);

}