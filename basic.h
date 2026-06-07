#pragma once

#include <utility>
#include <cmath>
#include<algorithm>
using namespace std;
class Engine;
class line{
    Engine &e;
public:
    line(Engine &eng);
    void draw(pair<int,int>v1,pair<int,int>v2,Engine &e);
};
class project{
int size = 2500;
public:
     pair <int,int> onscreen(double x1, double y1, double z1, float focal_lenght);
    void fill_color(pair<int,int>v1,pair<int,int>v2,pair<int,int>v3,uint32_t &color,int screen_widht,int screen_height,Engine &e);

};