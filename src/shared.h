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
    STATE_TERMINAL,          
    STATE_READING_EMAIL,     
    STATE_OPTIONS,              // <--- NEW: Options Menu
    STATE_STORY,             
    
    STATE_TRANSIT_TO_VIEW,      
    STATE_VIEW,                 
    STATE_CUTSCENE_WALK,        
    STATE_QTE,                  
    STATE_TRANSIT_BACK_TO_VIEW, 
    
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

// --- SCREEN SCALING GLOBALS ---
// Defined in main.c, used by dive.c to fix mouse input
extern float gameScale;
extern float gameOffsetX;
extern float gameOffsetY;

// Helper to get mouse coordinates relative to the virtual 1280x720 screen
static inline Vector2 GetVirtualMouse() {
    Vector2 m = GetMousePosition();
    return (Vector2){ 
        (m.x - gameOffsetX) / gameScale, 
        (m.y - gameOffsetY) / gameScale 
    };
}

#endif
