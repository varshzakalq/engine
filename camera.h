#pragma once
#include <vector>
#include <array>
using namespace std;
using Vector4 = std::array<double, 4>;

class camera{
public:
    float focal_length = 5;
};
class clipper{
    int clipping_plane_z= 10;
    
    array<double, 4> clip(array<double, 4>,array<double, 4>);
public:  
    array<std::array<double, 4>,2 >clipped_vertex;
    array<Vector4,4> get_vertex(array<Vector4,4>& vetex_buffer);
};