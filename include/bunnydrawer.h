
#ifndef _BK_BUNNYDRAWER_H
#define _BK_BUNNYDRAWER_H


bool bkBunnyDrawerLoad();
void bkBunnyDrawerRelease();

#define baSitting 0
#define baRun1 1
#define baRun2 2
#define baRun3 3
#define baJumpUp 4
#define baJumpTop 5
#define baJumpDown1 6
#define baJumpDown2 7
#define baPain 8

void bkBunnyDraw (int stage, bool reverse, char r, char  g, char b);


#endif

