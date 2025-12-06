#ifndef DIVE_H
#define DIVE_H
#include "shared.h"

void InitDive(int episodeNumber); 
void UpdateDrawDive(GameState *currentState, Camera *camera);

void LoadEpisode1(MemoryBlock *glitches);
void DrawEpisode1Room(void);

#endif