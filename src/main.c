#include "shared.h"
#include "terminal.h"
#include "dive.h"
#include "story.h" 

// Forward declaration so main knows this function exists
void InitOptions();
void UpdateDrawOptions(GameState *currentState);

int main(void) {
    // 1. ENABLE RESIZING (Must be before InitWindow)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

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
    InitOptions();    

    currentState = STATE_STORY; 

    while (!WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == STATE_STORY) {
            bool finished = UpdateDrawStory(&currentState);
            if (finished) {
                if (isIntro) { currentState = STATE_TERMINAL; isIntro = false; } 
                else { currentState = STATE_TERMINAL; }
            }
        }
        else if (currentState == STATE_TERMINAL || currentState == STATE_READING_EMAIL) {
            UpdateDrawTerminal(&currentState, &missionUnlocked);
            if (currentState == STATE_TERMINAL && IsKeyPressed(KEY_SPACE) && missionUnlocked) {
                currentState = STATE_TRANSIT_TO_VIEW;
                InitDive(1); 
            }
        } 
        // --- NEW: OPTIONS STATE ---
        else if (currentState == STATE_OPTIONS) {
            UpdateDrawOptions(&currentState);
        }
        // --------------------------
        else if (currentState == STATE_COMPLETE) {
            DrawText("MISSION COMPLETE", 400, 300, 40, LIME);
            DrawText("[ENTER] TO LOG OUT", 480, 360, 20, DARKGREEN);
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = STATE_TERMINAL;
                InitDive(1); 
                missionUnlocked = false;
            }
        }
        else {
            UpdateDrawDive(&currentState, &camera);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}