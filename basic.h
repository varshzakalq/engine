#pragma once

#include <utility>
#include <cmath>
#include<algorithm>

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
float margin_of_error = 0.01f; //this reduces small smaall lines in the mdel presentation
public:
   
     pair <int,int> onscreen(double x1, double y1, double z1, float focal_lenght);
    void fill_color(array<float,3>z_values,array<array<float,2>,3> uv, Texture &texture,float light_intensity,pair<int,int>v1,pair<int,int>v2,pair<int,int>v3,vector <float>&z_buffer,uint32_t &color,int screen_widht,int screen_height,Engine &e);

};