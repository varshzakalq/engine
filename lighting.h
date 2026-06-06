#pragma once
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <array>
using namespace std;
struct Vector3 { double x, y, z; };
class lighting{
    Vector3 light_pos = {0,0,1.0};
    uint32_t light_color;
    double ambient_light =0.1;
public:
    uint32_t flat_shader(array<double,4> v1,array<double,4>v2,array<double,4>v3,uint32_t base_color);
};