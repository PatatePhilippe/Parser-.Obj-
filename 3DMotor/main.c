#include <signal.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "3DMotor.h"
#include<unistd.h>
#include <math.h>
#include <math.h>

#include "fileReader.h"
#include "../parser/OBJ_parser.h"


long lightVec[3]={0.0,0.0,-1.0};

void visual_proj_obj(SDL_Renderer *rendu,Triangle *polys,Camera *cam,Mat4 *proj3D,size_t size)
{
    for (int i = 0; i < size; ++i)
    {
        //verification de mes normales
        Vec3 vecN,vecA,vecB;
        for (int j = 0; j < 3; ++j)
        {
            vecA.pos[j]=polys[i].points[1].pos[j]-polys[i].points[0].pos[j];
            vecB.pos[j]=polys[i].points[2].pos[j]-polys[i].points[0].pos[j];
        }
        //vecteur normalisé perpendiculaire au plan du vertex
        vecN.pos[0]=vecA.pos[1]*vecB.pos[2]-vecA.pos[2]*vecB.pos[1];
        vecN.pos[1]=vecA.pos[2]*vecB.pos[0]-vecA.pos[0]*vecB.pos[2];
        vecN.pos[2]=vecA.pos[0]*vecB.pos[1]-vecA.pos[1]*vecB.pos[0];

        double l=(vecN.pos[0]*vecN.pos[0]+
            vecN.pos[1]*vecN.pos[1]+
            vecN.pos[2]*vecN.pos[2]);
        double res =sqrt(l);
        vecN.pos[0]/=res;
        vecN.pos[1]/=res;
        vecN.pos[2]/=res;

        double facing =
            vecN.pos[0] * (polys[i].points[2].pos[0] - cam->x) +
            vecN.pos[1] * (polys[i].points[2].pos[1] - cam->y) +
            vecN.pos[2] * (polys[i].points[2].pos[2] - cam->z);

        if ((vecN.pos[0] * (polys[i].points[2].pos[0]-cam->x))+
            (vecN.pos[1] * (polys[i].points[2].pos[1]-cam->y))+
            (vecN.pos[2] * (polys[i].points[2].pos[2]-cam->z))<-0.35F)
        {
            double dotProduct=(vecN.pos[0]*lightVec[0]+
            vecN.pos[1]*lightVec[1]+
            vecN.pos[2]*lightVec[2]);

            //showing my vertex
            Triangle t=triangle_final(&polys[i],proj3D);
            SDL_Vertex test[3];
            for (int i = 0; i < 3; ++i)
            {
                test[i].position.x = (t.points[i].pos[0]+0.5)*600;
                test[i].position.y = (0.5-t.points[i].pos[1])*400;
                test[i].color.r = dotProduct;
                test[i].color.g = dotProduct;
                test[i].color.b = dotProduct;
                test[i].color.a = 1.0;
            }
            SDL_RenderGeometry(rendu, NULL, test, 3, NULL, 0);
        }
    }
}


void all_proj_obj(Triangle *poly_input,Triangle *poly_output,Camera *cam,Mat4 *proj3D,SDL_Renderer *rendu,long unsigned int size){
    Triangle poly_input_copy[size];
    memcpy(poly_input_copy, poly_input, size * sizeof(Triangle));

    Mat3 mat_y = mat3_rot_y(cam->angleY);
    Mat3 mat_x = mat3_rot_x(cam->angleX);
    Mat3 mat_z = mat3_rot_z(cam->angleZ);

    rotate_obj(poly_input_copy, poly_output, mat_x, size);
    memcpy(poly_input_copy, poly_output, size * sizeof(Triangle));
    rotate_obj(poly_input_copy,            poly_output, mat_y, size);
    memcpy(poly_input_copy, poly_output, size * sizeof(Triangle));
    rotate_obj(poly_input_copy,            poly_output, mat_z, size);
    memcpy(poly_input_copy, poly_output, size * sizeof(Triangle));//TODO : make it cleaner it use a lot of memory
    move_obj(poly_output,10,2,size);//temporary move to see the object clearly
    move_obj(poly_output,-5,1,size);//temporary move to see the object clearly
    visual_proj_obj(rendu,poly_output,cam,proj3D,size);
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
            turn_cam( cam, 'y', 0.03);
            //projC2=calculCube(c2,&cam,proj3D);
            break;
        case SDLK_D:
            turn_cam( cam, 'y', -0.03);
            break;
        case SDLK_Z:
            turn_cam( cam, 'x', 0.03);
            break;
        case SDLK_S:
            turn_cam( cam, 'x', -0.03);
            break;
            
        //TODO : camera moving forward, bakcward ...
        // case SDLK_DOWN:
        //     cam_move( cam, 'f', 1);
        //
        //     break;
        // case SDLK_UP:
        //     turn_cam( cam, 'y', -0.03);
        //     break;
        // case SDLK_LEFT:
        //     turn_cam( cam, 'x', 0.03);
        //     break;
        // case SDLK_RIGHT:
        //     turn_cam( cam, 'x', -0.03);
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
                                           900, 600,SDL_WINDOW_RESIZABLE);

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
    Mat4 proj3D=projectionMatrix();
    Camera cam = {0,0,0,0,0,0};

    //creation of the triangles

    char* OBJfile = read3dFile();
    if (parser(OBJfile)==1)return 1;
    printf("triangle vertexsize %d %d \n",cptTriangles,cptVertexs);
    Triangle polys_origin[cptTriangles];
    Triangle polys_transformed[cptTriangles];
    unsigned int coordSize=3*sizeof(double);
    for (int i = 0; i < cptTriangles; ++i){
        for (int y = 0; y < 3; ++y){
            printf("%lu \n",sizeof(trianglesIndexs[i].verticules[y]));

            unsigned int targetVertex = trianglesIndexs[i].verticules[y];
            printf("i:%d, y:%d -> Demande le sommet index : %u (Max autorisé : %d)\n", i, y, targetVertex, cptVertexs - 1);
            if (targetVertex >= (unsigned int)cptVertexs) {
                fprintf(stderr, "CRITIQUE : L'index de triangle %d pointe vers le sommet %u qui n'existe pas !\n", i, targetVertex);
                return 1;
            }
            memcpy(&polys_origin[i].points[y].pos,
                   &vertexList[targetVertex].coords,
                   coordSize);

        }
    }

    printf("test %f %f %f \n",polys_origin[0].points[0].pos[0],polys_origin[0].points[0].pos[1],polys_origin[0].points[0].pos[2]);
    all_proj_obj(polys_origin,polys_transformed,&cam,&proj3D,rendu,cptTriangles);
    
    //light vec init
    long val=lightVec[0]*lightVec[0]+
                        lightVec[1]*lightVec[1]+
                        lightVec[2]*lightVec[2];
    const long res2 =(long) sqrtl(val);
    lightVec[0]/=res2;
    lightVec[1]/=res2;
    lightVec[2]/=res2;

    while (isOpen!=0)
    {
        //treat sdl events (windows interactions and keyboard for now)
        while (SDL_PollEvent(&e)) {
            sdlEventCommands(&cam,&e,&isOpen);
        }
        
        SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
        SDL_RenderClear(rendu);
        SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255);
        
        all_proj_obj(polys_origin,polys_transformed,&cam,&proj3D,rendu,cptTriangles);
        SDL_RenderPresent(rendu);
        usleep(50000);
    }
    // 5. Libération de la mémoire et fermeture
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return 0;
}
