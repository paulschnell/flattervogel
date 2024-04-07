#include "application.hpp"

#include "raylib.h"
#include <chrono>

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
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
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

    m_uiHandler.addButton({ 10, 10, 130, 45 }, "Reset", buttonReset);
    m_uiHandler.addButton({ 10, 10 + 50, 170, 45 }, "New Gen", buttonNewGen);
    m_uiHandler.addButton({ 10, 10 + 100, 280, 45 }, "Play Computer", buttonCycleMode);

    m_uiHandler.addButton({ 10, (u32) GetScreenHeight() - 65, 110, 45 }, "1000", buttonCyclePopulationSize);

    m_uiHandler.addButton({ 10, 10 + 250, 230, 45 }, "Save Model", buttonSaveBestBirdToFile);

    ui::Dropdown dropdown({ 10, 10 + 200, 250, 45 }, dropdownPresets);
    dropdown.addElement("Empty", dropdownEmpty);
    for (const auto& ele : m_pGame->models()) {
        dropdown.addElement(ele);
    }
    m_uiHandler.addDropdown(dropdown);
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

    m_uiHandler.onUpdate(GetMousePosition().x, GetMousePosition().y, IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsMouseButtonReleased(MOUSE_BUTTON_LEFT), this);

    // Draw
    BeginDrawing();

    ClearBackground(DARKGRAY);

    m_pGame->onRender();

    DrawText("Next Pop. Size:", 10, (u32) GetScreenHeight() - 65 - 50, 45, LIGHTGRAY);
    DrawText(std::format("({})", m_pGame->getPopulationSize()).c_str(), 130, (u32) GetScreenHeight() - 65, 45, LIGHTGRAY);
    DrawText("Load Model:", 10, (u32) 160, 45, LIGHTGRAY);

    m_uiHandler.onRender();

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

void Application::buttonCycleMode(ui::Button& button, Application* pApplication) {
    switch (pApplication->m_pGame->cycleCurrentMode()) {
    case Game::Mode::MANUAL:
        button.setText("Play Yourself");
        break;
    case Game::Mode::TRAINING:
        button.setText("Train Model");
        break;
    case Game::Mode::DISPLAY:
        button.setText("Play Computer");
        break;
    }
}

void Application::buttonCyclePopulationSize(ui::Button& button, Application* pApplication) {
    u64 current = pApplication->m_pGame->nextPopulationSize();
    current *= 10;
    if (current > 10000)
        current = 10;

    pApplication->m_pGame->nextPopulationSize() = current;

    button.setText(std::format("{}", current));
}

void Application::buttonSaveBestBirdToFile(ui::Button& button, Application* pApplication) {
    pApplication->m_pGame->saveBestBirdToFile(pApplication->m_uiHandler.getDropdown(0));
}

void Application::dropdownPresets(ui::Dropdown::Element& newSelected, ui::Dropdown& dropdown, Application* pApplication) {
    pApplication->m_pGame->setBrain(dropdown.getSelected());
}

void Application::dropdownEmpty(ui::Dropdown& dropdown, Application* pApplication) {
}
