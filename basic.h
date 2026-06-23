#pragma once

#include <utility>
#include <cmath>
#include<algorithm>
#include "lighting.h"
using namespace std;
class Engine;
class Texture;
class normalMap;
class line{
    Engine &e;
public:
    line(Engine &eng);
    void draw(pair<int,int>v1,pair<int,int>v2,Engine &e);
};
class project{
int size = 600;
lighting light1;
float margin_of_error = 0.01f; 
float texture_scale_x  = 3.0f;
float texture_scale_y = 3.0f;
public:
pair<int, int> onscreen(double x1, double y1, double z1, float focal_length) ;
void fill_color(
     const array<vertex_data, 3> &vertexes, 
     const array<float, 3> &intensities,    
     const pair<int, int> &v1, 
     const pair<int, int> &v2, 
     const pair<int, int> &v3, 
     Texture &texture,   normalMap &normal_map,Texture &ao_map,
     vector<float>& z_buffer, 
     int width, 
     int height, 
     Engine &engine
    ) ;
    void draw_panorama_skybox(
    const Texture &panorama_bmp,      // Your single flat 360° BMP image
    const Matrix4x4 &inv_proj_matrix,  // Inverse of your projection matrix
    const Matrix4x4 &inv_view_matrix,  // Inverse of your view matrix
    int width, int height,
    Engine &engine
);
};