#include "terminal.h"

// Private variables for this module
static Email inbox[MAX_EMAILS];
static int selectedEmailIndex = 0;

void InitTerminal(void) {
    inbox[0] = (Email){ "HR_Bot", "Paycheck Delayed", "Due to server instability, payments are delayed.", false, false };
    inbox[1] = (Email){ "Client: Stirling", "TICKET #9092", "My daughter's birthday memory is corrupted.\nPlease fix it.", false, true };
    inbox[2] = (Email){ "UNKNOWN", ">> SEGFAULT <<", "They deleted the audio.\nFind out why.", false, false };
}

void UpdateDrawTerminal(GameState *currentState, bool *missionUnlocked) {
    // --- LOGIC ---
    if (*currentState == STATE_TERMINAL) {
        if (IsKeyPressed(KEY_UP)) selectedEmailIndex = (selectedEmailIndex - 1 + MAX_EMAILS) % MAX_EMAILS;
        if (IsKeyPressed(KEY_DOWN)) selectedEmailIndex = (selectedEmailIndex + 1) % MAX_EMAILS;
        
        if (IsKeyPressed(KEY_ENTER)) {
            *currentState = STATE_READING_EMAIL;
            inbox[selectedEmailIndex].isRead = true;
            if (inbox[selectedEmailIndex].isMissionTrigger) *missionUnlocked = true;
        }
        
        // Start Mission Trigger
        if (IsKeyPressed(KEY_SPACE) && *missionUnlocked) {
            *currentState = STATE_TRANSIT_TO_VIEW;
        }
    }
    else if (*currentState == STATE_READING_EMAIL) {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) *currentState = STATE_TERMINAL;
    }

    // --- DRAW ---
    if (*currentState == STATE_TERMINAL) {
        DrawRectangleLines(50, 50, 1180, 620, GREEN);
        DrawText("AMNESYS_OS v4.0", 70, 70, 20, GREEN);
        
        for (int i=0; i<MAX_EMAILS; i++) {
            Color c = (i == selectedEmailIndex) ? LIME : DARKGREEN;
            DrawText(inbox[i].subject, 70, 140 + (i*40), 20, c);
            if (!inbox[i].isRead) DrawText("[NEW]", 400, 140 + (i*40), 10, RED);
        }

        if (*missionUnlocked) DrawText("[SPACE] START MISSION", 70, 570, 20, LIME);
        else DrawText("MISSION LOCKED (READ EMAILS)", 70, 570, 20, DARKGRAY);
    }
    else if (*currentState == STATE_READING_EMAIL) {
        DrawRectangleLines(100, 100, 1080, 520, GREEN);
        DrawText(inbox[selectedEmailIndex].body, 120, 200, 20, WHITE);
        DrawText("[BACKSPACE] RETURN", 120, 580, 20, DARKGREEN);
    }
}