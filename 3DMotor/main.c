#include <signal.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "3DMotor.h"
#include<unistd.h>
#include <math.h>

#include "fileReader.h"

Vec3 lightVec={0.0F,0.0F,-1.0F};

void printCube(Cube c)
{
    for (int i = 0; i < 12; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            printf("(%f %f %f)\n",c.mesh[i].points[j].pos[0],
                c.mesh[i].points[j].pos[1],
                c.mesh[i].points[j].pos[2]);
        }
    }
}

void showTriangle(SDL_Renderer *rendu, Cube *c,Camera *cam,Mat4 *proj3D)
{
    for (int i = 0; i < 12; ++i)
    {
        //verification de mes normales
        Vec3 vecN,vecA,vecB;
        //line from O to 1
        vecA.pos[0]=c->mesh[i].points[1].pos[0]-c->mesh[i].points[0].pos[0];
        vecA.pos[1]=c->mesh[i].points[1].pos[1]-c->mesh[i].points[0].pos[1];
        vecA.pos[2]=c->mesh[i].points[1].pos[2]-c->mesh[i].points[0].pos[2];

        //line from O to 2
        vecB.pos[0]=c->mesh[i].points[2].pos[0]-c->mesh[i].points[0].pos[0];
        vecB.pos[1]=c->mesh[i].points[2].pos[1]-c->mesh[i].points[0].pos[1];
        vecB.pos[2]=c->mesh[i].points[2].pos[2]-c->mesh[i].points[0].pos[2];
        //vecteur normalisé perpendiculaire au plan du vertex
        vecN.pos[0]=vecA.pos[1]*vecB.pos[2]-vecA.pos[2]*vecB.pos[1];
        vecN.pos[1]=vecA.pos[2]*vecB.pos[0]-vecA.pos[0]*vecB.pos[2];
        vecN.pos[2]=vecA.pos[0]*vecB.pos[1]-vecA.pos[1]*vecB.pos[0];

        // printf("Triangle %d\n", i);

        // for (int j = 0; j < 3; ++j)
        // {
        //     printf("P%d = (%f, %f, %f)\n",
        //         j,
        //         c->mesh[i].points[j].pos[0],
        //         c->mesh[i].points[j].pos[1],
        //         c->mesh[i].points[j].pos[2]);
        // }

        double l=(vecN.pos[0]*vecN.pos[0]+
            vecN.pos[1]*vecN.pos[1]+
            vecN.pos[2]*vecN.pos[2]);
        double res =sqrt(l);
        vecN.pos[0]/=res;
        vecN.pos[1]/=res;
        vecN.pos[2]/=res;


        double facing =
            vecN.pos[0] * (c->mesh[i].points[2].pos[0] - cam->x) +
            vecN.pos[1] * (c->mesh[i].points[2].pos[1] - cam->y) +
            vecN.pos[2] * (c->mesh[i].points[2].pos[2] - cam->z);

        // printf("Triangle %d: N=(%.3f, %.3f, %.3f), facing=%.6f\n",
        //        i,
        //        vecN.pos[0], vecN.pos[1], vecN.pos[2],
        //        facing);

        if ((vecN.pos[0]*(c->mesh[i].points[2].pos[0]-cam->x))+
            (vecN.pos[1]*(c->mesh[i].points[2].pos[1]-cam->y))+
            (vecN.pos[2]*(c->mesh[i].points[2].pos[2]-cam->z))<-0.35F
            )
        {
            //printf("res%d %f",i,(vecN.pos[0]*(c->mesh[i].points[2].pos[0]-cam->x))+
    //vecN.pos[1]*(c->mesh[i].points[2].pos[1]-cam->y))+
    //(vecN.pos[2]*(c->mesh[i].points[2].pos[2]-cam->z)));
            //printf("%f,%f,%f \n", vecN.pos[0],vecN.pos[1],vecN.pos[2]);
            double dotProduct=(vecN.pos[0]*lightVec.pos[0]+
            vecN.pos[1]*lightVec.pos[1]+
            vecN.pos[2]*lightVec.pos[2]);

            //printf("dot : %f\n",dotProduct);

            //affichage de mes vertexs
            Triangle t=triangle_final(&c->mesh[i],proj3D);
            //
            // for (int j = 0; j < 3; ++j)
            // {
            //     printf("P%d = (%f, %f, %f)\n",
            //         j,
            //         (t.points[j].pos[0]+0.5)*600,
            //          (0.5-t.points[j].pos[1])*400,
            //          (t.points[j].pos[2]));
            // }
            SDL_Vertex test[3];
            test[0].position.x = (t.points[0].pos[0]+0.5)*600;
            test[0].position.y = (0.5-t.points[0].pos[1])*400;
            test[0].color.r = dotProduct;
            test[0].color.g = dotProduct;
            test[0].color.b = dotProduct;
            test[0].color.a = 1.0;

            // left
            test[1].position.x = (t.points[1].pos[0]+0.5)*600;
            test[1].position.y = (0.5-t.points[1].pos[1])*400;
            test[1].color.r = dotProduct;
            test[1].color.g = dotProduct;
            test[1].color.b = dotProduct;
            test[1].color.a = 1.0;

            // right
            test[2].position.x = (t.points[2].pos[0]+0.5)*600;
            test[2].position.y = (0.5-t.points[2].pos[1])*400;
            test[2].color.r = dotProduct;
            test[2].color.g = dotProduct;
            test[2].color.b = dotProduct;
            test[2].color.a = 1.0;
            SDL_RenderGeometry(rendu, NULL, test, 3, NULL, 0);
        }
    }
}


