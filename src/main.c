#include "shared.h"
#include "terminal.h"
#include "dive.h"

int main(void) {
    InitWindow(1280, 720, "SEGFAULT: Modular Build");
    SetTargetFPS(60);

    // Initial State
    GameState currentState = STATE_TERMINAL;
    bool missionUnlocked = false;
    
    // Setup Camera
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 6.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Init Modules
    InitTerminal();
    InitDive();

    while (!WindowShouldClose()) {
        
        // Reset Logic (Global)
        if (currentState == STATE_COMPLETE && IsKeyPressed(KEY_ENTER)) {
            currentState = STATE_TERMINAL;
            InitDive(); // Reset glitches
        }

        BeginDrawing();
            ClearBackground((Color){10, 10, 10, 255});

            // ROUTER: Decide which module to run
            if (currentState == STATE_TERMINAL || currentState == STATE_READING_EMAIL) {
                UpdateDrawTerminal(&currentState, &missionUnlocked);
            } 
            else {
                // Dive Mode handles View, Transit, Inspect, and Complete
                UpdateDrawDive(&currentState, &camera);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}