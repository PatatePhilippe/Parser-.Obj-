//
// Created by pat on 28/10/2026.
//
#ifndef OBJ_parser
#define OBJ_parser

typedef struct {
    unsigned int *verticules;
} triangle;

typedef struct {
    double coords[3];
} vertex;

extern triangle trianglesIndexs[400];
extern vertex vertexList[400];
extern int cptTriangles;  // ou size_t selon votre type actuel
extern int cptVertexs;    // ou size_t selon votre type actuel
int parser();

#endif //OBJ_parser
