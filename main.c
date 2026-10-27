#include <stdio.h>
#include <alexer.h>
#include <unistd.h>

#define ALEXER_IMPLEMENTATION
#include <errno.h>

#include "alexer.h"

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
    //TODO: see what it does in doc
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


bool copyDouble(double *resultPointer,Alexer_Token *t,Alexer *l){
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
    uint64_t start_tokens[]={ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX),
                            ALEXER_ID(ALEXER_KEYWORD,KEYWORD_MTL),
                            ALEXER_ID(ALEXER_KEYWORD,KEYWORD_G),
                            ALEXER_ID(ALEXER_KEYWORD,KEYWORD_NORMAL),
                            ALEXER_ID(ALEXER_KEYWORD,KEYWORD_POINT),
                            ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX),
                            ALEXER_ID(ALEXER_KEYWORD,KEYWORD_S)};
    uint64_t start_double[]={ALEXER_ID(ALEXER_PUNCT,PUNCT_MINUS),
                            ALEXER_INT};
    uint64_t cmdToken2 = 0;
    while (!end){
        alexer_get_token(&l, &token );
        // printf("debut \n");
        // printf("%s \n",token.begin);
        // printf("mid \n");
        // printf("%s \n",token.end);
        // printf("fin \n");
        if (!alexer_expect_one_of_ids(&l,token,start_tokens,7)) return 1;
        switch (ALEXER_KIND(token.id)){
            case  ALEXER_KEYWORD:
                l.diagf(token.loc, "COMMANDE", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token .id)),
                        token.end - token.begin, token.begin);
                cmdToken2=token.id;
                alexer_get_token(&l, &token );
                switch (cmdToken2){
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_VERTEX):
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_G):
                        l.diagf(token.loc, "PARA 1???", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token .id)),token.end - token.begin, token.begin);
                        if (!alexer_expect_id(&l,token,ALEXER_SYMBOL)) return 1;
                        alexer_get_token(&l, &token );
                        l.diagf(token.loc, "PARA 2", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token .id)),token.end - token.begin, token.begin);
                        if (!alexer_expect_id(&l,token,ALEXER_ID(ALEXER_PUNCT,PUNCT_POINT))) return 1;
                        alexer_get_token(&l, &token );
                         l.diagf(token.loc, "PARA 3", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token .id)),token.end - token.begin, token.begin);
                        if (!alexer_expect_id(&l,token, ALEXER_INT)) return 1;
                        printf("passe g \n");
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_MTL):
                        if (!alexer_expect_id(&l,token,ALEXER_SYMBOL)) return 1;
                        l.diagf(token.loc, "PARA 1", "%s: %.*s", alexer_kind_name(ALEXER_KIND(token .id)),
                                token.end - token.begin, token.begin);
                        //verifications of the file and it's format
                        alexer_get_token(&l, &token );
                        if (!alexer_expect_id(&l,token,ALEXER_ID(ALEXER_PUNCT,PUNCT_POINT))) return 1;
                        alexer_get_token(&l, &token );
                        if (!alexer_expect_id(&l,token, ALEXER_SYMBOL)) return 1;
                        if (access("../Goldfish_01.mtl", F_OK) != 0) {
                            puts( strerror( errno ) );
                            return 1;//TODO : change the path part later
                        }
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_USE):

                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_POINT):
                        double x=0;
                        if (!copyDouble(&x,&token,&l))return 1;
                        alexer_get_token(&l, &token );
                        double y =0;
                        if (!copyDouble(&y,&token,&l))return 1;
                        alexer_get_token(&l, &token );
                        double z =0;
                        if (!copyDouble(&z,&token,&l))return 1;
                        printf(" doubles : %f %f %f",x,y,z);
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_S):
                        break;
                    case ALEXER_ID(ALEXER_KEYWORD,KEYWORD_NORMAL):
                        double x2=0;
                        if (!copyDouble(&x2,&token,&l))return 1;
                        alexer_get_token(&l, &token );
                        double y2 =0;
                        if (!copyDouble(&y2,&token,&l))return 1;
                        alexer_get_token(&l, &token );
                        double z2 =0;
                        if (!copyDouble(&z2,&token,&l))return 1;
                        printf(" doubles : %f %f %f",x2,y2,z2);
                        break;
                    default:
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