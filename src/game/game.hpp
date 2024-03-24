#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include "bird.hpp"
#include "pipe.hpp"
#include <vector>

class Game {
public:
    Game(u64 populationSize);
    Game(utils::Rect<i32> gameScreen, u64 populationSize);
    ~Game() = default;

    void onUpdate(f64 deltaTime);
    void onRender();
    void reset();

    inline utils::Rect<i32>& gameScreen() noexcept { return m_gameScreen; }

    // ---------------------------------- AI stuff ----------------------------------
    void newGeneration();
    // ---------------------------------- AI stuff ----------------------------------

private:
    void onInit();

private:
    utils::Rect<i32> m_gameScreen;
    u64 m_bestScore = 0;
    bool m_paused = FALSE;

    u64 m_numAlive;
    u64 m_generationCounter = 1;
    u64 m_populationSize;
    Bird* m_pBestBird = nullptr;
    std::vector<Bird> m_birds; // Population

    Pipe m_pipe0;
    Pipe m_pipe1 = Pipe(1.6);
    Pipe* m_pNearestPipe = &m_pipe0;
    u64 m_pipeCounter = 0;

    static bool s_firstInit;
    static Texture2D s_textureBackground;
};
