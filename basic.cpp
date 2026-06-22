#include "engine.h"
#include "basic.h"
#include <utility>
using namespace std;
#include "basic.h"
#include "engine.h"
#include <cmath> // Required for std::abs
#include "texture.h"

line::line(Engine &eng): e(eng){

}
void line::draw(pair<int,int>v1,pair<int,int>v2, Engine &engine) {
    // 1. Calculate absolute distances
    int x1 = v1.first;
    int y1 = v1.second;
    int x2 = v2.first;
    int y2 = v2.second;
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    
    // 2. Track step direction (-1 if moving backward, 1 if moving forward)
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    
    int x = x1;
    int y = y1;
    uint32_t green = 0x00FF00FF; // Your line color

    // 3. Shallow lines (horizontal movement is dominant)
    if (dx > dy) {
        int p = 2 * dy - dx;
        while (x != x2) {
            engine.put_pixel(x, y, green);
            x += sx; // Moves forward or backward automatically!
            
            if (p >= 0) {
                y += sy; // Moves up or down automatically!
                p += 2 * (dy - dx);
            } else {
                p += 2 * dy;
            }
        }
    } 
    // 4. Steep lines (vertical movement is dominant)
    else {
        int p = 2 * dx - dy;
        while (y != y2) {
            engine.put_pixel(x, y, green);
            y += sy; // Moves up or down automatically!
            
            if (p >= 0) {
                x += sx; // Moves forward or backward automatically!
                p += 2 * (dx - dy);
            } else {
                p += 2 * dx;
            }
        }
    }
    
    // 5. Always catch the very last pixel
    engine.put_pixel(x2, y2, green);
}

array<float, 3> getweight_z(pair<int, int> v1, pair<int, int> v2, pair<int, int> v3, float px, float py) {
    array<float, 3> weights;
    
    float denom = static_cast<float>((v2.second - v3.second) * (v1.first - v3.first) + (v3.first - v2.first) * (v1.second - v3.second));
    
    // Avoid division by zero on degenerate triangles
    if (std::abs(denom) < 1e-6f) {
        return {0.0f, 0.0f, 0.0f};
    }

    weights[0] = ((v2.second - v3.second) * (px - v3.first) + (v3.first - v2.first) * (py - v3.second)) / denom;
    weights[1] = ((v3.second - v1.second) * (px - v3.first) + (v1.first - v3.first) * (py - v3.second)) / denom;
    weights[2] = 1.0f - weights[0] - weights[1];
    return weights;
}

