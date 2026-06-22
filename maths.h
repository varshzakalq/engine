#pragma once
#include <iostream>
#include <array>
#include <vector>
using namespace std;
#include <cmath>
#include <cstdint>
#include <algorithm>

using Vector4 = std::array<double, 4>;
using Matrix4x4 = std::array<std::array<double, 4>, 4>;
struct Vector3 {
    double x, y, z;
    double& operator[](int index) { return (&x)[index]; }
    const double& operator[](int index) const { return (&x)[index]; }
};
inline Vector3 cross(Vector3 v1,Vector3 v2){
    return {
        v1.y*v2.z-v1.z*v2.y,
        -v1.x*v2.z+ v1.z*v2.x,
        v1.x*v2.y - v1.y*v2.x
    };
}
inline Vector4 transform(const Matrix4x4& M, const Vector4& v) {
    return {
        M[0][0]*v[0] + M[0][1]*v[1] + M[0][2]*v[2] + M[0][3]*v[3],
        M[1][0]*v[0] + M[1][1]*v[1] + M[1][2]*v[2] + M[1][3]*v[3],
        M[2][0]*v[0] + M[2][1]*v[1] + M[2][2]*v[2] + M[2][3]*v[3],
        M[3][0]*v[0] + M[3][1]*v[1] + M[3][2]*v[2] + M[3][3]*v[3]
    };
}
inline double magnitude(Vector3 v1){
    return sqrt(v1.z*v1.z + v1.x*v1.x + v1.y*v1.y);
}
inline double magnitude(Vector4 v1){
    return sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
}
inline float dist(Vector3 v1,Vector3 v2){
    return sqrt((v1[0]-v2[0])*(v1[0]-v2[0]) +(v1[1]-v2[1])*(v1[1]-v2[1]) + (v1[2] -v2[2])*(v1[2]-v2[2]));
}
inline float dist(Vector4 v1,Vector4 v2){
    return sqrt((v1[0]-v2[0])*(v1[0]-v2[0]) +(v1[1]-v2[1])*(v1[1]-v2[1]) + (v1[2] -v2[2])*(v1[2]-v2[2]));
}
inline float dist(Vector3 v1,Vector4 v2){
    return sqrt((v1[0]-v2[0])*(v1[0]-v2[0]) +(v1[1]-v2[1])*(v1[1]-v2[1]) + (v1[2] -v2[2])*(v1[2]-v2[2]));
}
inline double dot(Vector3 v1,Vector3 v2){
    return v1.x*v2.x +v1.y*v2.y + v1.z*v2.z;
}
inline Vector3 subtract(array<double,4>v1,array<double,4>v2){
    return {v1[0]-v2[0],v1[1]-v2[1],v1[2]-v2[2]};
}
inline Vector3 subtract(Vector3 v1,array<double,4>v2){
    return {v1[0]-v2[0],v1[1]-v2[1],v1[2]-v2[2]};
}
inline Vector3 subtract(Vector4 v1,Vector3 v2){
    return {v1[0]-v2[0],v1[1]-v2[1],v1[2]-v2[2]};
}
//extracts vector3 of rgb from uint32
inline Vector3 extract_rgb(uint32_t pixelColor){
    // 1. Shift and mask to match your working layout: (R << 16) | (G << 8) | B
    uint8_t r = (pixelColor >> 16) & 0xFF; // Red is at bits 16-23
    uint8_t g = (pixelColor >> 8)  & 0xFF; // Green is at bits 8-15
    uint8_t b =  pixelColor        & 0xFF; // Blue is at bits 0-7
    // 2. Convert from 0->255 to 0.0->1.0
    Vector3 rgb ;
    rgb.x = r / 255.0f;
    rgb.y = g / 255.0f;
    rgb.z = b / 255.0f;
    return rgb;
    
}
inline float clamp_val(float val, float min, float max) {
    return (val < min) ? min : (val > max) ? max : val;
}
inline uint32_t rgb_to_color(Vector3 rgb){
    // 1. Clamp and scale float (0.0f - 1.0f) to integer (0 - 255)
    uint32_t r = static_cast<uint32_t>(clamp_val(rgb.x, 0.0f, 1.0f) * 255.0f);
    uint32_t g = static_cast<uint32_t>(clamp_val(rgb.y, 0.0f, 1.0f) * 255.0f);
    uint32_t b = static_cast<uint32_t>(clamp_val(rgb.z, 0.0f, 1.0f) * 255.0f);
    

    // 2. Combine channels using bitwise OR and bit-shifts
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}
inline Vector3 Normalize(Vector3 v) {
    // 1. Calculate the length squared
    float length_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    
    // 2. Prevent division by zero for null vectors
    if (length_sq > 0.00001f) {
        // Calculate the actual length
        float length = std::sqrt(length_sq);
        
        Vector3 normalized;
        normalized.x = v.x / length;
        normalized.y = v.y / length;
        normalized.z = v.z / length;
        return normalized;
    }
    
    // Return a zero vector if normalization isn't possible
    return Vector3{ 0.0f, 0.0f, 0.0f };
}
inline void scale(Vector3 &v,float starting,float range) // scales and adjusts vectors starting from 0
{ 
    v.x =(v.x +starting )*range;
    v.y =(v.y +starting )*range;
    v.z =(v.z +starting )*range;
}