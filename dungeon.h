#ifndef DUNGEON_H
#define DUNGEON_H

#include "engine_data.h"

extern unsigned char Dungeon[TOTAL_CELLS];
extern int Seq[4];

void GerarNovaSemente(void);
void BakeDungeon(void);
unsigned char CelulaMundo(int mundoX, int mundoY);

#endif

