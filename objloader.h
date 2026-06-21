#pragma once
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <iostream>
#include "maths.h"

using Vector4 = std::array<double, 4>;
using Matrix4x4 = std::array<std::array<double, 4>, 4>;

struct vertex_data{
    Vector4 vertex;
    Vector3 normals;
    std::array<float,2>textures;

};
class OBJ
{
private:
    std::ifstream file;
    
public:
    std::vector<vertex_data> vertex_buffer;
    std::vector<int> index_buffer;

    // Constructor: Takes the file path and opens it s
    OBJ(const std::string& filename);

    // One function to parse the entire file and fill both buffers
    void load();
};