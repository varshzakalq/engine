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
Engine::Engine() : myLine(*this) ,obj("m4a1_s.obj") {
    pixels = nullptr;
    width = 0;
    height = 0;
    curr_angle = 0;
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
   
    //vector<Vector4>& vertex_buffer = obj.vertex_buffer;
    //vector <int>&index_buffer = obj.index_buffer;
/* std::vector<Vector4> vertex_buffer = {
        {-1.0, -1.0, -1.0, 1.0}, // v0
        { 1.0, -1.0, -1.0, 1.0}, // v1
        { 1.0,  1.0, -1.0, 1.0}, // v2
        {-1.0,  1.0, -1.0, 1.0}, // v3
        {-1.0, -1.0,  1.0, 1.0}, // v4
        { 1.0, -1.0,  1.0, 1.0}, // v5
        { 1.0,  1.0,  1.0, 1.0}, // v6
        {-1.0,  1.0,  1.0, 1.0}  // v7
    };

    //index buffer now this will store order later i will make it able to take any file type
    vector<int> index_buffer ={
        0,1,2,
        0,2,3,
        3,2,6,
        3,6,7,
        6,5,4,
        6,7,4,
        2,6,5,
        2,1,5,
        0,1,5,
        0,4,5,
        0,3,7,
        0,4,7,

    };
*/
std::vector<Vector4> vertex_buffer = {
    { 0.0,  10.0,  15.0, 1.0 }, // v0: Safely inside (Z > 10)
    {-10.0, -10.0,   5.0, 1.0 }, // v1: Clipped out   (Z < 10)
    { 10.0, -10.0,  15.0, 1.0 }  // v2: Safely inside (Z > 10)
};

std::vector<int> index_buffer= { 0, 1, 2 };

 
    if(curr_angle>2*M_PI){
        curr_angle -= 2*M_PI;
    }
    double angle = curr_angle; // Convert to radians
    double c = cos(angle);
    double s = sin(angle);
    curr_angle += 0.01;

    Matrix4x4 transformMatrix = {{
        {   1, 0.0,   0, 0.0},  // Row 0: Rotate Y & Translate X by 5
        { 0.0, 1.0, 0.0, 0.0},  // Row 1: No change to Y
        {  0, 0.0,   1, 10.0 },  // Row 2: Rotate Y & Translate Z by 4
        { 0.0, 0.0, 0.0, 1.0}   // Row 3: Homogeneous row
    }};

//main transformation;

       

            for (size_t i = 0; i < vertex_buffer.size(); i++) {
                // Transform the original asset data, but save it into our frame-local copy
                transformed_vertices[i] = transform(transformMatrix, vertex_buffer[i]);
            }

            vector <pair<int,int>>vertex_buffer_projected;

           /*for(int i =0;i<vertex_buffer.size();i++){
                pair<int,int> p =projection.onscreen(transformed_vertices[i][0],transformed_vertices[i][1],transformed_vertices[i][2], 1);
                
                put_pixel(p.first,p.second,0xFF00FF);
                vertex_buffer_projected.push_back(p);
                
            }*/

        for(int i = 2;i<index_buffer.size();i+=3){
            auto v1 = transformed_vertices[index_buffer[i-2]];
            auto v2 = transformed_vertices[index_buffer[i-1]];
            auto v3 = transformed_vertices[index_buffer[i]];
            
           

            // Instead of a vector, use a fixed stack array or std::array
            std::array<Vector4, 4> vertexes = {v1, v2, v3,{0,0,-1,1}}; // z = -1 is a flag used to check if new vertex added due to clipping


            vertexes = clip.get_vertex(vertexes); //clips the vertexes


            // Pre-allocate xycoordinates with a fixed max size on the stack
           // Pre-allocate xycoordinates with a fixed max size on the stack
            std::array<pair<int,int>, 4> xycoordinates; 

            // 1. Determine how many valid vertices exist based on your flag
            int num_vertices = (vertexes[3][2] != -1.0) ? 4 : 3;

            // 2. Project only the active vertices into screen space
            for(int j = 0; j < num_vertices; j++){
                xycoordinates[j] = projection.onscreen(vertexes[j][0], vertexes[j][1], vertexes[j][2], cam1.focal_length);
            }
            
            // 3. DRAWING PHASE: Choose one exclusive path to prevent crisscrossed lines
            if (num_vertices == 3) {
                // Standard unclipped triangle
                myLine.draw(xycoordinates[0], xycoordinates[1], *this);
                myLine.draw(xycoordinates[1], xycoordinates[2], *this);
                myLine.draw(xycoordinates[2], xycoordinates[0], *this);
            } 
            else if (num_vertices == 4) {
                // Triangle clipped into a clean, sequential 4-sided polygon loop
                myLine.draw(xycoordinates[0], xycoordinates[1], *this);
                myLine.draw(xycoordinates[1], xycoordinates[2], *this);
                myLine.draw(xycoordinates[2], xycoordinates[3], *this);
                myLine.draw(xycoordinates[3], xycoordinates[0], *this); // Safely closes the quad perimeter
            }
        }

// this fills the triangle 
                /*auto v0 = vertex_buffer_projected[index_buffer[0]] ;
                auto v1 =  vertex_buffer_projected[index_buffer[1]];
                auto v2 =  vertex_buffer_projected[index_buffer[2]];

                int minX = max(0,min({v0.first,v1.first,v2.first}));
                int minY = min({v0.second,v1.second,v2.second});
                int maxX = min(width-1,max({v0.first,v1.first,v2.first}));
                int maxY = min(height-1,max({v0.second,v1.second,v2.second}));
                //cout<<maxY<<" " <<minY<< " " <<v0.second<<" "<<v1.second<< " "<<v2.second<<endl ;
                for(int y = minY;y<=maxY;y++){
                    for(int x = minX;x<=maxX;x++){
                        
                        float px = (float)x+0.5f;
                        float py = (float)y+0.5f;
                        //cheking pixel in correct side;
                        float    e0 = (v1.first-v0.first)*(py-v0.second) - (px- v0.first)*(v1.second-v0.second);
                        float    e1 = (v2.first-v1.first)*(py-v1.second) - (px- v1.first)*(v2.second-v1.second);
                        float    e2 = (v0.first-v2.first)*(py-v2.second) - (px- v2.first)*(v0.second-v2.second);
                        if(e0>= 0&& e1>= 0 && e2>= 0){
                            uint32_t white = 0xffffffff;
                            put_pixel(x,y,white);
                        }
                    }
}*/
    
}