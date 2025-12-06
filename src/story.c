#include "story.h"
#include <stdio.h>
#include <string.h>

#define MAX_SCRIPT_LINES 100

// Types of lines in your script file
typedef enum LineType { 
    LINE_DIALOGUE, 
    LINE_CHOICE 
} LineType;

// Represents one "Moment" in the cinematic
typedef struct ScriptLine {
    LineType type;
    char speaker[32];    // Who is talking?
    char text[128];      // What do they say?
    char optionA[64];    // Choice 1 (Only used if type == LINE_CHOICE)
    char optionB[64];    // Choice 2 (Only used if type == LINE_CHOICE)
    Vector3 camPos;      // Where the camera should be
    Vector3 camTarget;   // What the camera should look at
} ScriptLine;

// --- STATE VARIABLES ---
static ScriptLine script[MAX_SCRIPT_LINES];
static int totalLines = 0;
static int currentLineIndex = 0;
static int lastChoice = 0; // 0=None, 1=OptionA, 2=OptionB

static Camera camera = { 0 };

// --- VISUALS ---

// Draws a cool "Lidar Ghost" to represent a character speaking
void DrawActor(Vector3 pos, Color color) {
    // Draw a capsule shape made of dots
    for(int i=0; i<50; i++) {
        Vector3 p = pos;
        p.y += GetRandomFloat(0, 1.8f); // Height of a person
        p.x += GetRandomFloat(-0.2f, 0.2f);
        p.z += GetRandomFloat(-0.2f, 0.2f);
        DrawPoint3D(p, color);
    }
}

// Draws the environment context (The "Stage")
void DrawCinematicStage() {
    // Draw a subtle floor grid so movement is visible
    int slices = 20;
    Color gridColor = Fade(GREEN, 0.1f);

    int halfSlices = slices / 2;
    for (int i = -halfSlices; i <= halfSlices; i++) {
        DrawLine3D((Vector3){(float)i,0,(float)-halfSlices}, (Vector3){(float)i,0,(float)halfSlices}, gridColor);
        DrawLine3D((Vector3){(float)-halfSlices,0,(float)i}, (Vector3){(float)halfSlices,0,(float)i}, gridColor);
    }
}

// --- LOGIC ---

void InitStory(int episodeID) {
    // 1. Setup Camera Defaults
    camera.up = (Vector3){0,1,0};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // 2. Load Script
    char filename[64];
    sprintf(filename, "assets/ep%d_script.txt", episodeID);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        // Fallback if file missing
        totalLines = 1;
        currentLineIndex = 0;
        strcpy(script[0].speaker, "SYSTEM");
        strcpy(script[0].text, "ERROR: SCRIPT FILE NOT FOUND");
        script[0].camPos = (Vector3){0, 5, 5};
        return;
    }

    totalLines = 0;
    currentLineIndex = 0;
    lastChoice = 0;
    
    char buffer[256];
    while (totalLines < MAX_SCRIPT_LINES && fgets(buffer, 256, file)) {
        ScriptLine *line = &script[totalLines];
        char typeStr[16];
        
        // Peek at the first word to see if it's a CHOICE or DIALOGUE
        // We use | as the separator
        sscanf(buffer, "%[^|]", typeStr);
        
        if (strcmp(typeStr, "CHOICE") == 0) {
            line->type = LINE_CHOICE;
            // Format: CHOICE|Question|OptA|OptB|CamX|CamY|CamZ|LookX|LookY|LookZ
            sscanf(buffer, "CHOICE|%[^|]|%[^|]|%[^|]|%f|%f|%f|%f|%f|%f",
                line->text, line->optionA, line->optionB,
                &line->camPos.x, &line->camPos.y, &line->camPos.z,
                &line->camTarget.x, &line->camTarget.y, &line->camTarget.z
            );
        } else {
            line->type = LINE_DIALOGUE;
            // Format: Speaker|Text|CamX|CamY|CamZ|LookX|LookY|LookZ
            // Note: Speaker name is mapped to 'typeStr' from the first sscanf
            strcpy(line->speaker, typeStr);
            
            sscanf(buffer, "%*[^|]|%[^|]|%f|%f|%f|%f|%f|%f",
                line->text,
                &line->camPos.x, &line->camPos.y, &line->camPos.z,
                &line->camTarget.x, &line->camTarget.y, &line->camTarget.z
            );
        }
        totalLines++;
    }
    fclose(file);
    
    // Set initial camera instantly to the first shot (no smooth lerp for the first frame)
    if (totalLines > 0) {
        camera.position = script[0].camPos;
        camera.target = script[0].camTarget;
    }
}

bool UpdateDrawStory(GameState *currentState) {
    if (currentLineIndex >= totalLines) return true; // Script Finished!

    ScriptLine *current = &script[currentLineIndex];

    // --- 1. UPDATE LOGIC ---
    
    // Camera Move: "Cinematic Lerp"
    // We move 5% of the distance every frame. This creates a smooth "Ease Out" feel.
    camera.position = Vector3Lerp(camera.position, current->camPos, 0.05f);
    camera.target = Vector3Lerp(camera.target, current->camTarget, 0.1f); // Look target moves faster

    // Input Handling
    if (current->type == LINE_DIALOGUE) {
        if (IsKeyPressed(KEY_SPACE)) currentLineIndex++;
    }
    else if (current->type == LINE_CHOICE) {
        if (IsKeyPressed(KEY_ONE)) { 
            lastChoice = 1; 
            currentLineIndex++; 
        }
        if (IsKeyPressed(KEY_TWO)) { 
            lastChoice = 2; 
            currentLineIndex++; 
        }
    }

    // --- 2. DRAW SCENE ---
    BeginMode3D(camera);
        DrawCinematicStage();
        
        // Draw the actors at the focus point so the camera looks at "someone"
        // (Visual trick: We draw the actor slightly below the camera's look target)
        DrawActor((Vector3){current->camTarget.x, 0, current->camTarget.z}, GREEN);
    EndMode3D();

    // --- 3. DRAW UI ---
    // Cinematic Black Bars
    DrawRectangle(0, 0, 1280, 50, BLACK);
    DrawRectangle(0, 580, 1280, 140, Fade(BLACK, 0.9f)); // Bottom text area

    if (current->type == LINE_DIALOGUE) {
        DrawText(current->speaker, 60, 600, 20, ORANGE);
        DrawText(current->text, 60, 630, 28, WHITE);
        DrawText("[SPACE] NEXT", 1100, 680, 20, DARKGRAY);
    } 
    else {
        DrawText("DECISION REQUIRED:", 60, 600, 20, RED);
        DrawText(current->text, 60, 630, 28, WHITE);
        
        DrawText(TextFormat("[1] %s", current->optionA), 60, 670, 20, LIME);
        DrawText(TextFormat("[2] %s", current->optionB), 400, 670, 20, LIME);
    }

    return false; // Still playing
}

int GetLastChoice() {
    return lastChoice;
}