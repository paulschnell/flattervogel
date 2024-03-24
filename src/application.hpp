#pragma once

#include "defines.hpp"
#include "game/game.hpp"

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

private:
    bool m_isRunning = TRUE;

    Game* m_pGame = NULL;
};
