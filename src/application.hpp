#pragma once

#include "defines.hpp"
#include "game/game.hpp"
#include "ui/ui_handler.hpp"

constexpr const i32 SCREEN_WIDTH = 750 * 16 / 9;
constexpr const i32 SCREEN_HEIGHT = 750;
constexpr const f32 GAME_ASPECT_RATIO = 0.75;

// TODO: auf einen branch werfen und noch mal von neu

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
    static void buttonCycleMode(ui::Button& button, Application* pApplication);
    static void buttonCyclePopulationSize(ui::Button& button, Application* pApplication);
    static void buttonSaveBestBirdToFile(ui::Button& button, Application* pApplication);

    static void dropdownPresets(ui::Dropdown::Element& newSelected, ui::Dropdown& dropdown, Application* pApplication);
    static void dropdownEmpty(ui::Dropdown& dropdown, Application* pApplication);

private:
    bool m_isRunning = TRUE;

    Game* m_pGame = nullptr;
    ui::UIHandler m_uiHandler;
};
