#define ALEXER_IMPLEMENTATION
#include <stdio.h>
#include <alexer.h>
#include <unistd.h>

#include <errno.h>
#include "OBJ_parser.h"

typedef enum {
    PUNCT_POINT,
    PUNCT_MINUS,
    PUNCT_DOUBLE_SLASH,
    COUNT_PUNCTS,
} Punct_Index;

static_assert(COUNT_PUNCTS == 3, "Amount of puncts have changed");
const char *puncts[COUNT_PUNCTS] = {
    [PUNCT_POINT]           = ".",
    [PUNCT_MINUS]           = "-",
    [PUNCT_DOUBLE_SLASH]    = "//",
};

typedef enum {
    KEYWORD_POINT,
    KEYWORD_NORMAL,
    KEYWORD_VERTEX,
    KEYWORD_S,
    KEYWORD_MTL,
    KEYWORD_G,
    KEYWORD_USE,
    COUNT_KEYWORDS,
} Keyword_Index;

static_assert(COUNT_KEYWORDS == 7, "Amount of keywords have changed");
const char *keywords[COUNT_KEYWORDS] ={
    [KEYWORD_POINT]      = "v",
    [KEYWORD_NORMAL]     = "vn",
    [KEYWORD_VERTEX]     = "f",
    [KEYWORD_G]          = "g",
    [KEYWORD_S]          = "s",
    [KEYWORD_MTL]        = "mtllib",
    [KEYWORD_USE]        = "usemtl",
};


const char *sl_comments[] = {
    "#"
};



typedef struct {//TODO : use that later to extract data
    Alexer *lexer;
    Alexer_Token token;
    vertex *vertices;
    size_t vertex_count;
    size_t vertex_capacity;
} Parser;

uint64_t start_tokens[]={ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_MTL),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_G),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_NORMAL),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_POINT),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_S),
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_USE),
                        ALEXER_END};
uint64_t start_double[]={ALEXER_ID(ALEXER_PUNCT,PUNCT_MINUS),
                        ALEXER_INT};


triangle trianglesIndexs[400] = {0};
vertex vertexList[400] = {0};
int cptTriangles = 0;
int cptVertexs = 0;

bool double_parser(double *resultPointer,Alexer_Token *t,Alexer *l){//TODO : change variables names
    bool negation=false;
    if (t->id==ALEXER_ID(ALEXER_PUNCT,PUNCT_MINUS)){
        alexer_get_token(l, t );
        negation=true;
    }
    if (!(alexer_expect_id(l,*t, ALEXER_INT))) return false;

    char input2[t->end - t->begin];
    memcpy(&input2,t->begin,(t->end - t->begin)*sizeof(char));
    alexer_get_token(l, t );

    if (!alexer_expect_id(l,*t,ALEXER_ID(ALEXER_PUNCT,PUNCT_POINT))) return false ;
    alexer_get_token(l, t );

    if (!alexer_expect_id(l,*t,ALEXER_INT)) return false;
    char res[t->end - t->begin];
    memcpy(&res,t->begin,(t->end - t->begin)*sizeof(char));

    char str[32];
    snprintf(str, sizeof(str), "%s.%s", input2,res);
    *resultPointer = strtod(str, NULL);
    if (negation)
    {
        *resultPointer*=-1;
    }
    return true;
}

