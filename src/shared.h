#ifndef SHARED_H
#define SHARED_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h> // Added for string manip
#include <stdlib.h> // Added for parsing numbers

// --- CONFIG ---
#define MAX_GLITCHES 3
#define MAX_EMAILS 3
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CYAN CLITERAL(Color){0, 255, 255}

// --- STATES ---
typedef enum GameState {
    STATE_TERMINAL,          
    STATE_READING_EMAIL,     
    STATE_STORY,             
    STATE_TRANSIT_TO_VIEW,   
    STATE_VIEW,              
    STATE_CUTSCENE_WALK,     
    STATE_QTE,               
    STATE_EXITING_SIMULATION,
    STATE_COMPLETE           
} GameState;

// --- DATA STRUCTURES ---

typedef struct MemoryBlock {
    Vector3 position;
    bool isActive;
    float corruptionLevel; 
    float growthProgress;  
} MemoryBlock;

// UPDATED: Now uses arrays so we can load text from files
typedef struct Email {
    char sender[64];
    char subject[64];
    char body[512]; // Enough space for a long email
    bool isRead;
    bool isMissionTrigger;
} Email;

// --- HELPER MATH ---

static inline float EaseOut(float t) { 
    return t * (2.0f - t); 
}

static inline float GetRandomFloat(float min, float max) {
    return min + ((float)GetRandomValue(0, 10000) / 10000.0f) * (max - min);
}

#endif