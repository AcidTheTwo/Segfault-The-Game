#ifndef TERMINAL_H
#define TERMINAL_H
#include "shared.h"

// UPDATED: Now accepts int
void InitTerminal(int episodeID); 
void UpdateDrawTerminal(GameState *currentState, bool *missionUnlocked);

#endif