pair<int, int> project::onscreen(double x1, double y1, double z1, float focal_length) {
    // Avoid division by zero if geometry passes behind camera
    if (std::abs(z1) < 1e-4) z1 = 1e-4; 

    float x = static_cast<float>(focal_length / z1 * x1 * size);
    float y = static_cast<float>(focal_length / z1 * y1 * size);
    
    return { static_cast<int>(std::round(x)), static_cast<int>(std::round(y)) };
}
void project::fill_color(
     const array<vertex_data, 3> &vertexes, 
     const array<float, 3> &intensities,    // Pre-calculated point light intensities
     const pair<int, int> &v1, 
     const pair<int, int> &v2, 
     const pair<int, int> &v3, 
     Texture &texture, normalMap &normal_map,
     vector<float>& z_buffer, 
     int width, 
     int height, 
     Engine &engine
    ) {
    
    // 1. Compute and safely clamp bounding box
    int minX = max(-width / 2,     min({v1.first, v2.first, v3.first}));
    int minY = max(-height / 2,    min({v1.second, v2.second, v3.second}));
    int maxX = min(width / 2 - 1,  max({v1.first, v2.first, v3.first}));
    int maxY = min(height / 2 - 1, max({v1.second, v2.second, v3.second}));

    float x1 = v1.first,  y1 = v1.second;
    float x2 = v2.first,  y2 = v2.second;
    float x3 = v3.first,  y3 = v3.second;
    
    // Cache vertex depths and attributes outside the loop
    float z1 = vertexes[0].vertex[2];
    float z2 = vertexes[1].vertex[2];
    float z3 = vertexes[2].vertex[2];

    float inv_z1 = 1.0f / z1;
    float inv_z2 = 1.0f / z2;
    float inv_z3 = 1.0f / z3;

    // Pre-multiply texture coordinates by inverse depth
    float u1_z = vertexes[0].textures[0] * inv_z1;
    float u2_z = vertexes[1].textures[0] * inv_z2;
    float u3_z = vertexes[2].textures[0] * inv_z3;

    float v1_z = vertexes[0].textures[1] * inv_z1;
    float v2_z = vertexes[1].textures[1] * inv_z2;
    float v3_z = vertexes[2].textures[1] * inv_z3;

    // Pre-multiply point light intensities by inverse depth
    float point_lit1_z = intensities[0] * inv_z1;
    float point_lit2_z = intensities[1] * inv_z2;
    float point_lit3_z = intensities[2] * inv_z3;

    // =================================================================
    // PRE-CALCULATE TBN SPACE USING WORLD-SPACE EDGES
    // =================================================================
    Vector3 edge1 = subtract(vertexes[1].vertex, vertexes[0].vertex);
    Vector3 edge2 = subtract(vertexes[2].vertex, vertexes[0].vertex);

    float deltaU1 = vertexes[1].textures[0] - vertexes[0].textures[0];
    float deltaV1 = vertexes[1].textures[1] - vertexes[0].textures[1];
    float deltaU2 = vertexes[2].textures[0] - vertexes[0].textures[0];
    float deltaV2 = vertexes[2].textures[1] - vertexes[0].textures[1];

    float divisor = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
    float f = (std::abs(divisor) < 1e-6f) ? 1.0f : 1.0f / divisor;

    Vector3 face_tangent;
    face_tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
    face_tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
    face_tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);
    face_tangent = Normalize(face_tangent);

    Vector3 face_normal = Normalize(cross(edge1, edge2));
    Vector3 face_bitangent = Normalize(cross(face_normal, face_tangent));
    // =================================================================
    
    for (int y = minY; y <= maxY; y++) {
        float py = y + 0.5f;
        int screen_y = y + height / 2;
        int buffer_row_offset = screen_y * width + (width / 2);
        
        for (int x = minX; x <= maxX; x++) {
            float px = x + 0.5f;

            // Edge functions (2D cross products)
            float e0 = (x2 - x1) * (py - y1) - (px - x1) * (y2 - y1);
            float e1 = (x3 - x2) * (py - y2) - (px - x2) * (y3 - y2);
            float e2 = (x1 - x3) * (py - y3) - (px - x3) * (y1 - y3);

            if ((e0 <= margin_of_error && e1 <= margin_of_error && e2 <= margin_of_error) || 
                (e0 >= margin_of_error && e1 >= margin_of_error && e2 >= margin_of_error)) {
                
                // Compute barycentric weights
                array<float, 3> weight = getweight_z(v1, v2, v3, px, py);
                
                // Interpolate depth
                float inv_z = inv_z1 * weight[0] + inv_z2 * weight[1] + inv_z3 * weight[2];
                float perspective_correct_z = 1.0f / inv_z;

                int buffer_idx = buffer_row_offset + x;

                // Early Depth Test
                if (perspective_correct_z >= z_buffer[buffer_idx]) continue;

                // Perspective correct UV texture mapping
                float u_by_z = u1_z * weight[0] + u2_z * weight[1] + u3_z * weight[2];
                float v_by_z = v1_z * weight[0] + v2_z * weight[1] + v3_z * weight[2];
                float u = u_by_z / inv_z * texture_scale_x;
                float v = v_by_z / inv_z * texture_scale_y;
                
                int texel_x = ((static_cast<int>(u * texture.width)) % texture.width + texture.width) % texture.width;
                int texel_y = ((static_cast<int>(v * texture.height)) % texture.height + texture.height) % texture.height;

                z_buffer[buffer_idx] = perspective_correct_z;
                uint32_t texture_color = texture.texels[texel_y * texture.width + texel_x];
                
                // 1. POINT LIGHT PASS: Interpolate vertex light levels across triangle face
                float point_lit_by_z = point_lit1_z * weight[0] + point_lit2_z * weight[1] + point_lit3_z * weight[2];
                float point_light_intensity = point_lit_by_z / inv_z;

                // 2. DIRECTIONAL LIGHT PASS: Read normal map data
                uint32_t normal_map_value = normal_map.texels[texel_y * texture.width + texel_x];
                Vector3 normals_from_map = extract_rgb(normal_map_value);
                
                // Remap normal coordinates safely from [0.0, 1.0] to [-1.0, 1.0]
                scale(normals_from_map, 0.0f, 2.0f);
                normals_from_map.x -= 1.0f;
                normals_from_map.y -= 1.0f; 
                normals_from_map.z -= 1.0f;
                
                // Set normal map accent detail level
                float normal_strength = 1.0f; 
                normals_from_map.x *= normal_strength;
                normals_from_map.y *= normal_strength;
                normals_from_map.z = std::sqrt(std::max(0.0, 1.0 - (normals_from_map.x * normals_from_map.x + normals_from_map.y * normals_from_map.y)));
                normals_from_map = Normalize(normals_from_map);

                // Transform normal map sampling using World-Space TBN
                Vector3 transformed_world_normal;
                transformed_world_normal.x = face_tangent.x * normals_from_map.x + face_bitangent.x * normals_from_map.y + face_normal.x * normals_from_map.z;
                transformed_world_normal.y = face_tangent.y * normals_from_map.x + face_bitangent.y * normals_from_map.y + face_normal.y * normals_from_map.z;
                transformed_world_normal.z = face_tangent.z * normals_from_map.x + face_bitangent.z * normals_from_map.y + face_normal.z * normals_from_map.z;
                transformed_world_normal = Normalize(transformed_world_normal);

                // FIX: Pass the modified world normal into your custom shader function!
                // (Note: Update 'light1' if your active lighting class instantiation variable is named differently)
                float dir_light_intensity = light1.smooth_shader(transformed_world_normal);

                // 3. COMBINE LIGHT SYSTEMS
                float total_intensity = dir_light_intensity + point_light_intensity;
                total_intensity = std::max(0.0f, std::min(1.0f, total_intensity));

                // Render shaded color out to pipeline
                Vector3 color = extract_rgb(texture_color);
                color.x *= total_intensity;
                color.y *= total_intensity;
                color.z *= total_intensity;
                
                uint32_t shaded_color = rgb_to_color(color);
                engine.put_pixel(x, y, shaded_color);
            }
        }
    }
}