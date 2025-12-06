#ifndef STORY_H
#define STORY_H

#include "shared.h"

// Initializes the story engine and loads the script for the specific episode
// Example: InitStory(1) loads "assets/ep1_script.txt"
void InitStory(int episodeID);

// Runs the logic and drawing for the cutscene.
// Returns 'true' when the script is finished, so main.c can switch states.
bool UpdateDrawStory(GameState *currentState);

// Returns the result of the last choice made (1 or 2)
// You can use this in main.c to change the game ending.
int GetLastChoice(void);

#endif