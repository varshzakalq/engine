#pragma once
#include <cmath>
#include "objloader.h"
#include "maths.h"
#include <algorithm>
#include <cstdint>
#include <array>
using namespace std;
struct point_light{
    int radius;
    Vector3 position;
    Vector3 color;
    float constant  = 1.0f;
    float linear = 0.07f;
    float quadratic = 0.03f;
    int power = 10;
};

class lighting{
    
    Vector3 smooth_light = {0,0,-1.0};
    uint32_t light_color;
    
public:
    Vector3 sky_color    = { 0.3f, 0.4f, 0.6f }; 
    Vector3 ground_color = { 0.15f, 0.12f, 0.1f };
    Vector3 light_pos = {0,0,1.0};
    int scale = 5;
    
    double flat_shader(array<double,4> v1,array<double,4>v2,array<double,4>v3);
    float smooth_shader(Vector3 n1);
};