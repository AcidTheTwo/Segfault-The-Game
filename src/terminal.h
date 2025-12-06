#ifndef TERMINAL_H
#define TERMINAL_H
#include "shared.h"

void InitTerminal(void);
void UpdateDrawTerminal(GameState *currentState, bool *missionUnlocked);

#endif