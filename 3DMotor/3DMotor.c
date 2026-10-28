//
// Created by alexis on 12/07/2026.
//

#include "3DMotor.h"
#include <math.h>
#include <stdio.h>
const int fovX = 50;
const double fovY = 50.0;
const double zFar = 10;
const double zNear = 1;
const double screenW=600;
const double screenH=400;
const float fovXRad = fovX * (3.14159265359F / 180.0F);

Mat3 mat3_rot_x(double a) {
    Mat3 m = {{
        {1,0,0},
        {0,cos(a),-sin(a)},
        {0,sin(a),cos(a)}}
};
    return m;
}
Mat3 mat3_rot_y(double a) {
    Mat3 m = {{{cos(a), 0, sin(a)},
               { 0,      1, 0     },
               {-sin(a), 0, cos(a)}}};

    return m;
}
Mat3 mat3_rot_z(double a) {
    Mat3 m = {{{cos(a),-sin(a), 0,},
               { sin(a),cos(a), 0 },
               {0, 0, 1}}};

    return m;
}

Mat4 projectionMatrix(){
    Mat4 m = {{{(1.0F/(tanf(fovXRad/2.0F)))*(screenH/screenW), 0,0,0 },
         { 0,      1.0F/(tanf(fovXRad/2.0F)), 0,0     },
            {0,0,(zFar/(zFar-zNear)), 1},
         {0, 0, -((zFar*zNear)/zFar-zNear),0}}};
    return m;
}

Vec4 mat4_mul(Mat4 *m, Vec4 v) {
    Vec4 out;
    for (int row = 0; row < 4; row++) {
        double cpt = 0;
        for (int col = 0; col < 4; col++)
            cpt += m->m[col][row] * v.pos[col];
        out.pos[row] = cpt;
    }
    return out;
}


Vec3 mat3_mul(const Mat3 m, Vec3 v) {
    Vec3 out;
    for (int col = 0; col < 3; col++) {
        double cpt = 0;
        for (int row = 0; row < 3; row++)
            cpt += m.m[col][row] * v.pos[row];
        out.pos[col] = cpt;
    }
    return out;
}
Triangle triangle_final(Triangle *t,Mat4 *projection3D) { //TODO : change name to something like vertexVisual projection
    Triangle out;
    for(int j =0 ; j < 3; j++)
    {
        Vec4 inputV={t->points[j].pos[0],
                    t->points[j].pos[1],
                    t->points[j].pos[2],
                    1};
        Vec4 nVec=mat4_mul(projection3D,inputV);
        out.points[j].pos[0]=nVec.pos[0]/nVec.pos[2];
        out.points[j].pos[1]=nVec.pos[1]/nVec.pos[2];
        out.points[j].pos[2]=nVec.pos[2];
        if (nVec.pos[3]!=0.0F)
        {
            out.points[j].pos[0]/=nVec.pos[3];
            out.points[j].pos[1]/=nVec.pos[3];
            out.points[j].pos[2]/=nVec.pos[3];
        }
    }
    return out;

}

Cube cube_make(double size) {
    double h = size / 2.0;
    Cube c = {{
        //point(x y z)
        //south
        {{{ -h,-h, -h},{ -h,h, -h},{ h,h, -h}}},
        {{{ -h,-h, -h},{ h,h, -h},{ h,-h, -h}}},
        //top
        {{{ -h,h, -h},{ -h,h, h},{ h,h, h}}},
        {{{ -h,h, -h},{ h,h, h},{ h,h, -h}}},
        //east
        {{{ h,-h, -h},{ h,h, -h},{ h,h, h}}},
        {{{ h,-h, -h},{ h,h, h},{ h,-h, h}}},
        //west
        {{{ -h,-h, h},{ -h,h, h},{ -h,h, -h}}},
        {{{ -h,-h, h},{ -h,h, -h},{ -h,-h, -h}}},
        //bottom
        {{{ h,-h, h},{ -h,-h, h},{ -h,-h, -h}}},
        {{{ h,-h, h},{ -h,-h, -h},{ h,-h, -h}}},
        //north
        {{{ h,-h, h},{ h,h, h},{ -h,h, h}}},
        {{{ h,-h, h},{ -h,h, h},{ -h,-h, h}}},
    }};
    return c;
}

Cube cube_transform(Cube c,Mat3 matProjection) {//TODO : more explicit name
    Cube m = {{}};
    Triangle t={};
    for(int i =0 ; i < 12; i++) {
        for(int j =0 ; j < 3; j++) {
            t.points[j]=mat3_mul(matProjection,c.mesh[i].points[j]);
        }
        m.mesh[i] = t;
    }
    return m;

}


void cam_move(Camera *cam,char dir,int amount) {
    if (dir == 'f') {
        cam->x+=amount;
    }
    if (dir == 's') {
        cam->y+=amount;
    }
}
void cube_move(Cube *c ,double dist,int indexOfAxe) {
    for (int i = 0; i < 12; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            c->mesh[i].points[j].pos[indexOfAxe] += dist;
        }
    }
}

void cam_turn(Camera *cam, char dir, double amount) {
    if (dir == 'y') {
        cam->angleY += amount;
        if (cam->angleY > 6.28) cam->angleY -= 6.28;
        if (cam->angleY < 0)    cam->angleY += 6.28;
    }
    if (dir == 'x') {
        cam->angleX += amount;
        if (cam->angleX > 6.28) cam->angleX -= 6.28;
        if (cam->angleX < 0)    cam->angleX += 6.28;
    }
    if (dir == 'z') {
        cam->angleZ += amount;
        if (cam->angleZ > 6.28) cam->angleZ -= 6.28;
        if (cam->angleZ < 0)    cam->angleZ += 6.28;
    }
}