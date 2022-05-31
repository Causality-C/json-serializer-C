#ifndef __PLAYER_STRUCT_H
#define __PLAYER_STRUCT_H

struct Player{
    char name[20];
    int level;
    float height;    
    char division;
};

void printPlayer(struct Player *);

#endif