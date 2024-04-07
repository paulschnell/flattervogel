#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include "bird.hpp"
#include "pipe.hpp"
#include "ui/dropdown.hpp"
#include <vector>
#include <string>

class Game {
public:
    enum class Mode {
        TRAINING, // train a model (edits NN)
        DISPLAY, // let a model play (does not edit NN)
        MANUAL // play yourself (no NN)
    };

public:
    Game(u64 populationSize);
    Game(utils::Rect<i32> gameScreen, u64 populationSize);
    ~Game() = default;

    void onUpdate(f64 deltaTime);
    void onRender();
    void reset();
    void backToFront();

    Mode cycleCurrentMode();
    void saveBestBirdToFile(ui::Dropdown& dropdown);
    void setBrain(i32 id);

    inline utils::Rect<i32>& gameScreen() noexcept { return m_gameScreen; }
    inline const std::vector<std::string>& models() const noexcept { return m_models; }
    inline Mode getCurrentMode() const noexcept { return m_mode; }
    inline u64 getPopulationSize() const noexcept { return m_populationSize; }
    inline u64& nextPopulationSize() noexcept { return m_nextPopulationSize; }

    // ---------------------------------- AI stuff ----------------------------------
    void newGeneration();
    // ---------------------------------- AI stuff ----------------------------------

private:
    void onInit();

private:
    utils::Rect<i32> m_gameScreen;
    u64 m_bestScore = 0;

    std::vector<std::string> m_models;
    i32 m_id = -1;

    Mode m_mode = Mode::DISPLAY;
    Bird m_singleBird;

    u64 m_numAlive;
    u64 m_generationCounter = 1;
    u64 m_populationSize;
    u64 m_nextPopulationSize;
    Bird* m_pBestBird = nullptr;
    std::vector<Bird> m_birds; // Population

    Pipe m_pipe0;
    Pipe m_pipe1 = Pipe(1.6);
    Pipe* m_pNearestPipe = &m_pipe0;
    u64 m_pipeCounter = 0;

    static bool s_firstInit;
    static Texture2D s_textureBackground;
};
