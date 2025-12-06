#include "shared.h"
#include "terminal.h"
#include "dive.h"
#include "story.h" // Added story header so we can init it

int main(void) {
    InitWindow(1280, 720, "SEGFAULT: Modular Build");
    SetTargetFPS(1);

    // Initial State
    GameState currentState = STATE_TERMINAL;
    bool missionUnlocked = false;
    bool isIntro = true; // NEW: Track if we are in the Intro or Outro
    
    // Setup Camera
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 6.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Init Modules
    InitTerminal(1); // Load Ep 1 Emails
    
    // --- THE FIX ---
    InitDive(1);     // Load Ep 1 Level (Added the '1')
    InitStory(1);    // Load Ep 1 Script

    // OPTIONAL: Start with Intro Cutscene instead of Terminal
    //currentState = STATE_STORY; 

    while (!WindowShouldClose()) {
        
        // 1. STORY MODE (Intro / Outro)
        if (currentState == STATE_STORY) {
            bool finished = UpdateDrawStory(&currentState);
            
            if (finished) {
                if (isIntro) {
                    currentState = STATE_TERMINAL; // Intro Done -> Go to Menu
                    isIntro = false;
                } else {
                    // Outro Done (Loop or Close)
                    currentState = STATE_TERMINAL; 
                }
            }
        }

        // 2. TERMINAL MODE
        else if (currentState == STATE_TERMINAL) {
            UpdateDrawTerminal(&currentState, &missionUnlocked);
            
            // Start Mission -> Go to TRANSIT (Skip Story since we did it at start)
            if (IsKeyPressed(KEY_SPACE) && missionUnlocked) {
                currentState = STATE_TRANSIT_TO_VIEW;
                // InitDive(1) is already loaded, but we can reload to be safe
                InitDive(1); 
            }
        } 

        // 3. VICTORY RESET
        else if (currentState == STATE_COMPLETE) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("MISSION COMPLETE", 400, 300, 40, LIME);
            DrawText("[ENTER] TO LOG OUT", 480, 360, 20, DARKGREEN);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                currentState = STATE_TERMINAL;
                
                // --- THE FIX ---
                InitDive(1); // Reset Glitches (Added the '1')
                missionUnlocked = false;
            }
        }

        // 4. DIVE MODE (The Game)
        else {
            UpdateDrawDive(&currentState, &camera);
        }
    }

    CloseWindow();
    return 0;
}