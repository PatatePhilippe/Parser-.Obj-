//
// Created by alexis on 12/07/2026.
//

#ifndef MOTEUR_GRAPHIQUE_H
#define MOTEUR_GRAPHIQUE
#include <stddef.h>

typedef struct {
    double pos[3];
} Vec3;

typedef struct {
    double pos[4];
} Vec4;

typedef struct {
    double m[3][3];
} Mat3;

typedef struct {
    double m[4][4];
} Mat4;


//triangles representing the face
//clock wise
typedef struct {
    Vec3 points[3];
} Triangle;

//TODO: replace this part by an object struct to have separate coords later on
// typedef struct {
//     Triangle mesh[12];
//     Vec3 coord;
// } Cube;

typedef struct {
    double angleX, angleY, angleZ;
    int x, y, z;
} Camera;



Mat3  mat3_rot_y(double angle);
Mat3  mat3_rot_x(double angle);
Mat3  mat3_rot_z(double angle);

Vec3  mat3_mul(Mat3 m, Vec3 v);
//Cube  cube_make(double size);
void  rotate_obj(Triangle poly_input[],Triangle *poly_output[],Mat3 matProjection,size_t size);
void  move_cam(Camera *cam, char dir,int amount);
void  turn_cam(Camera *cam, char dir, double amount);
Vec3  finalOperation(Vec3 v);//TODO : change name
Triangle triangle_final(Triangle *t,Mat4 *projection3D);
void move_obj(Triangle *poly_input[] ,double dist,int indexOfAxe,size_t size);
Mat4 projectionMatrix();






#endif //MOTEUR_GRAPHIQUE_H