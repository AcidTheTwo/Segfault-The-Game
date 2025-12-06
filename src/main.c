#include "shared.h"
#include "terminal.h"
#include "dive.h"
#include "story.h" 

// Forward declare options logic
void InitOptions();
void UpdateDrawOptions(GameState *currentState);

// DEFINING THE GLOBALS from shared.h
float gameScale = 1.0f;
float gameOffsetX = 0.0f;
float gameOffsetY = 0.0f;

int main(void) {
    // 1. ENABLE RESIZING (Must be before InitWindow)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(1280, 720, "SEGFAULT: Hardcoded Build");
    SetTargetFPS(60);

    // 2. CREATE VIRTUAL SCREEN
    // We render everything to this 1280x720 texture first
    RenderTexture2D target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); // Keep pixels sharp

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
        
        // 3. CALCULATE SCALE FACTOR
        float scaleX = (float)GetScreenWidth() / SCREEN_WIDTH;
        float scaleY = (float)GetScreenHeight() / SCREEN_HEIGHT;
        gameScale = (scaleX < scaleY) ? scaleX : scaleY; // Pick smaller scale to fit

        // Calculate centering offsets (black bars)
        gameOffsetX = (GetScreenWidth() - (SCREEN_WIDTH * gameScale)) * 0.5f;
        gameOffsetY = (GetScreenHeight() - (SCREEN_HEIGHT * gameScale)) * 0.5f;
        
        // -----------------------------------------------------------
        // DRAW TO VIRTUAL SCREEN
        // -----------------------------------------------------------
        BeginTextureMode(target);
            ClearBackground(BLACK); // Clear the virtual screen

            // 1. STORY MODE
            if (currentState == STATE_STORY) {
                bool finished = UpdateDrawStory(&currentState);
                if (finished) {
                    if (isIntro) { currentState = STATE_TERMINAL; isIntro = false; } 
                    else { currentState = STATE_TERMINAL; }
                }
            }
            // 2. TERMINAL & EMAIL
            else if (currentState == STATE_TERMINAL || currentState == STATE_READING_EMAIL) {
                UpdateDrawTerminal(&currentState, &missionUnlocked);
                if (currentState == STATE_TERMINAL && IsKeyPressed(KEY_SPACE) && missionUnlocked) {
                    currentState = STATE_TRANSIT_TO_VIEW;
                    InitDive(1); 
                }
            } 
            // 3. OPTIONS MENU
            else if (currentState == STATE_OPTIONS) {
                UpdateDrawOptions(&currentState);
            }
            // 4. VICTORY RESET
            else if (currentState == STATE_COMPLETE) {
                DrawText("MISSION COMPLETE", 400, 300, 40, LIME);
                DrawText("[ENTER] TO LOG OUT", 480, 360, 20, DARKGREEN);
                if (IsKeyPressed(KEY_ENTER)) {
                    currentState = STATE_TERMINAL;
                    InitDive(1); 
                    missionUnlocked = false;
                }
            }
            // 5. DIVE MODE
            else {
                UpdateDrawDive(&currentState, &camera);
            }

        EndTextureMode();
        
        // -----------------------------------------------------------
        // DRAW TO ACTUAL WINDOW (SCALED)
        // -----------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK); // Draw black bars
            
            // Draw the Render Texture scaled and centered
            DrawTexturePro(target.texture, 
                (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, // Source (flip Y for OpenGL)
                (Rectangle){ gameOffsetX, gameOffsetY, (float)SCREEN_WIDTH * gameScale, (float)SCREEN_HEIGHT * gameScale }, // Dest
                (Vector2){ 0, 0 }, 0.0f, WHITE);
                
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}