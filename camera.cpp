#include <vector>
#include <array>
#include "camera.h"
using Vector4 = std::array<double, 4>;
using namespace std;

array<double, 4> clipper::clip(array<double, 4>v1,array<double, 4>v2){

    double t = (clipping_plane_z- v1[2])/(v1[2]-v2[2]);
    double x = v1[0] +(t*(v2[0]-v1[0]));
    double y = v1[1] +(t*(v2[1]-v1[1]));
    return {x,y,static_cast <double>(clipping_plane_z),v1[3]};
}

array<Vector4, 4> clipper::get_vertex(array<Vector4, 4>& vertex_buffer) {
    int clip_counter = 0;

    // Check how many vertices are out-of-bounds (behind the clipping plane)
    for(int i = 0; i < 3; i++) {
        if(vertex_buffer[i][2] < clipping_plane_z) {
            clip_counter++;
        }
    }

    // CASE 0: Entire triangle is behind the plane -> completely discard it
    if (clip_counter == 3) {
        vertex_buffer[3][2] = -1.0; // Keep our flag set to unclipped
        return vertex_buffer; 
    }

    // CASE 1: One vertex is clipped (Triangle becomes a Quad)
    if(clip_counter == 1) {
        if(vertex_buffer[0][2] < clipping_plane_z) {
            // Save original v0 to local space before overwriting it!
            Vector4 original_v0 = vertex_buffer[0];
            
            vertex_buffer[0] = clip(original_v0, vertex_buffer[1]);
            vertex_buffer[3] = clip(original_v0, vertex_buffer[2]);
            clipped_vertex = {vertex_buffer[0], vertex_buffer[1]};
        }
        else if(vertex_buffer[1][2] < clipping_plane_z) {
            Vector4 original_v1 = vertex_buffer[1];
            
            vertex_buffer[1] = clip(original_v1, vertex_buffer[0]);
            vertex_buffer[3] = clip(original_v1, vertex_buffer[2]);
            clipped_vertex = {vertex_buffer[1], vertex_buffer[2]};
        }
        else if(vertex_buffer[2][2] < clipping_plane_z) {
            Vector4 original_v2 = vertex_buffer[2];
            
            vertex_buffer[2] = clip(original_v2, vertex_buffer[1]);
            vertex_buffer[3] = clip(original_v2, vertex_buffer[0]);
            clipped_vertex = {vertex_buffer[2], vertex_buffer[0]};
        }
    }

    // CASE 2: Two vertices are clipped (Triangle remains a smaller Triangle)
    else if(clip_counter == 2) {
        if(vertex_buffer[0][2] >= clipping_plane_z) { // v0 is the only safe vertex
            vertex_buffer[1] = clip(vertex_buffer[1], vertex_buffer[0]);
            vertex_buffer[2] = clip(vertex_buffer[2], vertex_buffer[0]);
        }
        else if(vertex_buffer[1][2] >= clipping_plane_z) { // v1 is the only safe vertex
            vertex_buffer[0] = clip(vertex_buffer[0], vertex_buffer[1]);
            vertex_buffer[2] = clip(vertex_buffer[2], vertex_buffer[1]);
        }
        else if(vertex_buffer[2][2] >= clipping_plane_z) { // v2 is the only safe vertex
            vertex_buffer[0] = clip(vertex_buffer[0], vertex_buffer[2]);
            vertex_buffer[1] = clip(vertex_buffer[1], vertex_buffer[2]);
        }
    }

    return vertex_buffer;
}