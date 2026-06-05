#pragma once
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <iostream>

// Match the type alias you used
using Vector4 = std::array<double, 4>;

class OBJ
{
private:
    std::ifstream file;
    
public:
    std::vector<Vector4> vertex_buffer;
    std::vector<int> index_buffer;

    // Constructor: Takes the file path and opens it s
    OBJ(const std::string& filename);

    // One function to parse the entire file and fill both buffers
    void load();
};