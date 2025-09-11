// needed to use raygui.h
#define RAYGUI_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

#include "raylib.h"
#include "raygui.h"
#include "rlights.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// #if defined(PLATFORM_DESKTOP)
    // #define GLSL_VERSION            330
// #else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            330
// #endif

#define iprint(i) printf("%d\n", i)

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

    const int imgHeight = 32;
    const int imgWidth = 16;

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

    // Load basic lighting shader
    Shader shader = LoadShader(TextFormat("resources/lighting.vs", GLSL_VERSION),
                               TextFormat("resources/lighting.fs", GLSL_VERSION));
    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading,
    // no need to get the location again if using that uniform name
    // shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.5f, 0.5f, 0.5f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Create lights
    Light lights[MAX_LIGHTS] = { 0 };
    // lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -20, 10, -20 }, Vector3Zero(), YELLOW, shader);
    // lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 20, 10, 20 }, Vector3Zero(), RED, shader);
    // lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -20, 10, 20 }, Vector3Zero(), GREEN, shader);
    // lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 20, 10, 20 }, Vector3Zero(), BLUE, shader);

    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -200, 100, -200 }, Vector3Zero(), WHITE, shader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 200, 100, 200 }, Vector3Zero(), WHITE, shader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -200, 100, 200 }, Vector3Zero(), WHITE, shader);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 200, 100, 200 }, Vector3Zero(), WHITE, shader);

    // dont need this if i launch my_app from command line
    // char* dir = "/Users/zzoxnet/codes/cpp/raylib-test/";
    // ChangeDirectory(dir);

    Model model = LoadModel("resources/test-model.obj");             // Load model
    Texture2D texture = LoadTexture("resources/texture.png");        // Load model texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture
    model.materials[0].shader = shader; // the model's materials needs to use the shader to be lit!
    // ^^^ it probably sets the main shader

    Model aModel = LoadModel("resources/robot.glb");

    // Load gltf model animations
    int animsCount = 0;
    unsigned int animIndex = 0;
    unsigned int animCurrentFrame = 0;
    ModelAnimation *modelAnimations = LoadModelAnimations("resources/robot.glb", &animsCount);

    iprint(animsCount);

    Vector3 position = { 0.0f, 0.0f, 0.0f };                    // Set model position
    float rotation = 0.0f;                                      // Set model rotation
    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds
    int angles = 4;

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

        if (IsKeyPressed(KEY_LEFT_BRACKET)) {
            rotation = rotation - 45.0f;
            // printf("%f deg\n", rotation);
        }

        if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
            rotation = rotation + 45.0f;
            // printf("%f deg\n", rotation);
        }

        // Select current animation
        if (IsMouseButtonPressed(KEY_O)) animIndex = (animIndex + 1) % animsCount;
        else if (IsMouseButtonPressed(KEY_P)) animIndex = (animIndex + animsCount - 1) % animsCount;

        const int offsetX = (int)(screenWidth / 2) - (imgWidth / 2);
        const int offsetY = (int)(screenHeight / 2) - (imgHeight / 2);

        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);

        if (IsKeyPressed(KEY_ONE)) { lights[0].enabled = !lights[0].enabled; }
        if (IsKeyPressed(KEY_TWO)) { lights[1].enabled = !lights[1].enabled; }
        if (IsKeyPressed(KEY_THREE)) { lights[2].enabled = !lights[2].enabled; }
        if (IsKeyPressed(KEY_FOUR)) { lights[3].enabled = !lights[3].enabled; }

        // Select model on mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (mode == ANIMATED) {
                Image res = GenImageColor(imgWidth * angles, imgHeight * animsCount * dirs, BLANK);

                int frames = 0;

                for (int i = 0; i < animsCount * dirs; i++) {
                    for (int j = 0; j < angles; j++) {
                        frames++;
                        BeginTextureMode(rt);
                            ClearBackground(BLANK);
                            BeginMode3D(camera);
                            BeginShaderMode(shader);

                            // Update model animation
                            ModelAnimation a = modelAnimations[(int)floor(i / dirs)];
                            UpdateModelAnimation(aModel, a, (int)(a.frameCount / angles * j));

                            // DrawModel(model, position, mScale, WHITE);
                            DrawModelEx(aModel, position, (Vector3){ 0.0f, 1.0f, 0.0f }, (i % dirs) * 360 / dirs, (Vector3){ mScale, mScale, mScale }, WHITE);
                            EndShaderMode();
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
                Image res = GenImageColor(imgWidth * angles, imgHeight, BLANK);
                for (int i = 0; i < angles; i++) {
                    BeginTextureMode(rt);
                        ClearBackground(BLANK);
                        BeginMode3D(camera);
                        BeginShaderMode(shader);

                        // DrawModel(model, position, mScale, WHITE);
                        DrawModelEx(model, position, (Vector3){ 0.0f, 1.0f, 0.0f }, i * 360 / angles, (Vector3){ mScale, mScale, mScale }, WHITE);
                        EndShaderMode();
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
            BeginShaderMode(shader);
                if (mode == STATIC) DrawModelEx(model, position, (Vector3){ 0.0f, 1.0f, 0.0f }, rotation, (Vector3){ mScale, mScale, mScale }, WHITE);
                else DrawModel(aModel, position, mScale, WHITE);
                DrawGrid(20, 10.0f);                       // Draw a grid
            EndShaderMode();

            // Draw spheres to show where the lights are
            for (int i = 0; i < MAX_LIGHTS; i++)
            {
                if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
                else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
            }

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