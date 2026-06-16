#pragma once
#include "engine.h"
#include <vector>
#include <cmath>

inline Matrix4x4 multiply(const Matrix4x4& A, const Matrix4x4& B) {
    Matrix4x4 result = {0.0}; 

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result[row][col] = A[row][0] * B[0][col] +
                               A[row][1] * B[1][col] +
                               A[row][2] * B[2][col] +
                               A[row][3] * B[3][col];
        }
    }
    return result;
}



// Defined a clean identity matrix constant to initialize instances safely
const Matrix4x4 IDENTITY_MATRIX = {{
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 }
}};

struct Instance {
    int mesh_id;                // Which 3D model to draw
    Matrix4x4 local_transform;  // Transform relative to parent
    Matrix4x4 world_transform;  // Final calculated transform for the GPU

    // Parent/Child pointers for professional hierarchy tracking
    Instance* parent = nullptr;

    float texture_scale_x = 1.0f;  // Texture scaling in U direction
    float texture_scale_y = 1.0f;  // Texture scaling in V direction

    void translate(double x, double y, double z) {
        Matrix4x4 translationMatrix = {{
            { 1.0, 0.0, 0.0, x },
            { 0.0, 1.0, 0.0, y },
            { 0.0, 0.0, 1.0, z },
            { 0.0, 0.0, 0.0, 1.0 }
        }};
        local_transform = multiply(local_transform, translationMatrix);
    } 

    void rotate(double angle, Vector3 axis) {
        double length = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        if (length == 0) return; 
        
        double x = axis.x / length; double y = axis.y / length; double z = axis.z / length;
        double c = std::cos(angle); double s = std::sin(angle); double omc = 1.0 - c; 

        Matrix4x4 rotationMatrix = {{
            { c + x*x*omc,   x*y*omc - z*s, x*z*omc + y*s, 0.0 },
            { y*x*omc + z*s, c + y*y*omc,   y*z*omc - x*s, 0.0 },
            { z*x*omc - y*s, z*y*omc + x*s, c + z*z*omc,   0.0 },
            { 0.0,           0.0,           0.0,           1.0 }
        }};
        local_transform = multiply(local_transform, rotationMatrix);
    }
};

// 3. The Scene Manager tracks all instances and prepares data for the renderer
class Scene {
private:
    std::vector<Instance> m_instances;

public:
    void reserve(size_t count) {
        m_instances.reserve(count);
    }

    Instance& create_instance(int mesh_id) {
        m_instances.push_back({mesh_id, IDENTITY_MATRIX, IDENTITY_MATRIX, nullptr, 1.0f, 1.0f});
        return m_instances.back();
    }
    std::vector<Instance> get_buffer(){
        return m_instances;
    }

    
    void update_transforms() {
        for (auto& inst : m_instances) {
            if (inst.parent != nullptr) {
                // If it has a parent, combine world transforms hierarchically
                inst.world_transform = multiply(inst.parent->world_transform, inst.local_transform);
            } else {
                // Root objects copy local directly to world
                inst.world_transform = inst.local_transform;
            }
        }
    }

    // STEP 2: Expose a clean, flat buffer to pass directly to engine.h's render function
    const std::vector<Instance>& get_render_buffer() const {
        return m_instances;
    }
};