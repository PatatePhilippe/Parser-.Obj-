#include <stdio.h>
#include <alexer.h>
#include <unistd.h>

#define ALEXER_IMPLEMENTATION
#include <errno.h>


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
//TODO : implement something that look like the match in ocaml ?
// typedef enum {
//     POINT_CREATION,
//     TRIANGLE_CREATION,
//     POINT_COUNT,
// } cmd;
//
// const Alexer_Kind *test[POINT_COUNT] ={
//     [POINT_CREATION]      = {KEYWORD_POINT,ALEXER_INT,ALEXER_INT,ALEXER_INT},
//     [TRIANGLE_CREATION]      = {KEYWORD_VERTEX,ALEXER_INT,ALEXER_INT,ALEXER_INT,ALEXER_INT},
// };


const char *sl_comments[] = {
    "#"
};

typedef struct {
    unsigned int *verticules;
} triangle;

typedef struct {
    double coords[3];
} vertex;

typedef struct {//TODO : use that
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
                        ALEXER_ID(ALEXER_KEYWORD,KEYWORD_USE)};
uint64_t start_double[]={ALEXER_ID(ALEXER_PUNCT,PUNCT_MINUS),
                        ALEXER_INT};

triangle trianglesIndexs[400];//TODO : make an auto incrementing triangle memory space or read the file in advance and count the number of vertexs points and normals
vertex vertexList[400];//TODO : make an auto incrementing vertex memory space or read the file in advance and count the number of vertexs points and normals


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
    printf("res =  %s.%s \n",input2,res);

    char str[32];
    snprintf(str, sizeof(str), "%s.%s", input2,res);
    *resultPointer = strtod(str, NULL);
    if (negation)
    {
        *resultPointer*=-1;
    }
    printf("%f \n",*resultPointer);
    return true;
}

bool polygon_parser(Alexer_Token *t,Alexer *l,unsigned int *cptT){

    trianglesIndexs[*cptT].verticules=malloc(3*sizeof(unsigned int));
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
        alexer_get_token(l, t );

        memcpy(&trianglesIndexs[*cptT].verticules[0],&tempPos[0],3*sizeof(unsigned int));
        *cptT+=1;
        trianglesIndexs[*cptT].verticules=malloc(3*sizeof(unsigned int));
        trianglesIndexs[*cptT].verticules[0]=tempPos[2];
        trianglesIndexs[*cptT].verticules[1]=tempPos[3];
        trianglesIndexs[*cptT].verticules[2]=tempPos[0];
    }else{
        if(!alexer_expect_one_of_ids(l,*t,start_tokens,8))return false;
        memcpy(&trianglesIndexs[*cptT].verticules[0],&tempPos[0],3*sizeof(unsigned int));
    }
    *cptT+=1;
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
        return false;//TODO : change the path part later
    }
    alexer_get_token(l, t );
    return true;
}//TODO : the result is verified but not used for now

bool material_usage_parser(Alexer_Token *t,Alexer *l){
    alexer_get_token(l, t );
    alexer_get_token(l, t );
    return true;
}//TODO : the result is not verified and not used for now

bool vertex_parser(Alexer_Token *t,Alexer *l,unsigned int *cptVertexs){
    for (int i = 0; i < 3; ++i)
    {
        if (!double_parser(&vertexList[*cptVertexs].coords[i],t,l))return false;
        alexer_get_token(l, t );
    }
    *cptVertexs+=1;
    return true;
    //printf(" doubles : %f %f %f",x,y,z);
}

//TODO : change name to it's usage later
bool s_parser(Alexer_Token *t,Alexer *l){
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


int main()
{
    const char *file_path = "example/path"; // The file path is only needed for diagnostic message
    const char *content =
    "# Blender v2.78 (sub 0) OBJ File: '12.blend\n"
    "# www.blender.org\n"
    "mtllib Goldfish_01.mtl\n"
    "g Goldfish_01_Cube.058\n"
    "v -1.791817 2.124454 2.120096\n"
    "vn -0.4091 -0.8730 0.2654\n"
    "usemtl FF5722\n"
    "s off\n"
    "f 31//1 19//1 5//1 26//1\n"
    "";
    Alexer l = alexer_create(file_path, content, strlen(content));
    l.puncts = puncts;
    l.puncts_count = ALEXER_ARRAY_LEN(puncts);
    l.keywords = keywords;
    l.keywords_count = ALEXER_ARRAY_LEN(keywords);
    l.sl_comments = sl_comments;
    l.sl_comments_count = ALEXER_ARRAY_LEN(sl_comments);
    Alexer_Token token = {0};
    bool end=false;


    unsigned int cptTriangles=0;
    unsigned int cptVertexs=0;


    //TODO: rename those variables to clearer names
    uint64_t cmdToken2 = 0;
    alexer_get_token(&l, &token );
    while (!end){
        if (!alexer_expect_one_of_ids(&l,token,start_tokens,8)) return 1;
        switch (ALEXER_KIND(token.id)){
            case  ALEXER_KEYWORD:
                l.diagf(token.loc, "COMMANDE", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token.id)),token.end - token.begin, token.begin);
                cmdToken2=token.id;
                alexer_get_token(&l, &token );
                switch (cmdToken2){
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX):
                        if (!polygon_parser(&token,&l,&cptTriangles)) return 1;
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_G):
                        if (!g_parser(&token,&l)) return 1;
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_MTL):
                        if (!material_file_parser(&token,&l)) return 1;
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_USE):
                        if (!material_usage_parser(&token,&l)) return 1;
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_POINT):
                        if (!vertex_parser(&token,&l,&cptVertexs)) return 1;
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_S):
                        if (!s_parser(&token,&l)) return 1;
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_NORMAL):
                        if (!normal_parser(&token,&l)) return 1;
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
    //while (alexer_get_token(&l, &t)) {
    //    l.diagf(t.loc, "INFO", "%s: %.*s", alexer_kind_name(ALEXER_KIND(t.id)), t.end - t.begin, t.begin);
    //}
    //if (!alexer_expect_id(&l, t, ALEXER_END)) return 1;

    return 0;
}