bool polygon_parser(Alexer_Token *t,Alexer *l){

    trianglesIndexs[cptTriangles].verticules=malloc(3*sizeof(unsigned int));
    unsigned int tempPos[4];
    for (int index = 0; index < 3; ++index){

        if (!(alexer_expect_id(l,*t, ALEXER_INT)))return false;
        char p1[t->end - t->begin];
        memcpy(&p1,t->begin,(t->end - t->begin)*sizeof(char));
        tempPos[index]=(int)strtol(p1, NULL, 10);
        alexer_get_token(l, t );

        if (!alexer_expect_id(l,*t,ALEXER_ID(ALEXER_PUNCT,PUNCT_DOUBLE_SLASH)))return false;
        alexer_get_token(l, t );
        if (!(alexer_expect_id(l,*t, ALEXER_INT))) return false;
        alexer_get_token(l, t );
    }
    if (t->id==ALEXER_INT){

        if (!(alexer_expect_id(l,*t, ALEXER_INT))) return false;
        char p1[t->end - t->begin];
        memcpy(&p1,t->begin,(t->end - t->begin)*sizeof(char));
        tempPos[3]=(int)strtol(p1, NULL, 10);
        alexer_get_token(l, t );
        if (!alexer_expect_id(l,*t,ALEXER_ID(ALEXER_PUNCT,PUNCT_DOUBLE_SLASH))) return false;
        alexer_get_token(l, t );
        if (!(alexer_expect_id(l,*t, ALEXER_INT))) return false;

        memcpy(&trianglesIndexs[cptTriangles].verticules[0],&tempPos[0],3*sizeof(unsigned int));
        cptTriangles++;
        trianglesIndexs[cptTriangles].verticules=malloc(3*sizeof(unsigned int));
        trianglesIndexs[cptTriangles].verticules[0]=tempPos[2];
        trianglesIndexs[cptTriangles].verticules[1]=tempPos[3];
        trianglesIndexs[cptTriangles].verticules[2]=tempPos[0];
    }else{
        if(!alexer_expect_one_of_ids(l,*t,start_tokens,9))return false;
        memcpy(&trianglesIndexs[cptTriangles].verticules[0],&tempPos[0],3*sizeof(unsigned int));
    }
    cptTriangles++;
    alexer_get_token(l, t );
    return true;
}

//TODO : change name to it's usage later
bool g_parser(Alexer_Token *t,Alexer *l){//TODO : the result is verified but not used for now
    if (!alexer_expect_id(l,*t,ALEXER_SYMBOL)) return false;
    alexer_get_token(l, t );
    if (!alexer_expect_id(l,*t,ALEXER_ID(ALEXER_PUNCT,PUNCT_POINT))) return false;
    alexer_get_token(l, t );
    if (!alexer_expect_id(l,*t, ALEXER_INT)) return false;
    alexer_get_token(l, t );
    return true;
}//TODO : the result is verified but not used for now

bool material_file_parser(Alexer_Token *t,Alexer *l){
    if (!alexer_expect_id(l,*t,ALEXER_SYMBOL)) return false;
    alexer_get_token(l, t );
    if (!alexer_expect_id(l,*t,ALEXER_ID(ALEXER_PUNCT,PUNCT_POINT))) return false;
    alexer_get_token(l, t );
    if (!alexer_expect_id(l,*t, ALEXER_SYMBOL)) return false;
    if (access("../Goldfish_01.mtl", F_OK) != 0) {
        puts( strerror( errno ) );
        //return false;//TODO : change the path part later
    }
    alexer_get_token(l, t );
    return true;
}//TODO : the result is verified but not used for now

bool material_usage_parser(Alexer_Token *t,Alexer *l){
    alexer_get_token(l, t );
    return true;
}//TODO : the result is not verified and not used for now

bool vertex_parser(Alexer_Token *t,Alexer *l){
    for (int i = 0; i < 3; ++i)
    {
        if (!double_parser(&vertexList[cptVertexs].coords[i],t,l))return false;
        printf("%f \n",vertexList[cptVertexs].coords[i]);
        alexer_get_token(l, t );
    }
    cptVertexs+=1;
    return true;
    //printf(" doubles : %f %f %f",x,y,z);
}

//TODO : change name to it's usage later
bool s_parser(Alexer_Token *t,Alexer *l){
    //l->diagf(t->loc, "ERROR verif", "%s: %.*s \n", alexer_kind_name(ALEXER_KIND(t->id)),t->end - t->begin, t->begin);
    alexer_get_token(l, t );
    return true;
}//TODO : the result is not verified and not used for now

