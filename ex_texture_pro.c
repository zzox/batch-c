#include "raylib.h"
#include <string.h>

int main()
{
    InitWindow(800, 450, "raylib [core] example - basic window");

    // Texture2D texBunny = LoadTexture(strcat(WORKING_DIR,"resources/icon.png"));
    // change directory
    
    ChangeDirectory("/Users/zzoxnet/codes/cpp/raylib-test/");
    Texture2D texBunny = LoadTexture("resources/icon.png");

    Rectangle source = { 0, 0, 1024, 1024 };
    Rectangle source2 = { 512, 512, 1024, 1024 };

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTexturePro(texBunny, source2, source, (Vector2){ 0, 0 }, 0, WHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

            DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}