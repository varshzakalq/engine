#include "engine.h"
#include "basic.h"
#include <vector>
#include <utility>
#include <array>
#include<iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "objloader.h"
using namespace std; // Include this here so you can use your Line tool!
using Matrix4x4 = std::array<std::array<double, 4>, 4>;
using Vector4 = std::array<double, 4>;
#define M_PI 3.141592653589793


Vector4 transform(const Matrix4x4& M, const Vector4& v) {
    return {
        M[0][0]*v[0] + M[0][1]*v[1] + M[0][2]*v[2] + M[0][3]*v[3], // Fixed: changed v[4] to v[3]
        M[1][0]*v[0] + M[1][1]*v[1] + M[1][2]*v[2] + M[1][3]*v[3], // Fixed: changed v[4] to v[3]
        M[2][0]*v[0] + M[2][1]*v[1] + M[2][2]*v[2] + M[2][3]*v[3], // Fixed: changed v[4] to v[3]
        M[3][0]*v[0] + M[3][1]*v[1] + M[3][2]*v[2] + M[3][3]*v[3]  // Fixed: changed v[4] to v[3]
    };
}
Engine::Engine() : myLine(*this) ,obj("assets/gun1.obj") {
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
}

void Engine::put_pixel(int x, int y, uint32_t color) {
    //shifting the cordinates to center
    x += width/2;
    y+= height/2;
    if (x >= 0&& x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

void Engine::update_and_render() {
    // 1. CLEAR THE SCREEN FIRST
    if (pixels != nullptr) {
        memset(pixels, 0, width * height * sizeof(uint32_t));
    }
    // Since Line isn't a global header variable anymore, 
    // you can instantiate it right here when you need to draw!
    
    vector<vertex_data> vertex_buffer = obj.vertex_buffer;
    vector <int>&index_buffer = obj.index_buffer;
//  std::vector<Vector4> vertex_buffer = {
//         {-1.0, -1.0, -1.0, 1.0}, // v0
//         { 1.0, -1.0, -1.0, 1.0}, // v1
//         { 1.0,  1.0, -1.0, 1.0}, // v2
//         {-1.0,  1.0, -1.0, 1.0}, // v3
//         {-1.0, -1.0,  1.0, 1.0}, // v4
//         { 1.0, -1.0,  1.0, 1.0}, // v5
//         { 1.0,  1.0,  1.0, 1.0}, // v6
//         {-1.0,  1.0,  1.0, 1.0}  // v7
//     };

//     //index buffer now this will store order later i will make it able to take any file type
// std::vector<int> index_buffer = {
//     // Front Face (Z = -1.0) - Assuming Camera looks down -Z
//     0, 2, 1,
//     0, 3, 2,

//     // Back Face (Z = 1.0)
//     4, 5, 6,
//     4, 6, 7,

//     // Top Face (Y = 1.0)
//     3, 6, 2,
//     3, 7, 6,

//     // Bottom Face (Y = -1.0)
//     0, 1, 5,
//     0, 5, 4,

//     // Right Face (X = 1.0)
//     1, 2, 6,
//     1, 6, 5,

//     // Left Face (X = -1.0)
//     0, 4, 7,
//     0, 7, 3
// };

//just for testing
// std::vector<Vector4> vertex_buffer = {
//     { 0.0,  10.0,  15.0, 1.0 }, // v0: Safely inside (Z > 10)
//     {-10.0, -10.0,   5.0, 1.0 }, // v1: Clipped out   (Z < 10)
//     { 10.0, -10.0,  15.0, 1.0 }  // v2: Safely inside (Z > 10)
// };

// std::vector<int> index_buffer= { 0, 1, 2 };

 
    if(curr_angle>2*M_PI){
        curr_angle -= 2*M_PI;
    }
    double angle = curr_angle; // Convert to radians
    double c = cos(angle);
    double s = sin(angle);
    curr_angle += 0.005;

    Matrix4x4 transformMatrix = {{
        {   c, 0.0,   s, 0.0},  // Row 0: Rotate Y & Translate X by 5
        { 0.0, 1.0, 0.0, 0.0},  // Row 1: No change to Y
        {  -s, 0.0,   c, 20.0 },  // Row 2: Rotate Y & Translate Z by 4
        { 0.0, 0.0, 0.0, 1.0}   // Row 3: Homogeneous row
    }};

//main transformation;

       

            
            
            for (size_t i = 0; i < vertex_buffer.size(); i++) {
                // Transform the original asset data, but save it into our frame-local copy
                transformed_vertices[i].vertex = transform(transformMatrix, vertex_buffer[i].vertex);
        
                 // Pass along the UV coordinates and normals untouched for downstream processing (e.g. lighting/texturing)
                transformed_vertices[i].textures = vertex_buffer[i].textures;
                transformed_vertices[i].normals = vertex_buffer[i].normals;
            }
            
            

            vector <pair<int,int>>vertex_buffer_projected;

           /*for(int i =0;i<vertex_buffer.size();i++){
                pair<int,int> p =projection.onscreen(transformed_vertices[i][0],transformed_vertices[i][1],transformed_vertices[i][2], 1);
                
                put_pixel(p.first,p.second,0xFF00FF);
                vertex_buffer_projected.push_back(p);
                
            }*/

        for(int i = 2;i<index_buffer.size();i+=3){
            auto v1 = transformed_vertices[index_buffer[i-2]].vertex;
            auto v2 = transformed_vertices[index_buffer[i-1]].vertex;
            auto v3 = transformed_vertices[index_buffer[i]].vertex;
            
           
            std::array<Vector4, 4> vertexes = {v1, v2, v3,{0,0,-1,1}}; // z = -1 is a flag used to check if new vertex added due to clipping
            vertexes = clip.get_vertex(vertexes); 
            if (vertexes[0][3] == -999.0) continue; // skip fully clipped triangle // we have put -999 as a marker in camera
            
            array<pair<int,int>, 4> xycoordinates; 
            
            color = light1.flat_shader(v1,v2,v3,color);
            
            // 1. Determine how many valid vertices exist based on your flag
            int num_vertices = (vertexes[3][2] != -1.0) ? 4 : 3;

            // 2. Project only the active vertices into screen space
            for(int j = 0; j < num_vertices; j++){
                xycoordinates[j] = projection.onscreen(vertexes[j][0], vertexes[j][1], vertexes[j][2], cam1.focal_length);
            }
            
            // 3. DRAWING PHASE: Choose one exclusive path to prevent crisscrossed lines
            if (num_vertices == 3) {
                // Standard unclipped triangle
                //myLine.draw(xycoordinates[0], xycoordinates[1], *this);
                //myLine.draw(xycoordinates[1], xycoordinates[2], *this);
                //myLine.draw(xycoordinates[2], xycoordinates[0], *this);
                projection.fill_color(xycoordinates[0],xycoordinates[1],xycoordinates[2],color,width,height,*this); //this obviously means engine reference
            } 
            else if (num_vertices == 4) {
                //this i need to check because it can ruin the textures
                // Triangle clipped into a clean, sequential 4-sided polygon loop
                myLine.draw(xycoordinates[0], xycoordinates[1], *this);
                myLine.draw(xycoordinates[1], xycoordinates[2], *this);
                myLine.draw(xycoordinates[2], xycoordinates[3], *this);
                myLine.draw(xycoordinates[3], xycoordinates[0], *this); // Safely closes the quad perimeter
            }
        }


    
}