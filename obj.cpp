#include "obj.h"

void static_objects::set_vertex(vector<vertex_data> buffer)
{
    vertex_buffer = buffer;
}

void static_objects::set_index(vector<int> id_buffer)
{
    index_buffer = id_buffer;
}

void static_objects::set_texture(string location)
{
    t.load_bmp(location);
}

vector<vertex_data> static_objects::get_vertex()
{
    return vertex_buffer;
}

vector <int> static_objects::get_index()
{
    return index_buffer;
}

Texture static_objects::get_texture()
{
    return t;
}



//this is dynamic object
void dynamic_obj::update_transform(Matrix4x4 transformer)
{
    transform = transformer;
}
