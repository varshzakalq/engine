#pragma once

#include <utility>

using namespace std;
class Engine;
class line{
    Engine &e;
public:
    line(Engine &eng);
    void draw(pair<int,int>v1,pair<int,int>v2,Engine &e);
};
class project{

public:
     pair <int,int> onscreen(double x1, double y1, double z1, float focal_lenght);

};