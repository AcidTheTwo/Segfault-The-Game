#include "dive.h"

// --- STATE VARIABLES ---
static MemoryBlock glitches[MAX_GLITCHES];
static int selectedGlitchIndex = -1;

// NEW: Track the current episode so we know which room to draw
static int currentEpisodeID = 1; 

// Transition Variables
static Vector3 camStartPos, camEndPos;
static float transitProgress = 0.0f;

// --- SHARED VISUALS ---

// Helper: Renders the glitch effect (Wireframe Cube + Lidar Dots)
// This logic is shared across all episodes.
void DrawGlitch(MemoryBlock *b) {
    // 1. Growth (Restored State)
    if (b->growthProgress > 0) {
        float size = 1.0f * EaseOut(b->growthProgress); // Size is 1.0 to match furniture props
        DrawCubeWires(b->position, size, size, size, LIME);
        DrawCube(b->position, size*0.9f, size*0.9f, size*0.9f, Fade(LIME, 0.2f));
    }
    
    // 2. Corruption (Lidar State)
    if (b->corruptionLevel > 0) {
        for(int i=0; i<300; i++) {
            Vector3 p = b->position;
            float s = b->corruptionLevel * 0.8f; // Tighter spread for props
            p.x += GetRandomFloat(-s, s); 
            p.y += GetRandomFloat(-s, s); 
            p.z += GetRandomFloat(-s, s);
            DrawPoint3D(p, (b->corruptionLevel > 0.5f)? RED : ORANGE);
        }
    }
}

// --- INIT ---

void InitDive(int episodeNumber) {
    currentEpisodeID = episodeNumber;
    
    // Select the Level Data based on ID
    if (currentEpisodeID == 1) {
        LoadEpisode1(glitches);
    }
    // Future: else if (currentEpisodeID == 2) LoadEpisode2(glitches);
}

// --- UPDATE & DRAW ---

void UpdateDrawDive(GameState *currentState, Camera *camera) {
    
    // 1. TRANSITION LOGIC (Camera movement)
    if (*currentState == STATE_TRANSIT_TO_VIEW || *currentState == STATE_TRANSIT_TO_INSPECT) {
        if (transitProgress == 0.0f) { 
            camStartPos = camera->position;
            
            if (*currentState == STATE_TRANSIT_TO_VIEW) {
                // Fly to Satellite View (High up)
                camEndPos = (Vector3){0, 15, 0.1f}; 
            } else {
                // Fly to Object (Zoom in)
                Vector3 target = glitches[selectedGlitchIndex].position;
                camEndPos = (Vector3){target.x, target.y + 3.0f, target.z + 3.0f};
            }
        }
        
        transitProgress += 1.0f * GetFrameTime(); // Speed = 1.0 (1 second duration)
        camera->position = Vector3Lerp(camStartPos, camEndPos, transitProgress);
        
        // Handle Look Target
        if (*currentState == STATE_TRANSIT_TO_VIEW) camera->target = (Vector3){0,0,0};
        else camera->target = glitches[selectedGlitchIndex].position;

        if (transitProgress >= 1.0f) {
            transitProgress = 0.0f;
            *currentState = (*currentState == STATE_TRANSIT_TO_VIEW) ? STATE_VIEW : STATE_INSPECT;
        }
    }
    
    // 2. VIEW MODE LOGIC (Satellite Selection)
    else if (*currentState == STATE_VIEW) {
        // Zoom
        if (GetMouseWheelMove() != 0) {
            camera->position.y -= GetMouseWheelMove() * 2.0f;
            if (camera->position.y < 5.0f) camera->position.y = 5.0f;
        }
        
        // Check Win Condition
        int fixed = 0;
        for(int i=0; i<MAX_GLITCHES; i++) if(glitches[i].corruptionLevel <= 0) fixed++;
        if(fixed == MAX_GLITCHES) *currentState = STATE_COMPLETE;

        // Select Glitch
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), *camera);
            for(int i=0; i<MAX_GLITCHES; i++) {
                if (!glitches[i].isActive) continue;

                Vector3 pos = glitches[i].position;
                BoundingBox box = { (Vector3){pos.x-1, pos.y-1, pos.z-1}, (Vector3){pos.x+1, pos.y+1, pos.z+1} };
                
                if (GetRayCollisionBox(ray, box).hit) {
                    selectedGlitchIndex = i;
                    *currentState = STATE_TRANSIT_TO_INSPECT;
                }
            }
        }
    }
    
    // 3. INSPECT MODE LOGIC (Repairing)
    else if (*currentState == STATE_INSPECT) {
        MemoryBlock *current = &glitches[selectedGlitchIndex];

        // Manual Repair (Spacebar)
        if (IsKeyDown(KEY_SPACE) && current->corruptionLevel > 0) 
            current->corruptionLevel -= 0.01f;
        
        // Auto-Grow Animation
        if (current->corruptionLevel <= 0) {
            current->corruptionLevel = 0;
            if(current->growthProgress < 1) current->growthProgress += 0.05f;
        }

        // Exit (Right Click)
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) *currentState = STATE_TRANSIT_TO_VIEW;
    }

    // --- DRAW PHASE ---
    BeginMode3D(*camera);
        
        // A. Draw the Environment
        if (currentEpisodeID == 1) {
            DrawEpisode1Room(); // Defined in ep1.c
        }
        // else if (currentEpisodeID == 2) DrawEpisode2Room();

        // B. Draw the Glitches
        for(int i=0; i<MAX_GLITCHES; i++) {
            if (glitches[i].isActive) DrawGlitch(&glitches[i]);
        }
        
    EndMode3D();
    
    // --- UI OVERLAY ---
    if (*currentState == STATE_VIEW) {
        DrawText("SECTOR: LIVING_ROOM", 20, 20, 20, GREEN);
        DrawText("CLICK RED ZONES TO INSPECT", 20, 50, 20, WHITE);
    }
    if (*currentState == STATE_INSPECT) {
        DrawText("OBJECT INTERFACE", 20, 20, 20, ORANGE);
        DrawText("HOLD [SPACE] TO STABILIZE", 20, 50, 20, WHITE);
        
        // Health Bar
        float health = glitches[selectedGlitchIndex].corruptionLevel;
        DrawRectangle(20, 80, 200, 10, DARKGRAY);
        DrawRectangle(20, 80, (int)(200 * health), 10, RED);
    }
    if (*currentState == STATE_COMPLETE) {
        DrawText("MEMORY RESTORED", 400, 300, 40, LIME);
        DrawText("[PRESS ENTER]", 500, 350, 20, GREEN);
    }
}