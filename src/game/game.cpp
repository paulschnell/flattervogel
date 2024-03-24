#include "game.hpp"

#include "raylib.h"
#include <format>
#include <random>

bool Game::s_firstInit = FALSE;
Texture2D Game::s_textureBackground;

Game::Game(u64 populationSize)
    : m_gameScreen({}), m_populationSize(populationSize) {
    onInit();
}

Game::Game(utils::Rect<i32> gameScreen, u64 populationSize)
    : m_gameScreen(gameScreen), m_populationSize(populationSize) {
    onInit();
}

void Game::onUpdate(f64 deltaTime) {
    if (m_paused) {
        for (auto& bird : m_birds) {
            bird.freefall(deltaTime);
        }
        return;
    }

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

    bool allDead = TRUE;
    for (auto& bird : m_birds) {
        bird.move(deltaTime, m_pNearestPipe);
        if (!bird.isDead()) {
            allDead = FALSE;
            bird.think(m_pNearestPipe->getHoleY(), m_pNearestPipe->getLeftX() - Bird::X_OFFSET);

            if (haveCleared)
                bird.incrScore();
        }
    }

    if (allDead) {
        reset();
        newGeneration();
    }

    m_pipe0.move(deltaTime);
    m_pipe1.move(deltaTime);
}

void Game::onRender() {
    DrawRectangle(m_gameScreen.left, m_gameScreen.top, m_gameScreen.right, m_gameScreen.bottom, BLUE);
    DrawTexturePro(
        s_textureBackground,
        Rectangle(0, 0, s_textureBackground.width, s_textureBackground.height),
        Rectangle(m_gameScreen.left, m_gameScreen.top, m_gameScreen.right, m_gameScreen.bottom),
        Vector2(0, 0),
        0.0,
        WHITE
        );

    m_pipe0.draw(m_gameScreen);
    m_pipe1.draw(m_gameScreen);

    for (const auto& bird : m_birds) {
        bird.draw(m_gameScreen);
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
        std::format("Pipe: {}", m_pipeCounter).c_str(),
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
    DrawText(
        std::format("Gen: {}", m_generationCounter).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.15 * GetScreenHeight() + 0.01 * GetScreenHeight(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);
}

void Game::reset() {
    m_pipe0 = Pipe();
    m_pipe1 = Pipe(1.6);
    m_pNearestPipe = &m_pipe0;
    m_pipeCounter = 0;

    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();
}

void Game::onInit() {
    if (!s_firstInit) {
        s_firstInit = TRUE;
        s_textureBackground = LoadTexture("assets/background.png");
    }

    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();

    for (u32 i = 0; i < m_populationSize; i++) {
        m_birds.emplace_back();
        m_birds.back().createRandomNeuralNetwork();
    }
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

    m_birds.clear();

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
}
