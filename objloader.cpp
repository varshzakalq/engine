#include "objloader.h"
#include <sstream>
#include <unordered_map>
#include <vector>
#include <array>
#include <iostream>



// Define a structure to represent the combination of indices from the file
struct OBJVertex {
    int v_idx, vt_idx, vn_idx;

    bool operator==(const OBJVertex& other) const {
        return v_idx == other.v_idx && vt_idx == other.vt_idx && vn_idx == other.vn_idx;
    }
};

// Custom hash function for OBJVertex
struct OBJVertexHash {
    std::size_t operator()(const OBJVertex& k) const {
        return ((std::hash<int>()(k.v_idx) ^ 
                (std::hash<int>()(k.vt_idx) << 1)) >> 1) ^ 
               (std::hash<int>()(k.vn_idx) << 1);
    }
};

OBJ::OBJ(const std::string& filename) {
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to process the file: " << filename << std::endl;
    }
    load();
}

void OBJ::load() {
    if (!file.is_open()) return;

    // Temporary storage for raw values read from the file
    std::vector<Vector4> raw_positions;
    std::vector<Vector3> raw_normals; 
    std::vector<std::array<float, 2>> raw_uvs;  

    // Tracks unique combinations of vertex attributes
    std::unordered_map<OBJVertex, int, OBJVertexHash> unique_vertices;

    // Clear your final buffers
    vertex_buffer.clear();
    index_buffer.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        // 1. Gather raw positions
        if (prefix == "v") {
            Vector4 v;
            ss >> v[0] >> v[1] >> v[2];
            v[3] = 1.0f;
            raw_positions.push_back(v);
        }
        // 2. Gather raw texture coordinates (UVs)
        else if (prefix == "vt") {
            std::array<float, 2> uv;
            ss >> uv[0] >> uv[1];
            raw_uvs.push_back(uv);
        }
        // 3. Gather raw normals
        else if (prefix == "vn") {
            Vector3 n;
            ss >> n[0] >> n[1] >> n[2];
            raw_normals.push_back(n);
        }
        // 4. Process faces and build the optimized packed buffers
        else if (prefix == "f") {
            std::string token;
            while (ss >> token) {
                // Parse the "v/vt/vn" token
                int v_idx = 0, vt_idx = 0, vn_idx = 0;
                
                std::stringstream token_stream(token);
                std::string v_str, vt_str, vn_str;

                std::getline(token_stream, v_str, '/');
                std::getline(token_stream, vt_str, '/');
                std::getline(token_stream, vn_str, '/');

                // Convert to 0-indexed integers (handle empty strings for missing attributes)
                v_idx = (!v_str.empty()) ? std::stoi(v_str) - 1 : -1;
                vt_idx = (!vt_str.empty()) ? std::stoi(vt_str) - 1 : -1;
                vn_idx = (!vn_str.empty()) ? std::stoi(vn_str) - 1 : -1;

                OBJVertex current_vertex = { v_idx, vt_idx, vn_idx };

                // Check if we've already created a packed vertex for this specific combination
                auto it = unique_vertices.find(current_vertex);
                if (it != unique_vertices.end()) {
                    // Reuse existing index
                    index_buffer.push_back(it->second);
                } else {
                    // New unique vertex combination! 
                    int new_index = static_cast<int>(vertex_buffer.size());
                    
                    vertex_data data;

                    // Fetch position (OBJ files require positions, safely assume valid index)
                    data.vertex = raw_positions[v_idx];
                    
                    // Fetch normal if available, otherwise default to zero
                    if (vn_idx >= 0 && vn_idx < static_cast<int>(raw_normals.size())) {
                        data.normals = raw_normals[vn_idx];
                    } else {
                        data.normals = Vector3{0.0f, 0.0f, 0.0f};
                    }

                    // Fetch UV if available, otherwise default to zero
                    if (vt_idx >= 0 && vt_idx < static_cast<int>(raw_uvs.size())) {
                        data.textures = raw_uvs[vt_idx];
                    } else {
                        data.textures = {0.0f, 0.0f};
                    }

                    // Push the fully packed struct into your vertex buffer
                    vertex_buffer.push_back(data); 
                    
                    // Record it in the map and push to the index buffer
                    unique_vertices[current_vertex] = new_index;
                    index_buffer.push_back(new_index);
                }
            }
        }
    }
    file.close();

    std::cout << "Optimized Unique Vertices loaded: " << vertex_buffer.size() << std::endl;
    std::cout << "Indices loaded: " << index_buffer.size() << std::endl;
}