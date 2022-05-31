#ifndef __SERIALIZER_H
#define __SERIALIZER_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// DATATYPES
typedef enum{
    CHAR,
    INT,
    FLOAT,
    UNKNOWN
} DataTypes_t;

// Member of a Struct
struct Member{
    DataTypes_t type;
    char * value;
    uint8_t isArray;
};

typedef struct{
    int numFields;
    struct Member ** members;
} StructRep;

StructRep * getStructRepresentation(char * structFields);
char * getJSONFromStruct(StructRep *);
void printStructRepresentation(StructRep *);
void freeStructRep(StructRep * rep);
char * getFormatter(struct Member *);
DataTypes_t getDataTypeFromString(char * str);
void appendToString(int * ptr, char * src, char * dest);
char * combineMultipleStrings(int num, ...);
#endif