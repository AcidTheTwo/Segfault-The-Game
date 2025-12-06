#include "shared.h"
#include <stdio.h>

// --- SETTINGS STORAGE ---
static bool startFullscreen = false;
static bool startMaximized = false;

// Helper: Save current state to file
void SaveSettings() {
    FILE* file = fopen("assets/settings.txt", "w");
    if (file) {
        // Line 1: Fullscreen (1 or 0)
        fprintf(file, "%d\n", IsWindowFullscreen() ? 1 : 0);
        
        // Line 2: Maximized (1 or 0)
        fprintf(file, "%d\n", IsWindowMaximized() ? 1 : 0);
        
        fclose(file);
        printf("Settings Saved.\n");
    } else {
        printf("ERROR: Could not save settings.\n");
    }
}

// Helper: Load state from file
void LoadSettings() {
    FILE* file = fopen("assets/settings.txt", "r");
    if (!file) {
        printf("Settings file not found, using defaults.\n");
        return;
    }

    int fs = 0, max = 0;
    
    // Read Line 1 (Fullscreen)
    char line[32];
    if (fgets(line, sizeof(line), file)) fs = atoi(line);

    // Read Line 2 (Maximized)
    if (fgets(line, sizeof(line), file)) max = atoi(line);

    fclose(file);

    startFullscreen = (fs == 1);
    startMaximized = (max == 1);
}

void InitOptions() {
    LoadSettings();

    // Apply Settings
    if (startFullscreen) {
        if (!IsWindowFullscreen()) ToggleFullscreen();
    } else {
        // Only maximize if NOT fullscreen (they conflict)
        if (startMaximized) {
             MaximizeWindow();
        }
    }
}

void UpdateDrawOptions(GameState *currentState) {
    
    // --- LOGIC ---
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_TAB)) {
        SaveSettings(); // <--- SAVE ON EXIT
        *currentState = STATE_TERMINAL; 
    }

    if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
    }
    
    if (IsKeyPressed(KEY_M)) {
        if (!IsWindowFullscreen()) {
            if (IsWindowMaximized()) RestoreWindow();
            else MaximizeWindow();
        }
    }

    // --- DRAW ---
    // Note: No BeginDrawing() here because main.c handles the global texture
    
    DrawRectangleLines(100, 100, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 200, GREEN);
    DrawText("SYSTEM CONFIGURATION", 120, 120, 20, GREEN);
    
    DrawText("VIDEO SETTINGS:", 150, 200, 20, ORANGE);
    
    // Option 1: Fullscreen
    const char* fsStatus = IsWindowFullscreen() ? "[ENABLED]" : "[DISABLED]";
    DrawText(TextFormat("[F] FULLSCREEN MODE  %s", fsStatus), 180, 240, 20, WHITE);
    
    // Option 2: Maximize
    if (!IsWindowFullscreen()) {
        const char* maxStatus = IsWindowMaximized() ? "[MAXIMIZED]" : "[WINDOWED]";
        DrawText(TextFormat("[M] WINDOW SIZE      %s", maxStatus), 180, 280, 20, WHITE);
    } else {
        DrawText("[M] WINDOW SIZE      (LOCKED IN FULLSCREEN)", 180, 280, 20, DARKGRAY);
    }

    DrawText("[TAB] SAVE & EXIT", 150, SCREEN_HEIGHT - 150, 20, DARKGREEN);
    
    // Scanlines
    for(int i=0; i<SCREEN_HEIGHT; i+=4) {
        DrawLine(0, i, SCREEN_WIDTH, i, Fade(GREEN, 0.1f));
    }
}