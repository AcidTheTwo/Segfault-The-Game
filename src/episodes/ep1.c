#include "../shared.h"

// Note: LoadEpisode1 is removed because we now load data from "assets/ep1_level.txt"

// Function: Draw the Room (Environment Only)
void DrawEpisode1Room(void) {
    
    // 1. The Floor (Solid Base)
    DrawCube((Vector3){0, -0.5f, 0}, 20.0f, 1.0f, 20.0f, (Color){15, 15, 20, 255});
    DrawGrid(20, 1.0f); // Overlay grid for "Digital" look
    
    // 2. The Walls (Dark Green Wireframes)
    // Back Wall
    DrawCubeWires((Vector3){0, 2.5f, 6.0f}, 12.0f, 5.0f, 0.0f, DARKGREEN);
    // Left Wall
    DrawCubeWires((Vector3){-6.0f, 2.5f, 0.0f}, 0.0f, 5.0f, 12.0f, DARKGREEN);
    // Right Wall
    DrawCubeWires((Vector3){6.0f, 2.5f, 0.0f}, 0.0f, 5.0f, 12.0f, DARKGREEN);

    // 3. The Furniture (Bright Green Wireframes)
    // Center Table
    DrawCubeWires((Vector3){0, 0.5f, 0}, 3.0f, 1.0f, 2.0f, GREEN);
    
    // The Sofa
    DrawCubeWires((Vector3){-4.0f, 0.5f, 2.0f}, 2.0f, 1.0f, 4.0f, GREEN); // Base
    DrawCubeWires((Vector3){-5.0f, 1.5f, 2.0f}, 0.5f, 2.0f, 4.0f, GREEN); // Backrest

    // TV Stand
    DrawCubeWires((Vector3){0, 0.5f, 5.0f}, 4.0f, 1.0f, 1.0f, GREEN);
}