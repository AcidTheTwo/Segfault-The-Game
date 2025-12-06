#include "shared.h"  
#include "dive.h"
#include <stdio.h>
#include <stdlib.h> // For atof/strtof

// --- EXTERNAL VISUALS ---
// We removed LoadEpisode1 because we load from text now!
extern void DrawEpisode1Room(void);

// --- STATE VARIABLES ---
static MemoryBlock glitches[MAX_GLITCHES];
static int selectedGlitchIndex = -1;
static int currentEpisodeID = 1;

// MOVEMENT & CUTSCENE VARIABLES
static Vector3 mcPosition = {0, 0, 10.0f}; 
static Vector3 mcTargetPos = {0, 0, 0};
static float walkProgress = 0.0f; 

// TRANSITION VARIABLES
static float exitProgress = 0.0f; 

// QTE VARIABLES
static float qteProgress = 0.0f;   
static float qteSpeed = 1.5f;      
static float qteZoneStart = 0.7f;  
static float qteZoneEnd = 0.9f;    
static bool qteFailed = false;

// --- VISUALS ---

void DrawMC(Vector3 pos) {
    DrawCapsule((Vector3){pos.x, pos.y + 0.5f, pos.z}, (Vector3){pos.x, pos.y + 2.0f, pos.z}, 0.4f, 8, 8, Fade(CYAN, 0.8f));
    DrawSphereWires((Vector3){pos.x, pos.y + 1.8f, pos.z}, 0.3f, 8, 8, WHITE); 
}

void DrawGlitch(MemoryBlock *b) {
    if (b->growthProgress > 0) {
        float size = 1.0f * EaseOut(b->growthProgress);
        DrawCubeWires(b->position, size, size, size, LIME);
        DrawCube(b->position, size*0.9f, size*0.9f, size*0.9f, Fade(LIME, 0.2f));
    }
    if (b->corruptionLevel > 0) {
        for(int i=0; i<300; i++) {
            Vector3 p = b->position;
            float s = b->corruptionLevel * 0.8f;
            p.x += GetRandomFloat(-s, s); 
            p.y += GetRandomFloat(-s, s); 
            p.z += GetRandomFloat(-s, s);
            Color color = (b->corruptionLevel > 0.5f) ? RED : ORANGE;
            DrawPoint3D(p, color);
        }
    }
}

// --- LEVEL LOADER ---

void LoadLevelFromFile(const char* filename) {
    // Reset all glitches first
    for(int i=0; i<MAX_GLITCHES; i++) glitches[i].isActive = false;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ERROR: Level file not found: %s\n", filename);
        return;
    }

    char line[128];
    int count = 0;

    // Format: X Y Z | Corruption
    while (fgets(line, sizeof(line), file) && count < MAX_GLITCHES) {
        float x, y, z, corr;
        // Parse the line
        if (sscanf(line, "%f %f %f | %f", &x, &y, &z, &corr) == 4) {
            glitches[count].position = (Vector3){x, y, z};
            glitches[count].corruptionLevel = corr;
            glitches[count].growthProgress = 0.0f;
            glitches[count].isActive = true;
            count++;
        }
    }
    fclose(file);
    printf("Level Loaded: %d glitches found.\n", count);
}

// --- INIT ---

void InitDive(int episodeNumber) {
    currentEpisodeID = episodeNumber;
    
    // LOAD FROM FILE instead of Hardcoded C function
    if (currentEpisodeID == 1) {
        LoadLevelFromFile("assets/ep1_level.txt");
    }
    
    mcPosition = (Vector3){0, 0, 10.0f}; 
    exitProgress = 0.0f;
}

// --- UPDATE & DRAW ---