Cube calculCube(Cube c,Camera *cam,Mat4 *proj3D,SDL_Renderer *rendu)
{   //printf("passe pour i,j : %d %d\n",4,2);

    //cam_turn( cam, 'y', 0.03);
    //cam_turn( cam, 'x', 0.03);
    //cam_turn( cam, 'z', 0.03);

    Mat3 mat_y = mat3_rot_y(cam->angleY);
    Mat3 mat_x = mat3_rot_x(cam->angleX);
    Mat3 mat_z = mat3_rot_z(cam->angleZ);


    Cube projectionC = cube_transform(c,mat_y);
    projectionC = cube_transform(projectionC,mat_z);
    projectionC = cube_transform(projectionC,mat_x);
    cube_move(&projectionC ,3,2);
    showTriangle(rendu,&projectionC,cam,proj3D);


    return projectionC;
}
//TODO : cam in global variable ?
void sdlEventCommands(Camera *cam,SDL_Event *e,int *isOpen)
{
    if (e->type == SDL_EVENT_QUIT) {
        *isOpen=0;
    }
    if (e->type == SDL_EVENT_KEY_DOWN) {
        switch (e->key.key){
        case SDLK_ESCAPE:
            *isOpen=0;
            break;
            //camera rotation
        case SDLK_Q:
            cam_turn( cam, 'y', 0.03);
            //projC2=calculCube(c2,&cam,proj3D);
            break;
        case SDLK_D:
            cam_turn( cam, 'y', -0.03);
            break;
        case SDLK_Z:
            cam_turn( cam, 'x', 0.03);
            break;
        case SDLK_S:
            cam_turn( cam, 'x', -0.03);
            break;
            //TODO : camera movements

        // case SDLK_DOWN:
        //     cam_move( cam, 'f', 1);
        //
        //     break;
        // case SDLK_UP:
        //     cam_turn( cam, 'y', -0.03);
        //     break;
        // case SDLK_LEFT:
        //     cam_turn( cam, 'x', 0.03);
        //     break;
        // case SDLK_RIGHT:
        //     cam_turn( cam, 'x', -0.03);
        //     break;

        default:
            break;
        }
    }
}



int main(int argc, char *argv[]) {
    //init screen
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *fenetre = SDL_CreateWindow("Ma fenêtre SDL",
                                           600, 400,SDL_WINDOW_RESIZABLE);

    if (!fenetre) {
        printf("Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *rendu = SDL_CreateRenderer(fenetre, "openGL");
    if (!rendu) {
        printf("Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return 1;

    }
    int isOpen = 1;
    SDL_Event e;
    SDL_SetRenderLogicalPresentation(rendu,600,400,SDL_LOGICAL_PRESENTATION_STRETCH);

    //3D object init
    Cube c,c2,c3;
    Mat4 proj3D=projectionMatrix();
    Camera cam = {0,0,0,0,0,0};
    c = cube_make(1);
    c2 = cube_make(1);
    c3 = cube_make(1);
    cube_move(&c2 ,1,0);
    cube_move(&c3 ,1,2);
    Cube projC=calculCube(c,&cam,&proj3D,rendu);
    //Cube projC2=calculCube(c2,&cam,proj3D);
    //Cube projC3=calculCube(c3,&cam,proj3D);

    //light vec init
    double val=lightVec.pos[0]*lightVec.pos[0]+
                        lightVec.pos[1]*lightVec.pos[1]+
                        lightVec.pos[2]*lightVec.pos[2];
    const double res2 =sqrt(val);
    lightVec.pos[0]/=res2;
    lightVec.pos[1]/=res2;
    lightVec.pos[2]/=res2;

    while (isOpen!=0)
    {
        //treat sdl events (windows interactions and keyboard for now)
        while (SDL_PollEvent(&e)) {
            sdlEventCommands(&cam,&e,&isOpen);
        }
        //usleep(30000);
        //printf("TEST2\n");
        SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
        SDL_RenderClear(rendu);
        SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255);

        //TODO : show the vertexs in parallelism
        //showTriangle(rendu,&projC2);
        //showTriangle(rendu,&projC3);
        calculCube(c,&cam,&proj3D,rendu);
        SDL_RenderPresent(rendu);
        //projC2=calculCube(c2,&cam,proj3D);
        //projC3=calculCube(c3,&cam,proj3D);
        usleep(50000);


    }
    // 5. Libération de la mémoire et fermeture
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return 0;
}
