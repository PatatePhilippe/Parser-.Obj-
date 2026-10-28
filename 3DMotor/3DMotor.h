//
// Created by alexis on 12/07/2026.
//

#ifndef MOTEUR_GRAPHIQUE_H
#define MOTEUR_GRAPHIQUE

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


typedef struct {
    Triangle mesh[12];
    Vec3 coord;
} Cube;

typedef struct {
    double angleX, angleY, angleZ;
    int x, y, z;
} Camera;



Mat3  mat3_rot_y(double angle);
Mat3  mat3_rot_x(double angle);
Mat3  mat3_rot_z(double angle);

Vec3  mat3_mul(const Mat3 m, Vec3 v);
Cube  cube_make(double size);          // crée un cube centré sur 0
Cube  cube_transform(const Cube c, const Mat3 matProjection);
void  cam_move(Camera *cam, char dir,int amount);
void  cam_turn(Camera *cam, char dir, double amount);
Vec3  finalOperation(Vec3 v);
Triangle triangle_final(Triangle *t,Mat4 *projection3D);
void cube_move(Cube *c ,double dist,int indexOfAxe);
Mat4 projectionMatrix();






#endif //MOTEUR_GRAPHIQUE_H