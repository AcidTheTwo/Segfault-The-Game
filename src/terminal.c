#include "shared.h" 
#include "terminal.h"

static Email inbox[MAX_EMAILS];
static int selectedEmailIndex = 0;

// HELPER: Loads a single email file
void LoadEmailFromFile(int index, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ERROR: Could not load %s\n", filename);
        strcpy(inbox[index].sender, "ERROR");
        strcpy(inbox[index].subject, "FILE NOT FOUND");
        return;
    }

    char line[256];

    // 1. Sender
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; // Strip newline
        strcpy(inbox[index].sender, line);
    }
    
    // 2. Subject
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        strcpy(inbox[index].subject, line);
    }

    // 3. Is Mission Trigger? (0 or 1)
    if (fgets(line, sizeof(line), file)) {
        inbox[index].isMissionTrigger = (line[0] == '1');
    }

    // 4. Body (Read remaining lines)
    inbox[index].body[0] = '\0'; // Clear body
    while (fgets(line, sizeof(line), file)) {
        strcat(inbox[index].body, line);
    }

    inbox[index].isRead = false;
    fclose(file);
}

void InitTerminal(int episodeID) {
    // Load the files from the assets folder
    // Note: Paths are relative to where you run the .exe
    LoadEmailFromFile(0, "assets/ep1_email_0.txt");
    LoadEmailFromFile(1, "assets/ep1_email_1.txt");
    LoadEmailFromFile(2, "assets/ep1_email_2.txt");
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

        DrawLine(50, 550, 1230, 550, GREEN);
        if (*missionUnlocked) DrawText("[SPACE] START MISSION", 70, 570, 20, LIME);
        else DrawText("MISSION LOCKED (READ EMAILS)", 70, 570, 20, DARKGRAY);
    }
    else if (*currentState == STATE_READING_EMAIL) {
        DrawRectangleLines(100, 100, 1080, 520, GREEN);
        Email *e = &inbox[selectedEmailIndex];
        
        DrawText(TextFormat("FROM: %s", e->sender), 120, 120, 20, GREEN);
        DrawText(TextFormat("SUBJ: %s", e->subject), 120, 150, 20, LIME);
        DrawLine(100, 180, 1180, 180, GREEN);
        DrawText(e->body, 120, 200, 20, WHITE);
        DrawText("[BACKSPACE] RETURN", 120, 580, 20, DARKGREEN);
    }
}