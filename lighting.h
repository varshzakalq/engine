#pragma once
#include <cmath>
#include "objloader.h"
#include <algorithm>
#include <cstdint>
#include <array>
using namespace std;

class lighting{
    Vector3 light_pos = {0,0,1.0};
    Vector3 smooth_light = {0,0,-1.0};
    uint32_t light_color;
    double ambient_light =0.1;
public:
    int scale = 5;
    double flat_shader(array<double,4> v1,array<double,4>v2,array<double,4>v3);
    float smooth_shader(Vector3 n1);
};