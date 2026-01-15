#pragma once
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entity {
        int id;
        char *str;
} Entity;

typedef struct Vec3 {
        double x;
        double y;
        double z;
} Vec3;

typedef void *Ptr;

//
// Generic implementation of function pointer
//
static void PtrFree(Ptr ptr)
{
        Entity *in = (Entity *)ptr;
        if (in && in->str)
                free(in->str);
        if (in)
                free(in);
}
static Ptr PtrDup(Ptr ptr)
{
        assert(ptr && "invalid reference");
        Entity *in = (Entity *)ptr;
        Entity *res = (Entity *)malloc(sizeof(Entity));
        assert(res && "failed allocation");
        res->id = in->id;
        res->str = (char *)malloc(strlen(in->str) + 1);
        assert(res->str && "failed allocation");
        strcpy(res->str, in->str);
        res->str[strlen(in->str)] = 0;
        return res;
}
static Ptr PtrNew(const char *str)
{
        Entity *res = (Entity *)malloc(sizeof(Entity));
        assert(res && "failed allocation");
        static int glob_id = 0;
        res->id = glob_id++;
        res->str = (char *)malloc(strlen(str) + 1);
        assert(res->str && "failed allocation");
        strcpy(res->str, str);
        res->str[strlen(str)] = 0;
        return res;
}
