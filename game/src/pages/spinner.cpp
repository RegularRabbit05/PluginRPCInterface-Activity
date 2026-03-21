#pragma once
#include <raylib.h>
#include <cmath>

typedef struct {
    Model spinnerModel;
} SpinnerState;

SpinnerState* spinner_init();
void spinner_loop(SpinnerState* state);
void spinner_deinit(SpinnerState* state);

SpinnerState * spinner_init() {
    SpinnerState* state = new SpinnerState();
    state->spinnerModel = LoadModel("system/logo.glb");

    return state;
}

void spinner_loop(SpinnerState* state) {
    BeginDrawing(); EndDrawing();
    const Camera3D camera = {
        {0, 0, 10},
        {0, 0, 0},
        {0, 1, 0},
        45,
        CAMERA_PERSPECTIVE
    };
    float rotation = 0;
    float spinTimeRemaining = 5.0f;

    while (!WindowShouldClose()) {
        rotation += GetFrameTime() * 100.0;
        spinTimeRemaining -= GetFrameTime();
        if (spinTimeRemaining <= 0) break;
        if (rotation > 360) rotation -= 360;
        int alpha = 255;
        if (spinTimeRemaining < 0.5f) alpha = (int)(255 * spinTimeRemaining * 2);

        BeginDrawing();
        ClearBackground((Color){38, 38, 38, 255});
        BeginMode3D(camera);
        DrawModelEx(state->spinnerModel, (Vector3){0, 0, 0}, (Vector3){0, 1, 0}, rotation, (Vector3){1, 1, 1}, (Color){255, 255, 255, (unsigned char) alpha});
        EndMode3D();
        EndDrawing();
    }

    spinTimeRemaining = 0.5f;

    while (!WindowShouldClose()) {
        spinTimeRemaining -= GetFrameTime();
        if (spinTimeRemaining <= 0) break;
        BeginDrawing();
        ClearBackground((Color){38, 38, 38, 255});
        EndDrawing();
    }
}

void spinner_deinit(SpinnerState* state) {
    UnloadModel(state->spinnerModel);
    delete state;
}