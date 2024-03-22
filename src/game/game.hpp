#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include "bird.hpp"
#include "pipe.hpp"

class Game {
private:
    struct Vec {
        f64 x;
        f64 y;
    };

public:
    Game();
    Game(utils::Rect<i32> gameScreen);
    ~Game() = default;

    void onUpdate(f64 deltaTime);
    void onRender();
    void birdJump();
    void reset();

    inline utils::Rect<i32>& gameScreen() noexcept { return m_gameScreen; }
    inline bool& gameOver() noexcept { return m_gameOver; }

private:
    void onInit();
    void onGameOver();
    bool collided();
    static bool circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim);

private:
    utils::Rect<i32> m_gameScreen;
    bool m_gameOver = FALSE;
    u64 m_score = 0;
    u64 m_highScore = 0;

    Bird m_bird;

    Pipe m_pipe0;
    Pipe m_pipe1 = Pipe(1.6);
    Pipe* m_pNearestPipe = &m_pipe0;
};
