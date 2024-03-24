#include "application.hpp"

#include <chrono>
#include "raylib.h"

i32 Application::run() noexcept {
    onInit();

    auto prevTime = std::chrono::system_clock::now();
    while (m_isRunning) {
        auto curTime = std::chrono::system_clock::now();
        std::chrono::duration<f64> diff = curTime - prevTime;
        prevTime = curTime;

        onUpdate(diff.count());
    }

    onDestroy();
    return 0;
}

void Application::onInit() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "flattervogel");
    SetTargetFPS(60);

    m_pGame = new Game(10000);

    i32 gameScreenHeight = SCREEN_HEIGHT;
    i32 gameScreenWidth = (i32) (GAME_ASPECT_RATIO * gameScreenHeight);
    m_pGame->gameScreen() = {
        0 /*GetScreenWidth() / 2 - gameScreenWidth / 2*/,
        0,
        gameScreenWidth,
        gameScreenHeight,
    };
}

void Application::onUpdate(f64 deltaTime) {
    // Update
    m_pGame->onUpdate(deltaTime);

    if (IsWindowResized()) {
        i32 gameScreenHeight = GetScreenHeight();
        i32 gameScreenWidth = (i32) (GAME_ASPECT_RATIO * gameScreenHeight);
        m_pGame->gameScreen() = {
            0 /*GetScreenWidth() / 2 - gameScreenWidth / 2*/,
            0,
            gameScreenWidth,
            gameScreenHeight,
        };
    }

    if (IsKeyPressed(KEY_SPACE)) {

    }

    // Draw
    BeginDrawing();

    ClearBackground(DARKGRAY);

    m_pGame->onRender();

    EndDrawing();

    m_isRunning = !WindowShouldClose();
}

void Application::onDestroy() {
    CloseWindow();

    delete m_pGame;
}