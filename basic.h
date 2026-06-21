#pragma once

#include <utility>
#include <cmath>
#include<algorithm>
#include "lighting.h"
using namespace std;
class Engine;
class Texture;
class line{
    Engine &e;
public:
    line(Engine &eng);
    void draw(pair<int,int>v1,pair<int,int>v2,Engine &e);
};
class project{
int size = 300;
lighting light1;
float margin_of_error = 0.01f; 
float texture_scale_x  = 4.0f;
float texture_scale_y = 4.0f;
public:
pair<int, int> onscreen(double x1, double y1, double z1, float focal_length) ;
void fill_color(
     const array<vertex_data, 3> &vertexes, 
     const array<float, 3> &intensities,    
     const pair<int, int> &v1, 
     const pair<int, int> &v2, 
     const pair<int, int> &v3, 
     Texture &texture,                     // <-- Put it back here
     vector<float>& z_buffer, 
     int width, 
     int height, 
     Engine &engine
    ) ;

};