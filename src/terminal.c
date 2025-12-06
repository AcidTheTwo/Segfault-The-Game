#include "terminal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Required for malloc/atoi

static Email inbox[MAX_EMAILS];
static int selectedEmailIndex = 0;

// --- FILE LOADING SYSTEM ---
Email LoadEmailFromFile(const char* filename) {
    Email newEmail = {0}; 
    newEmail.sender = strdup("UNKNOWN SENDER");
    newEmail.subject = strdup("NO SUBJECT");
    newEmail.body = strdup("No data.");
    newEmail.isRead = false;
    newEmail.isMissionTrigger = false;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        newEmail.sender = "SYSTEM ERROR";
        newEmail.subject = "FILE NOT FOUND";
        newEmail.body = "Error loading data.";
        return newEmail;
    }

    char buffer[256];
    if (fgets(buffer, 256, file)) {
        buffer[strcspn(buffer, "\n")] = 0;
        newEmail.sender = strdup(buffer); 
    }
    if (fgets(buffer, 256, file)) {
        buffer[strcspn(buffer, "\n")] = 0;
        newEmail.subject = strdup(buffer);
    }
    if (fgets(buffer, 256, file)) {
        newEmail.isMissionTrigger = (atoi(buffer) == 1);
    }

    char *bodyBuffer = (char*)malloc(1024); 
    bodyBuffer[0] = '\0'; 
    while (fgets(buffer, 256, file)) {
        strcat(bodyBuffer, buffer); 
    }
    newEmail.body = bodyBuffer;
    newEmail.isRead = false;

    fclose(file);
    return newEmail;
}

// --- THE FIX IS HERE ---
// Ensure this matches the header: void InitTerminal(int episodeID)
void InitTerminal(int episodeID) {
    char filepath[64];

    for (int i = 0; i < MAX_EMAILS; i++) {
        // Load "assets/ep1_email_0.txt", etc.
        sprintf(filepath, "assets/ep%d_email_%d.txt", episodeID, i);
        inbox[i] = LoadEmailFromFile(filepath);
    }
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