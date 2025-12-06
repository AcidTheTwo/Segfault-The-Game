#include "dive.h"

static MemoryBlock glitches[MAX_GLITCHES];
static int selectedGlitchIndex = -1;

// Transition State Variables
static Vector3 camStartPos, camEndPos;
static float transitProgress = 0.0f;

// Helper: Custom Grid
void DrawGridColor(int slices, float spacing, Color color) {
    int halfSlices = slices / 2;
    for (int i = -halfSlices; i <= halfSlices; i++) {
        DrawLine3D((Vector3){(float)i,0,(float)-halfSlices}, (Vector3){(float)i,0,(float)halfSlices}, color);
        DrawLine3D((Vector3){(float)-halfSlices,0,(float)i}, (Vector3){(float)halfSlices,0,(float)i}, color);
    }
}

// Helper: Draw Glitch
void DrawGlitch(MemoryBlock *b) {
    if (b->growthProgress > 0) {
        float size = 2.0f * EaseOut(b->growthProgress);
        DrawCubeWires(b->position, size, size, size, LIME);
        DrawCube(b->position, size*0.9f, size*0.9f, size*0.9f, Fade(LIME, 0.2f));
    }
    if (b->corruptionLevel > 0) {
        for(int i=0; i<300; i++) {
            Vector3 p = b->position;
            float s = b->corruptionLevel * 1.5f;
            p.x += GetRandomFloat(-s, s); p.y += GetRandomFloat(-s, s); p.z += GetRandomFloat(-s, s);
            DrawPoint3D(p, (b->corruptionLevel > 0.5f)? RED : ORANGE);
        }
    }
}

void InitDive(void) {
    for(int i=0; i<MAX_GLITCHES; i++) {
        glitches[i] = (MemoryBlock){ .position = { GetRandomFloat(-10,10), 0, GetRandomFloat(-10,10) }, .corruptionLevel = 1.0f, .isActive = true };
    }
}

void UpdateDrawDive(GameState *currentState, Camera *camera) {
    // --- UPDATE LOGIC ---
    
    // TRANSITION
    if (*currentState == STATE_TRANSIT_TO_VIEW || *currentState == STATE_TRANSIT_TO_INSPECT) {
        if (transitProgress == 0.0f) { // Init Transition
            camStartPos = camera->position;
            if (*currentState == STATE_TRANSIT_TO_VIEW) camEndPos = (Vector3){0, 50, 0.1f};
            else camEndPos = (Vector3){glitches[selectedGlitchIndex].position.x, 5, glitches[selectedGlitchIndex].position.z + 5};
        }
        
        transitProgress += 0.5f * GetFrameTime();
        camera->position = Vector3Lerp(camStartPos, camEndPos, transitProgress);
        camera->target = (*currentState == STATE_TRANSIT_TO_VIEW) ? (Vector3){0,0,0} : glitches[selectedGlitchIndex].position;

        if (transitProgress >= 1.0f) {
            transitProgress = 0.0f; // Reset for next time
            *currentState = (*currentState == STATE_TRANSIT_TO_VIEW) ? STATE_VIEW : STATE_INSPECT;
        }
    }
    
    // VIEW MODE
    else if (*currentState == STATE_VIEW) {
        if (GetMouseWheelMove() != 0) camera->position.y -= GetMouseWheelMove() * 2.0f;
        
        // Check Win
        int fixed = 0;
        for(int i=0; i<MAX_GLITCHES; i++) if(glitches[i].corruptionLevel <= 0) fixed++;
        if(fixed == MAX_GLITCHES) *currentState = STATE_COMPLETE;

        // Select
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), *camera);
            for(int i=0; i<MAX_GLITCHES; i++) {
                BoundingBox box = { (Vector3){glitches[i].position.x-2,-2,glitches[i].position.z-2}, (Vector3){glitches[i].position.x+2,2,glitches[i].position.z+2} };
                if (GetRayCollisionBox(ray, box).hit) {
                    selectedGlitchIndex = i;
                    *currentState = STATE_TRANSIT_TO_INSPECT;
                }
            }
        }
    }
    
    // INSPECT MODE
    else if (*currentState == STATE_INSPECT) {
        if (IsKeyDown(KEY_SPACE) && glitches[selectedGlitchIndex].corruptionLevel > 0) 
            glitches[selectedGlitchIndex].corruptionLevel -= 0.02f;
        
        if (glitches[selectedGlitchIndex].corruptionLevel <= 0) {
            glitches[selectedGlitchIndex].corruptionLevel = 0;
            if(glitches[selectedGlitchIndex].growthProgress < 1) glitches[selectedGlitchIndex].growthProgress += 0.05f;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) *currentState = STATE_TRANSIT_TO_VIEW;
    }

    // --- DRAW (3D) ---
    BeginMode3D(*camera);
        DrawGridColor(20, 1.0f, Fade(GREEN, 0.2f));
        for(int i=0; i<MAX_GLITCHES; i++) DrawGlitch(&glitches[i]);
    EndMode3D();
    
    // --- DRAW (UI) ---
    if (*currentState == STATE_VIEW) DrawText("SATELLITE VIEW", 20, 20, 20, GREEN);
    if (*currentState == STATE_INSPECT) DrawText("HOLD SPACE TO REPAIR", 20, 20, 20, WHITE);
    if (*currentState == STATE_COMPLETE) DrawText("MISSION COMPLETE (PRESS ENTER)", 400, 300, 30, LIME);
}