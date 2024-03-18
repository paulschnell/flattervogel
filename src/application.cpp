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
    // Don't Ask Why Just Press Enter
    if (IsKeyPressed(KEY_ENTER)) {
        m_supersecretmode = !m_supersecretmode;
        if (d != m_game.gameScreen().left / 2.0) {
            m_game.gameScreen().left = GetScreenWidth() / 2 - m_game.gameScreen().right / 2;
        }
        d = m_game.gameScreen().left / 2.0;
        e = 1.0;
    }
    if (m_supersecretmode) {
        d += e * f;
        m_game.gameScreen().left = d;
        if (m_game.gameScreen().left + m_game.gameScreen().right >= GetScreenWidth()) {
            e = -1.0;
        } else if (m_game.gameScreen().left <= 0) {
            e = 1.0;
        }

        if (IsKeyDown(KEY_UP)) {
            f += IsKeyDown(KEY_LEFT_SHIFT) ? 1 : 0.5;
        }
        if (IsKeyDown(KEY_DOWN)) {
            f -= IsKeyDown(KEY_LEFT_SHIFT) ? 1 : 0.5;
        }
    }

    // Update
    m_game.onUpdate(deltaTime);

    if (IsWindowResized()) {
        i32 gameScreenHeight = GetScreenHeight();
        i32 gameScreenWidth = (i32) (GAME_ASPECT_RATIO * gameScreenHeight);
        m_game.gameScreen() = {
            GetScreenWidth() / 2 - gameScreenWidth / 2,
            0,
            gameScreenWidth,
            gameScreenHeight,
        };
    }

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

    ClearBackground(DARKGRAY);

    m_game.onRender();

    EndDrawing();

    m_isRunning = !WindowShouldClose();
}

void Application::onDestroy() {
    CloseWindow();
}
