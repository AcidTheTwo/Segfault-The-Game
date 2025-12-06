#ifndef SHARED_H
#define SHARED_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>

// --- CONFIG ---
#define MAX_GLITCHES 3
#define MAX_EMAILS 3

// --- STATES ---
typedef enum GameState {
    STATE_TERMINAL,          // Menu
    STATE_READING_EMAIL,     // Reading
    STATE_STORY,             // <--- NEW: Added this!
    STATE_TRANSIT_TO_VIEW,   // Flying to map
    STATE_VIEW,              // Satellite Map
    STATE_TRANSIT_TO_INSPECT,// Zooming in
    STATE_INSPECT,           // Fixing
    STATE_COMPLETE           // Win
} GameState;

// --- DATA STRUCTURES ---
typedef struct MemoryBlock {
    Vector3 position;
    bool isActive;
    float corruptionLevel; 
    float growthProgress;  
} MemoryBlock;

typedef struct Email {
    const char* sender;
    const char* subject;
    const char* body;
    bool isRead;
    bool isMissionTrigger;
} Email;

// --- HELPER MATH ---
static inline float EaseOut(float t) { return t * (2.0f - t); }
static inline float GetRandomFloat(float min, float max) {
    return min + ((float)GetRandomValue(0, 10000) / 10000.0f) * (max - min);
}

#endif