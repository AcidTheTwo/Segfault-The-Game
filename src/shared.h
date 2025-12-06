#ifndef SHARED_H
#define SHARED_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 

// --- CONFIG ---
#define MAX_GLITCHES 3
#define MAX_EMAILS 3
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CYAN CLITERAL(Color){0, 255, 255}

// --- STATES ---
typedef enum GameState {
    STATE_OPTIONS,
    STATE_TERMINAL,          
    STATE_READING_EMAIL,     
    STATE_STORY,             
    
    STATE_TRANSIT_TO_VIEW,      // Fly IN
    STATE_VIEW,                 // Satellite Mode
    STATE_CUTSCENE_WALK,        // MC Walk
    STATE_QTE,                  // Repair
    STATE_TRANSIT_BACK_TO_VIEW, // Fly OUT <--- NEW!
    
    STATE_EXITING_SIMULATION,   // Mission Complete fade
    STATE_COMPLETE           
} GameState;

// --- DATA STRUCTURES ---

typedef struct MemoryBlock {
    Vector3 position;
    bool isActive;
    float corruptionLevel; 
    float growthProgress;  
} MemoryBlock;

typedef struct Email {
    char sender[64];
    char subject[64];
    char body[512]; 
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
