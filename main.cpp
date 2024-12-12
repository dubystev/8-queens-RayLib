#include <iostream>
#include "raylib.h"

int main()
{
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "My First Raylib Project");

    // Set the FPS (frames per second)
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update the game (handle input, update logic, etc.)

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircle(screenWidth / 2, screenHeight / 2, 50, BLUE); // Draw a circle
        EndDrawing(); // End drawing
    }

    // Close the window and OpenGL context
    CloseWindow();
    return 0;
}
