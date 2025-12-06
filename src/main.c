#include "shared.h"
#include "terminal.h"
#include "dive.h"
#include "story.h" 

int main(void) {
    InitWindow(1280, 720, "SEGFAULT: Hardcoded Build");
    SetTargetFPS(60);

    GameState currentState = STATE_TERMINAL;
    bool missionUnlocked = false;
    bool isIntro = true; 
    
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 6.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    InitTerminal(1); 
    InitDive(1);     
    InitStory(1);    

    currentState = STATE_STORY; 

    while (!WindowShouldClose()) {
        
        // CHANGE 1: START DRAWING HERE ------------------
        BeginDrawing();
        ClearBackground(BLACK); // Wipes the screen clean for the new frame
        // -----------------------------------------------

        // 1. STORY MODE
        if (currentState == STATE_STORY) {
            bool finished = UpdateDrawStory(&currentState);
            if (finished) {
                if (isIntro) {
                    currentState = STATE_TERMINAL; 
                    isIntro = false;
                } else {
                    currentState = STATE_TERMINAL; 
                }
            }
        }

        // 2. TERMINAL MODE
        else if (currentState == STATE_TERMINAL || currentState == STATE_READING_EMAIL) {
            UpdateDrawTerminal(&currentState, &missionUnlocked);
            
            // Only allow starting the mission if we are on the main list (STATE_TERMINAL)
            if (currentState == STATE_TERMINAL && IsKeyPressed(KEY_SPACE) && missionUnlocked) {
                currentState = STATE_TRANSIT_TO_VIEW;
                InitDive(1); 
            }
        }

        // 3. VICTORY RESET
        else if (currentState == STATE_COMPLETE) {
            // CHANGE 2: REMOVE THE NESTED BeginDrawing/EndDrawing HERE!
            // We handled it at the top level now.
            
            DrawText("MISSION COMPLETE", 400, 300, 40, LIME);
            DrawText("[ENTER] TO LOG OUT", 480, 360, 20, DARKGREEN);
            
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = STATE_TERMINAL;
                InitDive(1); 
                missionUnlocked = false;
            }
        }

        // 4. DIVE MODE
        else {
            UpdateDrawDive(&currentState, &camera);
        }

        // CHANGE 3: END DRAWING HERE --------------------
        EndDrawing();
        // -----------------------------------------------
    }

    CloseWindow();
    return 0;
}