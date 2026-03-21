#pragma once
#include <raylib.h>
#include <string>

#include "../include/net.hpp"

struct HomeState {
    struct Activity {
        bool showing;
        std::string name;
        std::string description;
        std::string state;
    } activity;
    char key[129];
    Font font;
    Texture copyIcon;
};

HomeState* home_init();
void home_loop(HomeState* state);
void home_deinit(HomeState* state);

HomeState * home_init() {
    SetTargetFPS(20);
    HomeState* state = new HomeState();
    state->font = LoadFontEx("system/Roboto_SemiCondensed-Medium.ttf", 40, nullptr, 256);
    SetTextureFilter(state->font.texture, TEXTURE_FILTER_BILINEAR);
    state->copyIcon = LoadTexture("system/copy.png");

    state->activity.showing = false;
    rpcGetVariable("key", state->key, sizeof(state->key));
    return state;
}

void home_packetparser(HomeState* state) {
    NetPacket pck = netReceivePacket();
    if (pck.size <= 0) return;
    auto json = netParsePacket(&pck);
    if (json.contains("activate")) {
        if (json["activate"].get<bool>()) {
            state->activity.showing = true;
            state->activity.name = "";
            state->activity.description = "";
            state->activity.state = "";
        } else {
            state->activity.showing = false;
        }
    } else {
        if (json.contains("activities") && json["activities"].is_array() && json["activities"].size() > 0) {
            auto act = json["activities"][0];
            state->activity.name = act.value("name", "");
            state->activity.description = act.value("details", "");
            state->activity.state = act.value("state", "");
        }
    }
}

void home_loop(HomeState* state) {
    float copyTextShowTime = 0;
    while (!WindowShouldClose()) {
        const int sx = GetScreenWidth(), sy = GetScreenHeight();
        const char* titleTxt = "Your current activity:";
        home_packetparser(state);

        BeginDrawing();
        ClearBackground((Color){38, 38, 38, 255});
        DrawRectangleRounded((Rectangle){(float)(sx/2 - 400), (float)(sy/2 - 100), 800, 200}, 0.5f, 10, (Color){48, 48, 48, 255});
        DrawTextEx(state->font, titleTxt, (Vector2){(float)(sx/2 - MeasureTextEx(state->font, titleTxt, 40, 0).x/2), (float)(sy/2 - 80)}, 40, 0, WHITE);
        if (state->activity.showing) {
            DrawTextEx(state->font, state->activity.name.c_str(), (Vector2){(float)(sx/2 - MeasureTextEx(state->font, state->activity.name.c_str(), 30, 0).x/2), (float)(sy/2 - 20)}, 30, 0, (Color){200, 200, 200, 255});
            DrawTextEx(state->font, state->activity.description.c_str(), (Vector2){(float)(sx/2 - MeasureTextEx(state->font, state->activity.description.c_str(), 20, 0).x/2), (float)(sy/2 + 20)}, 20, 0, (Color){150, 150, 150, 255});
            DrawTextEx(state->font, state->activity.state.c_str(), (Vector2){(float)(sx/2 - MeasureTextEx(state->font, state->activity.state.c_str(), 20, 0).x/2), (float)(sy/2 + 50)}, 20, 0, (Color){150, 150, 150, 255});
        } else {
            const char* noActivityTxt = "No activity detected.";
            DrawTextEx(state->font, noActivityTxt, (Vector2){(float)(sx/2 - MeasureTextEx(state->font, noActivityTxt, 30, 0).x/2), (float)(sy/2)}, 30, 0, (Color){200, 200, 200, 255});
        }

        DrawRectangleRounded((Rectangle){(float)(sx - 210), 10, 200, 50}, 0.5f, 10, (Color){70, 70, 70, 255});
        const char* copyKeyText = "Copy Key";
        DrawTextEx(state->font, copyKeyText, (Vector2){(float)(sx - 110 - MeasureTextEx(state->font, copyKeyText, 20, 0).x/2), 25}, 20, 0, WHITE);
        const int textureW = state->copyIcon.width, textureH = state->copyIcon.height;
        DrawTexture(state->copyIcon, sx - 30 - textureW, 25 + (20 - textureH)/2, RED);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x >= sx - 210 && mousePos.x <= sx - 10 && mousePos.y >= 10 && mousePos.y <= 60) {
                SetClipboardText(state->key);
                copyTextShowTime = 2.0f;
            }
        }

        if (copyTextShowTime > 0) {
            const char* copiedTxt = "Key Copied!";
            DrawRectangleRounded((Rectangle){(float)(sx/2 - 100), (float)(sy - 80), 200, 50}, 0.5f, 10, (Color){70, 70, 70, 255});
            DrawTextEx(state->font, copiedTxt, (Vector2){(float)(sx/2 - MeasureTextEx(state->font, copiedTxt, 20, 0).x/2), (float)(sy - 65)}, 20, 0, WHITE);
            copyTextShowTime -= GetFrameTime();
        }

        EndDrawing();
    }
}

void home_deinit(HomeState* state) {
    UnloadFont(state->font);
    UnloadTexture(state->copyIcon);
    delete state;
}
