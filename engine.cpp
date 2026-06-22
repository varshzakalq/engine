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
    crate_texture.load_bmp("assets/texture.bmp");
    crate_Map.load_bmp("assets/normal.bmp");
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
void Engine::render_instance(const Instance& inst, vector<point_light> lights) {
    const vector<vertex_data>& vertex_buffer = obj.vertex_buffer;
    const vector<int>& index_buffer = obj.index_buffer;

    // Camera/view transform matrix
    Matrix4x4 view_matrix = {{
        { 1.0, 0.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 1.0 }
    }};

    // Combine: world_transform then view_matrix
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

        // 4. Calculate length and normalize the vector
        double length = sqrt(rotated_normal[0] * rotated_normal[0] +
                             rotated_normal[1] * rotated_normal[1] +
                             rotated_normal[2] * rotated_normal[2]);

        if (length > 0.0001) { 
            transformed_vertices[i].normals.x = rotated_normal[0] / length;
            transformed_vertices[i].normals.y = rotated_normal[1] / length;
            transformed_vertices[i].normals.z = rotated_normal[2] / length;
        } else {
            transformed_vertices[i].normals = {0.0f, 1.0f, 0.0f};
        }

        // 5. Pass along the UV coordinates untouched
        transformed_vertices[i].textures = vertex_buffer[i].textures;
    }

    for (int i = 2; i < (int)index_buffer.size(); i += 3) {
        auto v1 = transformed_vertices[index_buffer[i-2]].vertex;
        auto v2 = transformed_vertices[index_buffer[i-1]].vertex;
        auto v3 = transformed_vertices[index_buffer[i]].vertex;

        // z = -1 is a flag used to check if a new vertex was added due to clipping
        std::array<Vector4, 4> vertexes = {v1, v2, v3, {0,0,-1,1}}; 
        vertexes = clip.get_vertex(vertexes);
        if (vertexes[0][3] == -999.0) continue; // skip fully clipped triangle

        array<pair<int,int>, 4> xycoordinates;

        // Bundle our vertex_data to pass directly to the simplified fill_color function
        array<vertex_data, 3> triangle_vertices = {
            transformed_vertices[index_buffer[i-2]],
            transformed_vertices[index_buffer[i-1]],
            transformed_vertices[index_buffer[i]]
        };

        // Determine how many valid vertices exist based on the flag
        int num_vertices = (vertexes[3][2] != -1.0) ? 4 : 3;

        // Project active vertices into screen space
        for (int j = 0; j < num_vertices; j++) {
            xycoordinates[j] = projection.onscreen(vertexes[j][0], vertexes[j][1], vertexes[j][2], cam1.focal_length);
        }

        // DRAWING PHASE
        if (num_vertices == 3) {
            array<float, 3> intensities = {0.0f, 0.0f, 0.0f};
            
            // FIX: Use the actual (potentially clipped) vertexes positions for accurate lighting calculations
            for (size_t l = 0; l < lights.size(); l++) {
                float d1 = dist(lights[l].position, vertexes[0]);
                float d2 = dist(lights[l].position, vertexes[1]);
                float d3 = dist(lights[l].position, vertexes[2]);

                if (d1 < lights[l].radius) {
                    auto direction = subtract(lights[l].position, vertexes[0]); 
                    float mag_prod = magnitude(triangle_vertices[0].normals) * magnitude(direction);
                    
                    if (mag_prod > 0.0f) {
                        float diffuse = std::max<float>(0.0f, dot(direction, triangle_vertices[0].normals) / mag_prod);
                        intensities[0] += (lights[l].power / (lights[l].constant + lights[l].linear * d1 + lights[l].quadratic * d1 * d1)) * diffuse;
                    }
                }

                if (d2 < lights[l].radius) {
                    auto direction = subtract(lights[l].position, vertexes[1]); 
                    float mag_prod = magnitude(triangle_vertices[1].normals) * magnitude(direction);
                    
                    if (mag_prod > 0.0f) {
                        float diffuse = std::max<float>(0.0f, dot(direction, triangle_vertices[1].normals) / mag_prod);
                        intensities[1] += (lights[l].power / (lights[l].constant + lights[l].linear * d2 + lights[l].quadratic * d2 * d2)) * diffuse;
                    }
                }

                if (d3 < lights[l].radius) {
                    auto direction = subtract(lights[l].position, vertexes[2]); 
                    float mag_prod = magnitude(triangle_vertices[2].normals) * magnitude(direction);
                    
                    if (mag_prod > 0.0f) {
                        float diffuse = std::max<float>(0.0f, dot(direction, triangle_vertices[2].normals) / mag_prod);
                        intensities[2] += (lights[l].power / (lights[l].constant + lights[l].linear * d3 + lights[l].quadratic * d3 * d3)) * diffuse;
                    }
                }
            }
            
            // Call our newly streamlined fill_color function
            projection.fill_color(
                triangle_vertices, 
                intensities, 
                xycoordinates[0], 
                xycoordinates[1], 
                xycoordinates[2], 
                crate_texture, crate_Map, 
                z_buffer, 
                width, 
                height, 
                *this
            );
        }
        else if (num_vertices == 4) {
            myLine.draw(xycoordinates[0], xycoordinates[1], *this);
            myLine.draw(xycoordinates[1], xycoordinates[2], *this);
            myLine.draw(xycoordinates[2], xycoordinates[3], *this);
            myLine.draw(xycoordinates[3], xycoordinates[0], *this);
        }
    }
}

void Engine::update_and_render(const std::vector<Instance>& instances,const vector<point_light> lights) {
    // 1. CLEAR THE SCREEN FIRST
    if (pixels != nullptr) {
        memset(pixels, 0, width * height * sizeof(uint32_t));
    }
    fill_n(z_buffer.begin(), width * height, deapth);

    for (const Instance& inst : instances) {
        render_instance(inst,lights);
    }
}