void UpdateDrawDive(GameState *currentState, Camera *camera) {
    
    // 1. FLY-IN TRANSITION 
    if (*currentState == STATE_TRANSIT_TO_VIEW) {
        camera->position = Vector3Lerp(camera->position, (Vector3){0, 15, 0.1f}, 0.05f);
        camera->target = (Vector3){0,0,0};
        if (Vector3Distance(camera->position, (Vector3){0, 15, 0.1f}) < 0.5f) {
            *currentState = STATE_VIEW;
        }
    }
    
    // 2. SATELLITE VIEW 
    else if (*currentState == STATE_VIEW) {
        int fixedCount = 0;
        for(int i=0; i<MAX_GLITCHES; i++) {
            if (glitches[i].isActive && glitches[i].corruptionLevel <= 0) fixedCount++;
        }
        // Count active glitches only
        int totalActive = 0;
        for(int i=0; i<MAX_GLITCHES; i++) if(glitches[i].isActive) totalActive++;
        
        if(totalActive > 0 && fixedCount == totalActive) {
            *currentState = STATE_EXITING_SIMULATION;
            exitProgress = 0.0f;
        }

        // WASD
        float speed = 10.0f * GetFrameTime();
        if (IsKeyDown(KEY_W)) { camera->position.z -= speed; camera->target.z -= speed; }
        if (IsKeyDown(KEY_S)) { camera->position.z += speed; camera->target.z += speed; }
        if (IsKeyDown(KEY_A)) { camera->position.x -= speed; camera->target.x -= speed; }
        if (IsKeyDown(KEY_D)) { camera->position.x += speed; camera->target.x += speed; }

        // Selection
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), *camera);
            for(int i=0; i<MAX_GLITCHES; i++) {
                if (!glitches[i].isActive || glitches[i].corruptionLevel <= 0) continue;

                Vector3 pos = glitches[i].position;
                BoundingBox box = { 
                    (Vector3){pos.x-1, pos.y-1, pos.z-1}, 
                    (Vector3){pos.x+1, pos.y+1, pos.z+1} 
                };
                
                if (GetRayCollisionBox(ray, box).hit) {
                    selectedGlitchIndex = i;
                    *currentState = STATE_CUTSCENE_WALK;
                    walkProgress = 0.0f;
                    mcTargetPos = glitches[i].position;
                    mcTargetPos.z += 2.0f; 
                    mcTargetPos.y = 0.0f; 
                }
            }
        }
    }

    // 3. CUTSCENE
    else if (*currentState == STATE_CUTSCENE_WALK) {
        walkProgress += 0.5f * GetFrameTime(); 
        mcPosition = Vector3Lerp(mcPosition, mcTargetPos, 0.05f); 
        camera->position = (Vector3){ mcPosition.x + 3.0f, mcPosition.y + 4.0f, mcPosition.z + 4.0f };
        camera->target = mcPosition;

        if (Vector3Distance(mcPosition, mcTargetPos) < 0.5f) {
            *currentState = STATE_QTE;
            qteProgress = 0.0f;
            qteFailed = false;
        }
    }

    // 4. QTE
    else if (*currentState == STATE_QTE) {
        qteProgress += qteSpeed * GetFrameTime();
        if (qteProgress > 1.2f) { qteProgress = 0.0f; qteFailed = true; }

        if (IsKeyPressed(KEY_SPACE)) {
            if (qteProgress >= qteZoneStart && qteProgress <= qteZoneEnd) {
                glitches[selectedGlitchIndex].corruptionLevel = 0; 
                glitches[selectedGlitchIndex].growthProgress = 0.05f; 
                *currentState = STATE_TRANSIT_BACK_TO_VIEW; 
            } else {
                qteFailed = true;
                qteProgress = 0.0f; 
            }
        }
    }
    
    // 5. FLY-OUT
    else if (*currentState == STATE_TRANSIT_BACK_TO_VIEW) {
        camera->position = Vector3Lerp(camera->position, (Vector3){0, 15, 0.1f}, 0.05f);
        camera->target = Vector3Lerp(camera->target, (Vector3){0, 0, 0}, 0.05f);
        if (Vector3Distance(camera->position, (Vector3){0, 15, 0.1f}) < 0.5f) {
            *currentState = STATE_VIEW;
            camera->position = (Vector3){0, 15, 0.1f};
            camera->target = (Vector3){0, 0, 0};
        }
    }

    // 6. EXIT
    else if (*currentState == STATE_EXITING_SIMULATION) {
        exitProgress += GetFrameTime();
        camera->position.y += 20.0f * GetFrameTime();
        camera->target = Vector3Lerp(camera->target, (Vector3){0,0,0}, 0.1f);
        if (exitProgress > 2.0f) *currentState = STATE_COMPLETE;
    }

    // --- DRAW PHASE ---
    BeginMode3D(*camera);
        if (currentEpisodeID == 1) DrawEpisode1Room();
        
        if (*currentState == STATE_CUTSCENE_WALK || *currentState == STATE_QTE || *currentState == STATE_TRANSIT_BACK_TO_VIEW) {
            DrawMC(mcPosition);
        }

        for(int i=0; i<MAX_GLITCHES; i++) {
            if (glitches[i].isActive) DrawGlitch(&glitches[i]);
        }
    EndMode3D();
    
    // UI
    if (*currentState == STATE_VIEW) {
        DrawText("SATELLITE LINK ACTIVE", 20, 20, 20, GREEN);
        DrawText("WASD TO MOVE - CLICK RED ZONES TO DEPLOY", 20, 50, 20, WHITE);
    }
    else if (*currentState == STATE_CUTSCENE_WALK) {
        DrawText("DEPLOYING GHOST UNIT...", 500, 600, 30, Fade(CYAN, 0.8f));
    }
    else if (*currentState == STATE_QTE) {
        DrawText("STABILIZE MEMORY", 450, 500, 20, WHITE);
        DrawRectangle(400, 550, 400, 30, DARKGRAY);
        int barW = 400;
        DrawRectangle(400 + (int)(barW * qteZoneStart), 550, (int)(barW * (qteZoneEnd - qteZoneStart)), 30, GREEN);
        int markerX = 400 + (int)(barW * qteProgress);
        if (markerX > 400 + barW) markerX = 400 + barW; 
        DrawRectangle(markerX, 540, 5, 50, WHITE);
        
        if (qteFailed) DrawText("SYNC FAILED - RETRY", 500, 600, 20, RED);
        else DrawText("[PRESS SPACE IN GREEN ZONE]", 480, 600, 20, LIME);
    }
    else if (*currentState == STATE_EXITING_SIMULATION) {
        float alpha = exitProgress / 2.0f; 
        if (alpha > 1.0f) alpha = 1.0f;
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(DARKGREEN, alpha));
        DrawText("DISCONNECTING...", 500, 350, 30, Fade(WHITE, alpha));
    }
}