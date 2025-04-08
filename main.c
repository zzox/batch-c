// needed to use raygui.h
#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include "raygui.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

enum Mode {
    STATIC = 0,
    ANIMATED
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    const int imgHeight = 64;
    const int imgWidth = 64;

    const int dirs = 4;

    float mScale = 1.0f;

    int savedCount = 180;
    int resNum = 0;

    float val = 0.0;

    enum Mode mode = STATIC;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - models loading");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 50.0f, 50.0f, 50.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 10.0f, 0.0f };     // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
    camera.projection = CAMERA_ORTHOGRAPHIC;

    // dont need this if i launch my_app from command line
    // char* dir = "/Users/zzoxnet/codes/cpp/raylib-test/";
    // ChangeDirectory(dir);

    Model model = LoadModel("resources/castle.obj");                 // Load model
    Texture2D texture = LoadTexture("resources/castle_diffuse.png"); // Load model texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture

    Model aModel = LoadModel("resources/robot.glb");

    // Load gltf model animations
    int animsCount = 0;
    unsigned int animIndex = 0;
    unsigned int animCurrentFrame = 0;
    ModelAnimation *modelAnimations = LoadModelAnimations("resources/robot.glb", &animsCount);

    printf("%d\n", animsCount);

    Vector3 position = { 0.0f, 0.0f, 0.0f };                    // Set model position

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds

    RenderTexture rt = LoadRenderTexture(800, 450);

    DisableCursor();                // Limit cursor to relative movement inside the window

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Load new models/textures on drag&drop
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1) // Only support one file dropped
            {
                if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
                    IsFileExtension(droppedFiles.paths[0], ".gltf") ||
                    IsFileExtension(droppedFiles.paths[0], ".glb") ||
                    IsFileExtension(droppedFiles.paths[0], ".vox") ||
                    IsFileExtension(droppedFiles.paths[0], ".iqm") ||
                    IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
                {
                    UnloadModel(model);                         // Unload previous model
                    model = LoadModel(droppedFiles.paths[0]);   // Load new model
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

                    bounds = GetMeshBoundingBox(model.meshes[0]);

                    // TODO: Move camera position from target enough distance to visualize model properly
                }
                else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
                {
                    // Unload current model texture and load new one
                    UnloadTexture(texture);
                    texture = LoadTexture(droppedFiles.paths[0]);
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                }
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        if (IsKeyPressed(KEY_P)) {
            if (camera.projection == CAMERA_ORTHOGRAPHIC) camera.projection = CAMERA_PERSPECTIVE;
            else camera.projection = CAMERA_ORTHOGRAPHIC;
        }

        if (IsKeyDown(KEY_EQUAL)) mScale *= 1.02f;
        if (IsKeyDown(KEY_MINUS)) mScale *= 0.98f;

        if (IsKeyPressed(KEY_M)) {
            mode = mode == ANIMATED ? STATIC : ANIMATED;
            // printf("%d", animCurrentFrame);
        }

        // Select current animation
        if (IsMouseButtonPressed(KEY_O)) animIndex = (animIndex + 1) % animsCount;
        else if (IsMouseButtonPressed(KEY_P)) animIndex = (animIndex + animsCount - 1) % animsCount;

        const int offsetX = (int)(screenWidth / 2) - (imgWidth / 2);
        const int offsetY = (int)(screenHeight / 2) - (imgHeight / 2);

        // Select model on mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (mode == ANIMATED) {
                Image res = GenImageColor(imgWidth * 8, imgHeight * animsCount * dirs, BLANK);

                int frames = 0;

                for (int i = 0; i < animsCount * dirs; i++) {
                    for (int j = 0; j < 8; j++) {
                        frames++;
                        BeginTextureMode(rt);
                            ClearBackground(BLANK);
                            BeginMode3D(camera);

                            // Update model animation
                            ModelAnimation a = modelAnimations[(int)floor(i / dirs)];
                            UpdateModelAnimation(aModel, a, (int)(a.frameCount / 8 * j));

                            // DrawModel(model, position, mScale, WHITE);
                            DrawModelEx(aModel, position, (Vector3){ 0.0f, 1.0f, 0.0f }, (i % dirs) * 360 / dirs, (Vector3){ mScale, mScale, mScale }, WHITE);
                            EndMode3D();
                        EndTextureMode();

                        Image big = LoadImageFromTexture(rt.texture);
                        ImageFlipVertical(&big);

                        ImageDraw(&res, big, (Rectangle){ offsetX, offsetY, imgWidth, imgHeight }, (Rectangle){ imgWidth * j, imgHeight * i, imgWidth, imgHeight }, WHITE);
                        UnloadImage(big);
                    }
                }

                printf("%i frames exported\n", frames);

                char resText[100] = "results/test-sheet";
                char numText[4];
                sprintf(numText, "%d", resNum++);
                strcat(resText, numText);
                strcat(resText, ".png");

                ExportImage(res, resText);
                UnloadImage(res);
            } else {
                Image res = GenImageColor(imgWidth * 8, imgHeight, BLANK);
                for (int i = 0; i < 8; i++) {
                    BeginTextureMode(rt);
                        ClearBackground(BLANK);
                        BeginMode3D(camera);

                        // DrawModel(model, position, mScale, WHITE);
                        DrawModelEx(model, position, (Vector3){ 0.0f, 1.0f, 0.0f }, i * 360 / 8, (Vector3){ mScale, mScale, mScale }, WHITE);
                        EndMode3D();
                    EndTextureMode();

                    Image big = LoadImageFromTexture(rt.texture);
                    ImageFlipVertical(&big);

                    ImageDraw(&res, big, (Rectangle){ offsetX, offsetY, imgWidth, imgHeight }, (Rectangle){ imgWidth * i, 0, imgWidth, imgHeight }, WHITE);
                    UnloadImage(big);
                }

                char resText[100] = "results/test-";
                char numText[4];
                sprintf(numText, "%d", resNum++);
                strcat(resText, numText);
                strcat(resText, ".png");

                ExportImage(res, resText);
                UnloadImage(res);
            }

            savedCount = 0;
        }
        //----------------------------------------------------------------------------------

        // Update model animation
        ModelAnimation anim = modelAnimations[animIndex];
        animCurrentFrame = (animCurrentFrame + 1) % anim.frameCount;

        UpdateModelAnimation(aModel, anim, animCurrentFrame);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                if (mode == STATIC) DrawModel(model, position, mScale, WHITE); // Draw 3d model with texture
                else DrawModel(aModel, position, mScale, WHITE);
                DrawGrid(20, 10.0f);                       // Draw a grid
            EndMode3D();

            // guide box
            DrawRectangleLines(offsetX, offsetY, imgWidth, imgHeight, MAGENTA);

            DrawText("Drag & drop model to load mesh/texture.", 10, GetScreenHeight() - 20, 10, DARKGRAY);
            if (++savedCount < 180) DrawText("SAVED EXPORT", GetScreenWidth() - 110, 10, 12, BLUE);
            DrawText("(c) Castle 3D model by Alberto Cano", screenWidth - 200, screenHeight - 20, 10, GRAY);

            GuiSlider((Rectangle){10, 30, 100, 16}, "0", "100", &val, 0, 100);

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);     // Unload texture
    UnloadModel(model);         // Unload model
    UnloadRenderTexture(rt);

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}