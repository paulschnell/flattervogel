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
   
#ifdef PS_DEBUG
    struct DebugInfo {
        utils::Rect<f64> topPipe = {};
        Vec topNearestPoint = {};
        utils::Rect<f64> botPipe = {};
        Vec botNearestPoint = {};
        bool topNearestPointSet = FALSE;
    };
#endif

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

    #ifdef PS_DEBUG
    inline void toggleRenderCollisionInfo() noexcept { m_renderCollisionInfo = !m_renderCollisionInfo; }
    inline void toggleDebugPause() noexcept { m_debugPause = !m_debugPause; }
    #endif

private:
    void onInit();
    bool collided();
    bool circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim);

private:
    utils::Rect<i32> m_gameScreen;
    bool m_gameOver = FALSE;

    Bird m_bird;

    Pipe m_pipe0;
    Pipe m_pipe1 = Pipe(1.6);

    #ifdef PS_DEBUG
    bool m_debugPause = FALSE;
    bool m_renderCollisionInfo = FALSE;
    DebugInfo m_debugInfoData = {};
    #endif
};
