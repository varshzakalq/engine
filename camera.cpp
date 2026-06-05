#include <vector>
#include <array>
#include "camera.h"
#include <iostream>
using Vector4 = std::array<double, 4>;
using namespace std;

array<double, 4> clipper::clip(array<double, 4>v1,array<double, 4>v2){

    double t = (clipping_plane_z- v1[2])/(v2[2]-v1[2]);
    double x = v1[0] +(t*(v2[0]-v1[0]));
    double y = v1[1] +(t*(v2[1]-v1[1]));
    return {x,y,static_cast <double>(clipping_plane_z),v1[3]};
}

array<Vector4, 4> clipper::get_vertex(array<Vector4, 4>& vertex_buffer) {
    vertex_buffer[3][2] = -1.0; // default: no 4th vertex
        int clip_counter = 0;
        
        // Check how many vertices are out-of-bounds (behind the clipping plane)
        for(int i = 0; i < 3; i++) {
            if(vertex_buffer[i][2] < clipping_plane_z) {
                clip_counter++;
                
            }
            
        }
        //cout<<"ddd"<<clip_counter<<"fffff";
       
        if (clip_counter == 3) {
            vertex_buffer[0][3] = -999.0; // use w as a "discard" flag
            return vertex_buffer;
        }

    if(clip_counter == 1) {
        if(vertex_buffer[0][2] < clipping_plane_z) {
            Vector4 orig = vertex_buffer[0];
            Vector4 safe1 = vertex_buffer[1];
            Vector4 safe2 = vertex_buffer[2];
            vertex_buffer[0] = safe1;
            vertex_buffer[1] = safe2;
            vertex_buffer[2] = clip(orig, safe2); // newB: orig→safe2
            vertex_buffer[3] = clip(orig, safe1); // newA: orig→safe1
    
            //cout<<vertex_buffer[1][2]<<" "<<vertex_buffer[2][2]<< " "<<vertex_buffer[3][2]<<endl;
        }
        else if(vertex_buffer[1][2] < clipping_plane_z) {
            Vector4 orig  = vertex_buffer[1];
            Vector4 safe0 = vertex_buffer[0];
            Vector4 safe2 = vertex_buffer[2];
            vertex_buffer[1] = clip(orig, safe0); // newA: orig→safe0
            vertex_buffer[2] = clip(orig, safe2); // newB: orig→safe2
            vertex_buffer[3] = safe2;             // move safe2 to slot 3
           // cout<<vertex_buffer[1][2]<<" "<<vertex_buffer[2][2]<< " "<<vertex_buffer[3][2]<<endl;
         
        }
        else if(vertex_buffer[2][2] < clipping_plane_z) {
            Vector4 orig  = vertex_buffer[2];
            Vector4 safe0 = vertex_buffer[0];
            Vector4 safe1 = vertex_buffer[1];
            vertex_buffer[2] = clip(orig, safe1); // newA: orig→safe1
            vertex_buffer[3] = clip(orig, safe0); // newB: orig→safe0
            
           // cout<<vertex_buffer[1][2]<<" "<<vertex_buffer[2][2]<< " "<<vertex_buffer[3][2]<<endl;
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