bool normal_parser(Alexer_Token *t,Alexer *l){
    double tempNormal[3];
    for (int i = 0; i < 3; ++i)
    {
        if (!double_parser(&tempNormal[i],t,l))return false;
        alexer_get_token(l, t );
    }
    return true;
}//TODO : the result is verified and not used for now


int parser(){
    const char *file_path = "example/path"; // The file path is only needed for diagnostic message
    const char *content =
    "# Blender v2.78 (sub 0) OBJ File: '12.blend\n"
    "# www.blender.org\n"
    "mtllib Goldfish_01.mtl\n"
    "g Goldfish_01_Cube.058\n"
    "v 0.500001 0.500001 0.500001\n"
    "v -0.500000 0.500000 0.500000\n"
    "v -0.500000 -0.500000 0.500000\n"
    "v 0.500000 -0.500000 0.500000\n"
    "v 1.200000 0.500000 1.000000\n"
    "vn -0.4091 -0.8730 0.2654\n"
    "usemtl FF5722\n"
    "s off\n"
    "f 0//1 1//1 2//1 3//1\n"
    "f 3//1 4//1 0//1 \n"
    "";
    Alexer l=alexer_create(file_path, content, strlen(content));
    l.puncts = puncts;
    l.puncts_count = ALEXER_ARRAY_LEN(puncts);
    l.keywords = keywords;
    l.keywords_count = ALEXER_ARRAY_LEN(keywords);
    l.sl_comments = sl_comments;
    l.sl_comments_count = ALEXER_ARRAY_LEN(sl_comments);
    Alexer_Token token = {0};
    bool end=false;
    //TODO: rename those variables to clearer names
    uint64_t cmdToken2 = 0;
    alexer_get_token(&l, &token );
    while (!end){
        if (!alexer_expect_one_of_ids(&l,token,start_tokens,9)) return 1;

        switch (ALEXER_KIND(token.id)){
            case  ALEXER_KEYWORD:
                l.diagf(token.loc, "COMMANDE", "%s: %.*s \n", alexer_kind_name(ALEXER_KIND(token.id)),token.end - token.begin, token.begin);
                cmdToken2=token.id;
                alexer_get_token(&l, &token );
                switch (cmdToken2){
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX):
                        if (!polygon_parser(&token,&l)) return 1;
                        printf("passe v \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_G):
                        if (!g_parser(&token,&l)) return 1;
                        printf("passe g \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_MTL):
                        if (!material_file_parser(&token,&l)) return 1;
                        printf("passe mtl \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_USE):
                        if (!material_usage_parser(&token,&l)) return 1;
                        printf("passe use \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_POINT):
                        if (!vertex_parser(&token,&l)) return 1;
                        printf("passe p \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_S):
                        if (!s_parser(&token,&l)) return 1;
                        printf("passe s \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_NORMAL):
                        if (!normal_parser(&token,&l)) return 1;
                        printf("passe n \n");
                        break;
                    default:
                        printf("error");
                        break;
                }
                break;
            case ALEXER_END:
                end = true;
                break;
            default:
                l.diagf(token.loc, "DONNEE", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token .id)),
                       token.end - token.begin, token.begin);
                break;
        }
    }
    printf("compteurs t:%d v:%d \n",cptTriangles,cptVertexs);
    for (int i = 0; i < cptVertexs; ++i){
        printf("coords points %f, %f, %f \n",vertexList[i].coords[0],
            vertexList[i].coords[1],
            vertexList[i].coords[2]);
    }
    for (int i = 0; i < cptTriangles; ++i){
        printf("triangles from vertexs indexs %d, %d, %d \n",trianglesIndexs[i].verticules[0],
            trianglesIndexs[i].verticules[1],
            trianglesIndexs[i].verticules[2]);
    }
    return 0;
}