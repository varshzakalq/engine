#include "objloader.h"
#include <sstream>

// Constructor implementation
OBJ::OBJ(const std::string& filename) {
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to process the file: " << filename << std::endl;
    }
    load();
}

void OBJ::load() {
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix; // Read the whole prefix (handles 'v', 'vt', 'vn', 'f')

        // 1. Geometric Vertices
        if (prefix == "v") {
            Vector4 v;
            ss >> v[0] >> v[1] >> v[2];
            v[3] = 1.0; // Default w component
            
            vertex_buffer.push_back(v);
        }
        // 2. Face Indices (Triangles Only for simplicity)
        else if (prefix == "f") {
            std::string token;
            // Read all vertex tokens on the 'f' line
            while (ss >> token) {
                // Find the first slash if it looks like: v/vt/vn or v//vn
                size_t slashIdx = token.find('/');
                std::string cleanIndexStr = (slashIdx == std::string::npos) ? token : token.substr(0, slashIdx);
                
                // Convert to 0-indexed C++ integer
                int objIndex = std::stoi(cleanIndexStr) - 1;
                index_buffer.push_back(objIndex);
            }
        }
    }
    file.close();

    std::cout << "Vertices loaded: " << vertex_buffer.size() << std::endl;
    std::cout << "Indices loaded: " << index_buffer.size() << std::endl;
}