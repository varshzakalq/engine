#pragma once
#include <iostream>
#include <array>
#include <vector>
using namespace std;
#include <cmath>

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
