#pragma once

#include "defines.hpp"
#include "game/game.hpp"
#include "ui/button_handler.hpp"

constexpr const i32 SCREEN_WIDTH = 750 * 16 / 9;
constexpr const i32 SCREEN_HEIGHT = 750;
constexpr const f32 GAME_ASPECT_RATIO = 0.75;

class Application {
public:
    Application() = default;
    ~Application() = default;
    Application(const Application&) = delete;
    Application operator=(const Application&) = delete;

    i32 run() noexcept;

private:
    void onInit();
    void onUpdate(f64 deltaTime);
    void onDestroy();

    static void buttonReset(ui::Button& button, Application* pApplication);
    static void buttonNewGen(ui::Button& button, Application* pApplication);
    static void buttonTogglePlayer(ui::Button& button, Application* pApplication);
    static void buttonCyclePopulationSize(ui::Button& button, Application* pApplication);

private:
    bool m_isRunning = TRUE;

    Game* m_pGame = nullptr;
    ui::ButtonHandler m_buttonHandler;
};
