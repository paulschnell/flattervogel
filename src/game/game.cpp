#include "game.hpp"

#include "raylib.h"
#include <format>
#include <random>
#include <filesystem>

bool Game::s_firstInit = FALSE;
Texture2D Game::s_textureBackground;

Game::Game(u64 populationSize)
    : m_gameScreen({}), m_populationSize(populationSize), m_nextPopulationSize(populationSize) {
    onInit();
}

Game::Game(utils::Rect<i32> gameScreen, u64 populationSize)
    : m_gameScreen(gameScreen), m_populationSize(populationSize) {
    onInit();
}

void Game::onUpdate(f64 deltaTime) {
    Vector2 mousePos = GetMousePosition();
    bool mouseInsideGameScreen =
        mousePos.x > m_gameScreen.left && mousePos.x < m_gameScreen.left + m_gameScreen.right &&
        mousePos.y > m_gameScreen.top && mousePos.y < m_gameScreen.top + m_gameScreen.bottom;

    bool haveCleared = FALSE;
    // switch to nearest pipe (if last ist cleared)
    if (m_pNearestPipe->getLeftX() + Pipe::WIDTH < Bird::X_OFFSET - Bird::RADIUS) {
        if (m_pNearestPipe == &m_pipe0)
            m_pNearestPipe = &m_pipe1;
        else
            m_pNearestPipe = &m_pipe0;

        haveCleared = TRUE;
        m_pipeCounter++;
    }

    if (m_mode != Mode::TRAINING) {
        if (!m_singleBird.isDead()) {
            if (m_mode == Mode::MANUAL) {
                if (IsKeyPressed(KEY_SPACE) || (mouseInsideGameScreen && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
                    m_singleBird.jump();
                }
            } else {
                m_singleBird.think(m_pNearestPipe->getHoleY(), m_pNearestPipe->getLeftX() - Bird::X_OFFSET);
            }

            if (haveCleared)
                m_singleBird.incrScore();

            m_singleBird.move(deltaTime, m_pNearestPipe);
        } else {
            if (m_singleBird.getScore() > m_bestScore)
                m_bestScore = m_singleBird.getScore();

            if (IsKeyPressed(KEY_SPACE) || (mouseInsideGameScreen && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
                m_singleBird.reset();
                backToFront();
            }

            m_singleBird.freefall(deltaTime);
        }

    } else {
        bool allDead = TRUE;
        for (auto& bird : m_birds) {
            bool deadBefore = bird.isDead();

            bird.move(deltaTime, m_pNearestPipe);
            if (!bird.isDead()) {
                allDead = FALSE;
                bird.think(m_pNearestPipe->getHoleY(), m_pNearestPipe->getLeftX() - Bird::X_OFFSET);

                if (haveCleared)
                    bird.incrScore();

                if (bird.fitness() > m_pBestBird->fitness())
                    m_pBestBird = &bird;
            } else if (!deadBefore) {
                m_numAlive--;
            }
        }

        if (allDead) {
            newGeneration();
            backToFront();
        }
    }

    if (!m_singleBird.isDead()) {
        m_pipe0.move(deltaTime);
        m_pipe1.move(deltaTime);
    }
}

void Game::onRender() {
    DrawRectangle(m_gameScreen.left, m_gameScreen.top, m_gameScreen.right, m_gameScreen.bottom, BLUE);
    DrawTexturePro(
        s_textureBackground,
        Rectangle(0, 0, s_textureBackground.width, s_textureBackground.height),
        Rectangle(m_gameScreen.left, m_gameScreen.top, m_gameScreen.right, m_gameScreen.bottom),
        Vector2(0, 0),
        0.0,
        WHITE);

    m_pipe0.draw(m_gameScreen);
    m_pipe1.draw(m_gameScreen);

    if (m_mode == Mode::TRAINING) {
        for (usize i = 1; i < m_populationSize; i++) {
            m_birds[i].draw(m_gameScreen, Bird::Type::NORMAL);
        }
        m_pBestBird->draw(m_gameScreen, Bird::Type::BEST);
        m_birds[0].draw(m_gameScreen, Bird::Type::LAST_BEST);
    } else {
        m_singleBird.draw(gameScreen(), Bird::Type::SINGLE);
    }

    // Clear left and right side of screen
    DrawRectangle(0, 0, m_gameScreen.left, GetScreenHeight(), RAYWHITE);
    DrawRectangle(
        m_gameScreen.left + m_gameScreen.right,
        0,
        GetScreenWidth() - m_gameScreen.right - m_gameScreen.left,
        GetScreenHeight(),
        RAYWHITE);

    // Draw Info
    DrawText(
        m_mode != Mode::TRAINING ? std::format("Score: {}", m_singleBird.getScore()).c_str() : std::format("Pipe: {}", m_pipeCounter).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.01 * GetScreenWidth(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);
    DrawText(
        std::format("Best: {}", m_bestScore).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.075 * GetScreenHeight() + 0.01 * GetScreenHeight(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);
    if (m_mode == Mode::TRAINING) {
        DrawText(
            std::format("Gen: {}", m_generationCounter).c_str(),
            m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
            m_gameScreen.top + 0.15 * GetScreenHeight() + 0.01 * GetScreenHeight(),
            0.075 * GetScreenHeight(),
            LIGHTGRAY);
        DrawText(
            std::format("Alive: {}", m_numAlive).c_str(),
            m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
            m_gameScreen.top + 0.225 * GetScreenHeight() + 0.01 * GetScreenHeight(),
            0.075 * GetScreenHeight(),
            LIGHTGRAY);
    }
    if (m_mode == Mode::TRAINING) {
        m_pBestBird->getBrain().draw(m_gameScreen);
    } else if (m_mode == Mode::DISPLAY) {
        m_singleBird.getBrain().draw(m_gameScreen);
    }
}

void Game::reset() {
    backToFront();

    m_singleBird.reset();
    m_birds.clear();
    m_numAlive = 0;
    m_generationCounter = 1;
    m_pBestBird = nullptr;

    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();

    if (m_mode == Mode::TRAINING) {
        std::random_device dev;
        std::mt19937 rng(dev());

        m_birds.reserve(m_populationSize);
        m_birds.emplace_back(m_singleBird.getBrain());
        for (u32 i = 1; i < m_populationSize; i++) {
            if (m_id != -1) {
                NeuralNetwork brain = m_singleBird.getBrain();
                brain.mutate(0.5, 0.2, rng);
                m_birds.emplace_back(brain);
            } else {
                m_birds.emplace_back();
                m_birds.back().createRandomNeuralNetwork();
            }
        }
        m_pBestBird = &m_birds[0];
        m_numAlive = m_populationSize;
    }
}

void Game::backToFront() {
    m_pipe0 = Pipe();
    m_pipe1 = Pipe(1.6);
    m_pNearestPipe = &m_pipe0;
    m_pipeCounter = 0;
    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();
}

Game::Mode Game::cycleCurrentMode() {
    switch (m_mode) {
    case Mode::MANUAL:
        m_mode = Mode::TRAINING;
        break;
    case Mode::TRAINING:
        m_mode = Mode::DISPLAY;

        m_singleBird = Bird(m_pBestBird->getBrain());
        break;
    case Mode::DISPLAY:
        m_mode = Mode::MANUAL;
        break;
    }

    reset();
    return m_mode;
}

void Game::saveBestBirdToFile(ui::Dropdown& dropdown) {
    Bird& bird = m_mode == Mode::TRAINING ? *m_pBestBird : m_singleBird;
    bird.getBrain().serialize(std::format("presets/{}.json", m_id != -1 ? m_models[m_id] : "New Model"));
    if (m_id == -1) {
        m_models.push_back("New Model");
        dropdown.addElement("New Model");
    }
}

void Game::setBrain(i32 id) {
    m_id = id - 1;

    if (m_id == -1) {
        m_singleBird.createRandomNeuralNetwork();
    } else {
        m_singleBird.getBrainFromFile(std::format("presets/{}.json", m_models[m_id]).c_str());
    }
    reset();
}

void Game::onInit() {
    if (!s_firstInit) {
        s_firstInit = TRUE;
        s_textureBackground = LoadTexture("assets/background.png");

        // find every model in presets/*.json
        for (const auto& entry : std::filesystem::directory_iterator("presets")) {
            if (entry.is_regular_file() && entry.path().extension() == ".json" && entry.path().stem().string() != "sample") {
                m_models.emplace_back(entry.path().stem().string());
            }
        }
    }

    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();

    m_numAlive = m_populationSize;

    setBrain(0);
}

void Game::newGeneration() {
    m_generationCounter++;

    Bird bird0 = m_birds[0];
    Bird bird1 = m_birds[0];
    for (const auto& bird : m_birds) {
        if (bird.getScore() > m_bestScore)
            m_bestScore = bird.getScore();

        if (bird.fitness() > bird0.fitness()) {
            bird1 = bird0;
            bird0 = bird;
        } else if (bird.fitness() > bird1.fitness()) {
            bird1 = bird;
        }
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    NeuralNetwork nn(bird0.getBrain(), bird1.getBrain(), rng);

    m_populationSize = m_nextPopulationSize;
    m_birds.clear();
    m_birds.reserve(m_populationSize);

    // Parents and child are pushed to next generation
    m_birds.emplace_back(bird0.getBrain());
    m_birds.emplace_back(bird1.getBrain());
    m_birds.emplace_back(nn);

    for (usize i = 3; i < m_populationSize; i++) {
        NeuralNetwork brain = nn;
        std::mt19937 rng1(dev());
        brain.mutate(0.5, 0.2, rng1);
        m_birds.emplace_back(brain);
    }

    m_pBestBird = &m_birds[0];
    m_numAlive = m_populationSize;
}
