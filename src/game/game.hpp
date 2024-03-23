#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include "bird.hpp"
#include "pipe.hpp"
#include <vector>

class Game {
public:
    Game();
    Game(utils::Rect<i32> gameScreen);
    ~Game() = default;

    void onUpdate(f64 deltaTime);
    void onRender();
    void birdJump();
    void reset();

    inline utils::Rect<i32>& gameScreen() noexcept { return m_gameScreen; }

private:
    void onInit();

private:
    utils::Rect<i32> m_gameScreen;
    u64 m_bestScore = 0;

    std::vector<Bird> m_birds;

    Pipe m_pipe0;
    Pipe m_pipe1 = Pipe(1.6);
    Pipe* m_pNearestPipe = &m_pipe0;
    u64 m_pipeCounter = 0;
};
