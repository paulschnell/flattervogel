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

    Image icon = LoadImage("assets/bird_single.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    m_pGame = new Game(1000);

    i32 gameScreenHeight = SCREEN_HEIGHT;
    i32 gameScreenWidth = (i32) (GAME_ASPECT_RATIO * gameScreenHeight);
    m_pGame->gameScreen() = {
        GetScreenWidth() / 2 - gameScreenWidth / 2,
        0,
        gameScreenWidth,
        gameScreenHeight,
    };

    m_buttonHandler.addButton({ 10, 10, 130, 45 }, "Reset", buttonReset);
    m_buttonHandler.addButton({ 10, 10 + 50, 170, 45 }, "New Gen", buttonNewGen);
    m_buttonHandler.addButton({ 10, 10 + 100, 280, 45 }, "Play Yourself", buttonTogglePlayer);

    m_buttonHandler.addButton({ 10, (u32) GetScreenHeight() - 65, 110, 45 }, "1000", buttonCyclePopulationSize);
}

void Application::onUpdate(f64 deltaTime) {
    // Update
    m_pGame->onUpdate(deltaTime);

    if (IsWindowResized()) {
        i32 gameScreenHeight = GetScreenHeight();
        i32 gameScreenWidth = (i32) (GAME_ASPECT_RATIO * gameScreenHeight);
        m_pGame->gameScreen() = {
            GetScreenWidth() / 2 - gameScreenWidth / 2,
            0,
            gameScreenWidth,
            gameScreenHeight,
        };
    }

    if (IsKeyPressed(KEY_P)) {
        m_pGame->togglePlayerPlaying();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        m_pGame->reset();
    }

    m_buttonHandler.onUpdate(GetMousePosition().x, GetMousePosition().y, IsMouseButtonDown(MOUSE_BUTTON_LEFT), this);

    // Draw
    BeginDrawing();

    ClearBackground(DARKGRAY);

    m_pGame->onRender();
    m_buttonHandler.onRender();
    DrawText("Next Pop. Size:", 10, (u32) GetScreenHeight() - 65 - 50, 45, LIGHTGRAY);
    DrawText(std::format("({})", m_pGame->getPopulationSize()).c_str(), 130, (u32) GetScreenHeight() - 65, 45, LIGHTGRAY);

    EndDrawing();

    m_isRunning = !WindowShouldClose();
}

void Application::onDestroy() {
    CloseWindow();

    delete m_pGame;
}

void Application::buttonReset(ui::Button& button, Application* pApplication) {
    pApplication->m_pGame->reset();
}

void Application::buttonNewGen(ui::Button& button, Application* pApplication) {
    pApplication->m_pGame->backToFront();
    pApplication->m_pGame->newGeneration();
}

void Application::buttonTogglePlayer(ui::Button& button, Application* pApplication) {
    pApplication->m_pGame->togglePlayerPlaying();
    if (pApplication->m_pGame->isPlayerPlaying())
        button.setText("Play Computer");
    else
        button.setText("Play Yourself");
}

void Application::buttonCyclePopulationSize(ui::Button& button, Application* pApplication) {
    u64 current = pApplication->m_pGame->nextPopulationSize();
    current *= 10;
    if (current > 10000)
        current = 10;

    pApplication->m_pGame->nextPopulationSize() = current;

    button.setText(std::format("{}", current));
}
