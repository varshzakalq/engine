#pragma once
#include <vector>
#include <array>
#include "texture.h"
using namespace std;
using Matrix4x4 = array<array<double, 4>, 4>;
class vertex_data;
class static_objects{
private:
    vector<vertex_data> vertex_buffer;
    vector <int>index_buffer ;
    Texture t;
    Matrix4x4 transform ={{
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}
    }};

public:
    void set_vertex(vector<vertex_data> vertex_buffer);
    void set_index( vector <int>index_buffer );
    void set_texture(string loaction);
    vector<vertex_data>  get_vertex();
    vector <int> get_index();
    Texture get_texture();
};

class dynamic_obj : public static_objects{
private:   
    Matrix4x4 transform ={{
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}
    }};
public:
    void update_transform(Matrix4x4 transformer);
    

};
