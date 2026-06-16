#include "engine.h"
#include "basic.h"
#include "main_scene.h"
#include <vector>
#include <utility>
#include <array>
#include<iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "objloader.h"
using namespace std; // Include this here so you can use your Line tool!


Vector4 transform(const Matrix4x4& M, const Vector4& v) {
    return {
        M[0][0]*v[0] + M[0][1]*v[1] + M[0][2]*v[2] + M[0][3]*v[3],
        M[1][0]*v[0] + M[1][1]*v[1] + M[1][2]*v[2] + M[1][3]*v[3],
        M[2][0]*v[0] + M[2][1]*v[1] + M[2][2]*v[2] + M[2][3]*v[3],
        M[3][0]*v[0] + M[3][1]*v[1] + M[3][2]*v[2] + M[3][3]*v[3]
    };
}

Engine::Engine() : myLine(*this) ,obj("assets/cube.obj") {
    pixels = nullptr;
    width = 0;
    height = 0;
    curr_angle = 0;
    color = 0xffff500;
    
}

// 2. Add the Destructor implementation
Engine::~Engine() {
    // Empty for now, unless you allocate memory with 'new' later
}
void Engine::init(uint32_t* pixel_array, int w, int h) {
    pixels = pixel_array;
    transformed_vertices.resize(obj.vertex_buffer.size());
    
    width = w;
    height = h;
    z_buffer.resize(w*h);
    fill_n(z_buffer.begin(), w * h, deapth);
    crate_texture.load_bmp("assets/crate_1.bmp");
}

void Engine::put_pixel(int x, int y, uint32_t color) {
    //shifting the cordinates to center
    x += width/2;
    y+= height/2;
    if (x >= 0&& x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

// Renders a single instance using its world_transform, combined with the
// camera/view transform that pushes everything into view of the camera.
void Engine::render_instance(const Instance& inst) {
    const vector<vertex_data>& vertex_buffer = obj.vertex_buffer;
    const vector<int>& index_buffer = obj.index_buffer;

    // Camera/view transform: pushes the whole scene out in front of the camera.
    // (The original code's hardcoded rotation+translate is kept here as the view matrix.)
    Matrix4x4 view_matrix = {{
        { 1.0, 0.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 1.0 }
    }};

    // Combine: world_transform (per-instance position/rotation) then view_matrix
    Matrix4x4 final_matrix = multiply(view_matrix, inst.world_transform);

    for (size_t i = 0; i < vertex_buffer.size(); i++) {
        // 1. Transform Position
        transformed_vertices[i].vertex = transform(final_matrix, vertex_buffer[i].vertex);

        // 2. Set up the raw normal vector with w = 0.0 to discard translation
        Vector4 raw_normal = {
            vertex_buffer[i].normals.x,
            vertex_buffer[i].normals.y,
            vertex_buffer[i].normals.z,
            0.0
        };

        // 3. Transform the normal vector
        Vector4 rotated_normal = transform(final_matrix, raw_normal);

        // 4. CRITICAL STEP: Calculate length and normalize the vector!
        double length = sqrt(rotated_normal[0] * rotated_normal[0] +
                            rotated_normal[1] * rotated_normal[1] +
                            rotated_normal[2] * rotated_normal[2]);

        if (length > 0.0001) { // Prevent division by zero
            transformed_vertices[i].normals.x = rotated_normal[0] / length;
            transformed_vertices[i].normals.y = rotated_normal[1] / length;
            transformed_vertices[i].normals.z = rotated_normal[2] / length;
        } else {
            // Fallback default normal if math collapses
            transformed_vertices[i].normals = {0.0f, 1.0f, 0.0f};
        }

        // 5. Pass along the UV coordinates untouched
        transformed_vertices[i].textures = vertex_buffer[i].textures;
    }

    for(int i = 2; i < (int)index_buffer.size(); i+=3){
        auto v1 = transformed_vertices[index_buffer[i-2]].vertex;
        auto v2 = transformed_vertices[index_buffer[i-1]].vertex;
        auto v3 = transformed_vertices[index_buffer[i]].vertex;

        std::array<Vector4, 4> vertexes = {v1, v2, v3,{0,0,-1,1}}; // z = -1 is a flag used to check if new vertex added due to clipping
        vertexes = clip.get_vertex(vertexes);
        if (vertexes[0][3] == -999.0) continue; // skip fully clipped triangle

        array<pair<int,int>, 4> xycoordinates;

        float intensity = light1.flat_shader(v1,v2,v3);

        array<float,3> z_values ={static_cast<float>(v1[2]),static_cast<float>(v2[2]),static_cast<float>(v3[2])};
        array<Vector3,3> normals = {
            transformed_vertices[index_buffer[i-2]].normals,
            transformed_vertices[index_buffer[i-1]].normals,
            transformed_vertices[index_buffer[i]].normals
        };
        array<array<float, 2>, 3> uv_coords = {
            transformed_vertices[index_buffer[i-2]].textures,
            transformed_vertices[index_buffer[i-1]].textures,
            transformed_vertices[index_buffer[i]].textures
        };

        // 1. Determine how many valid vertices exist based on your flag
        int num_vertices = (vertexes[3][2] != -1.0) ? 4 : 3;

        // 2. Project only the active vertices into screen space
        for(int j = 0; j < num_vertices; j++){
            xycoordinates[j] = projection.onscreen(vertexes[j][0], vertexes[j][1], vertexes[j][2], cam1.focal_length);
        }

        // 3. DRAWING PHASE: Choose one exclusive path to prevent crisscrossed lines
        if (num_vertices == 3) {
            projection.fill_color(normals,z_values,uv_coords,crate_texture,intensity,xycoordinates[0],xycoordinates[1],xycoordinates[2],z_buffer,color,width,height,*this);
        }
        else if (num_vertices == 4) {
            myLine.draw(xycoordinates[0], xycoordinates[1], *this);
            myLine.draw(xycoordinates[1], xycoordinates[2], *this);
            myLine.draw(xycoordinates[2], xycoordinates[3], *this);
            myLine.draw(xycoordinates[3], xycoordinates[0], *this);
        }
    }
}

void Engine::update_and_render(const std::vector<Instance>& instances) {
    // 1. CLEAR THE SCREEN FIRST
    if (pixels != nullptr) {
        memset(pixels, 0, width * height * sizeof(uint32_t));
    }
    fill_n(z_buffer.begin(), width * height, deapth);

    for (const Instance& inst : instances) {
        render_instance(inst);
    }
}
