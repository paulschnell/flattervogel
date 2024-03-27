#include "game.hpp"

#include "raylib.h"
#include <format>
#include <random>

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

    if (m_playerPlaying) {
        if (!m_playerBird.isDead()) {
            if (IsKeyPressed(KEY_SPACE) || (mouseInsideGameScreen && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
                m_playerBird.jump();
            }

            if (haveCleared)
                m_playerBird.incrScore();
        } else {
            if (m_playerBird.getScore() > m_bestScore)
                m_bestScore = m_playerBird.getScore();

            if (IsKeyPressed(KEY_SPACE) || (mouseInsideGameScreen && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
                m_playerBird.reset();
                backToFront();
            }
        }

        if (m_playerBird.isDead())
            m_playerBird.freefall(deltaTime);
        else
            m_playerBird.move(deltaTime, m_pNearestPipe);

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

    if (!m_playerBird.isDead()) {
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

    if (m_playerPlaying) {
        m_playerBird.draw(gameScreen(), Bird::Type::SINGLE);
    } else {
        for (usize i = 1; i < m_populationSize; i++) {
            m_birds[i].draw(m_gameScreen, Bird::Type::NORMAL);
        }
        m_pBestBird->draw(m_gameScreen, Bird::Type::BEST);
        m_birds[0].draw(m_gameScreen, Bird::Type::LAST_BEST);
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
        m_playerPlaying ? std::format("Score: {}", m_playerBird.getScore()).c_str() : std::format("Pipe: {}", m_pipeCounter).c_str(),
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
    if (!m_playerPlaying) {
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

        m_pBestBird->getBrain().draw(m_gameScreen);
    }
}

void Game::reset() {
    backToFront();

    m_playerBird.reset();
    m_birds.clear();
    m_numAlive = 0;
    m_generationCounter = 1;
    m_pBestBird = nullptr;

    onInit();
}

void Game::backToFront() {
    m_pipe0 = Pipe();
    m_pipe1 = Pipe(1.6);
    m_pNearestPipe = &m_pipe0;
    m_pipeCounter = 0;
    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();
}

bool Game::togglePlayerPlaying() {
    reset();

    m_playerPlaying = !m_playerPlaying;
    return m_playerPlaying;
}

void Game::onInit() {
    if (!s_firstInit) {
        s_firstInit = TRUE;
        s_textureBackground = LoadTexture("assets/background.png");
    }

    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();

    m_birds.reserve(m_populationSize);
    for (u32 i = 0; i < m_populationSize; i++) {
        m_birds.emplace_back();
        m_birds.back().createRandomNeuralNetwork();
    }
    m_pBestBird = &m_birds[0];
    m_numAlive = m_populationSize;
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
