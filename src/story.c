#include "story.h"
#include "shared.h"

#define MAX_SCRIPT_LINES 100

typedef enum LineType { LINE_DIALOGUE, LINE_CHOICE } LineType;

typedef struct ScriptLine {
    LineType type;
    char speaker[32];
    char text[128];
    Vector3 camPos;
    Vector3 camTarget;
} ScriptLine;

static ScriptLine script[MAX_SCRIPT_LINES];
static int totalLines = 0;
static int currentLineIndex = 0;
static Camera camera = { 0 };

// --- VISUALS ---

// UPDATED: Now draws the "Ghost Tech" model (Cyan Capsule)
void DrawActor(Vector3 pos) {
    // 1. The Body (Cyan Capsule with Fade)
    // We add +0.5 to Y so it floats slightly above the ground grid
    DrawCapsule(
        (Vector3){pos.x, pos.y + 0.5f, pos.z}, 
        (Vector3){pos.x, pos.y + 2.0f, pos.z}, 
        0.4f, 8, 8, 
        Fade(CYAN, 0.8f)
    );
    
    // 2. The Head (Wireframe Visor)
    DrawSphereWires(
        (Vector3){pos.x, pos.y + 1.8f, pos.z}, 
        0.3f, 8, 8, 
        WHITE
    );
}

void DrawCinematicStage() {
    int slices = 20; 
    Color gridColor = Fade(GREEN, 0.1f);
    int halfSlices = slices / 2;
    for (int i = -halfSlices; i <= halfSlices; i++) {
        DrawLine3D((Vector3){(float)i,0,(float)-halfSlices}, (Vector3){(float)i,0,(float)halfSlices}, gridColor);
        DrawLine3D((Vector3){(float)-halfSlices,0,(float)i}, (Vector3){(float)halfSlices,0,(float)i}, gridColor);
    }
}

// --- INIT ---

void LoadScriptFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Script not found: %s\n", filename);
        return;
    }

    char line[256];
    totalLines = 0;

    while (fgets(line, sizeof(line), file) && totalLines < MAX_SCRIPT_LINES) {
        ScriptLine* sl = &script[totalLines];
        sl->type = LINE_DIALOGUE;

        // 1. Speaker
        char* token = strtok(line, "|");
        if (!token) continue;
        strcpy(sl->speaker, token);

        // 2. Text
        token = strtok(NULL, "|");
        if (token) strcpy(sl->text, token);

        // 3. Camera Position
        float cx=0, cy=0, cz=0;
        token = strtok(NULL, "|"); if(token) cx = strtof(token, NULL);
        token = strtok(NULL, "|"); if(token) cy = strtof(token, NULL);
        token = strtok(NULL, "|"); if(token) cz = strtof(token, NULL);
        sl->camPos = (Vector3){cx, cy, cz};

        // 4. Camera Target
        float tx=0, ty=0, tz=0;
        token = strtok(NULL, "|"); if(token) tx = strtof(token, NULL);
        token = strtok(NULL, "|"); if(token) ty = strtof(token, NULL);
        token = strtok(NULL, "|"); if(token) tz = strtof(token, NULL);
        sl->camTarget = (Vector3){tx, ty, tz};

        totalLines++;
    }
    fclose(file);
}

void InitStory(int episodeID) {
    camera.up = (Vector3){0,1,0};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    currentLineIndex = 0;
    
    LoadScriptFromFile("assets/ep1_script.txt");

    if (totalLines > 0) {
        camera.position = script[0].camPos;
        camera.target = script[0].camTarget;
    }
}

bool UpdateDrawStory(GameState *currentState) {
    if (currentLineIndex >= totalLines) return true; 

    ScriptLine *current = &script[currentLineIndex];

    // Logic
    camera.position = Vector3Lerp(camera.position, current->camPos, 0.05f);
    camera.target = Vector3Lerp(camera.target, current->camTarget, 0.1f);

    if (current->type == LINE_DIALOGUE) {
        if (IsKeyPressed(KEY_SPACE)) currentLineIndex++;
    }

    // Draw
    BeginMode3D(camera);
        DrawCinematicStage();
        
        // UPDATED CALL: Removed the Color argument
        DrawActor((Vector3){current->camTarget.x, 0, current->camTarget.z});
        
    EndMode3D();

    // UI
    DrawRectangle(0, 0, 1280, 50, BLACK);
    DrawRectangle(0, 580, 1280, 140, Fade(BLACK, 0.9f));

    DrawText(current->speaker, 60, 600, 20, ORANGE);
    DrawText(current->text, 60, 630, 28, WHITE);
    DrawText("[SPACE] NEXT", 1100, 680, 20, DARKGRAY);

    return false; 
}