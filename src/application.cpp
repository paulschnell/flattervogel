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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "flattervogel");
    SetTargetFPS(60);

    i32 gameScreenHeight = SCREEN_HEIGHT;
    i32 gameScreenWidth = (i32) (GAME_ASPECT_RATIO * gameScreenHeight);
    m_game.gameScreen() = {
        SCREEN_WIDTH / 2 - gameScreenWidth / 2,
        0,
        gameScreenWidth,
        gameScreenHeight,
    };
}

void Application::onUpdate(f64 deltaTime) {
    // Update
    m_game.onUpdate(deltaTime);

    if (IsKeyPressed(KEY_SPACE)) {
        if (m_game.gameOver()) {
            m_game.reset();
        } else {
            m_game.birdJump();
        }
    }

    #ifdef PS_DEBUG
    if (IsKeyPressed(KEY_F1)) {
        m_game.toggleDebugPause();
    }
    if (IsKeyPressed(KEY_F2)) {
        m_game.toggleRenderCollisionInfo();
    }
    #endif

    // Draw
    BeginDrawing();

    ClearBackground(RAYWHITE);

    m_game.onRender();

    DrawRectangle(0, 0, m_game.gameScreen().left, SCREEN_HEIGHT, RAYWHITE);
    DrawRectangle(
        m_game.gameScreen().left + m_game.gameScreen().right,
        0,
        SCREEN_WIDTH - m_game.gameScreen().right - m_game.gameScreen().left,
        SCREEN_HEIGHT,
        RAYWHITE);

#ifdef PS_DEBUG 
    DrawRectangle(
        m_game.gameScreen().left + m_game.gameScreen().right,
        0,
        20,
        m_game.gameScreen().bottom / 4,
        BLACK);
    DrawRectangle(
        m_game.gameScreen().left + m_game.gameScreen().right,
        m_game.gameScreen().bottom / 4,
        20,
        m_game.gameScreen().bottom / 4,
        YELLOW);
    DrawRectangle(
        m_game.gameScreen().left + m_game.gameScreen().right,
        2 * m_game.gameScreen().bottom / 4,
        20,
        m_game.gameScreen().bottom / 4,
        BLACK);
    DrawRectangle(
        m_game.gameScreen().left + m_game.gameScreen().right,
        3 * m_game.gameScreen().bottom / 4,
        20,
        m_game.gameScreen().bottom / 4,
        YELLOW);
#endif

    EndDrawing();

    m_isRunning = !WindowShouldClose();
}

void Application::onDestroy() {
    CloseWindow();
}
