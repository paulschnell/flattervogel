#include "game.hpp"

#include "raylib.h"
#include <format>

Game::Game()
    : m_gameScreen({}) {
    onInit();
}

Game::Game(utils::Rect<i32> gameScreen)
    : m_gameScreen(gameScreen) {
    onInit();
}

void Game::onUpdate(f64 deltaTime) {
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

    for (auto& bird : m_birds) {
        bird.move(deltaTime, m_pNearestPipe);
        if (!bird.isDead() && haveCleared)
            bird.incrScore();
    }

    m_pipe0.move(deltaTime);
    m_pipe1.move(deltaTime);
}

void Game::onRender() {
    DrawRectangle(m_gameScreen.left, m_gameScreen.top, m_gameScreen.right, m_gameScreen.bottom, BLUE);

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

    // Draw Score
    DrawText(
        std::format("Pipe: {}", m_pipeCounter).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.01 * GetScreenWidth(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);
    DrawText(
        std::format("Best Score: {}", m_bestScore).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.075 * GetScreenHeight() + 0.01 * GetScreenHeight(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);
}

void Game::reset() {
    m_pipe0 = Pipe();
    m_pipe1 = Pipe(1.6);
    m_pNearestPipe = &m_pipe0;

    onInit();
}

void Game::onInit() {
    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();

    for (u32 i = 0; i < 20; i++) {
        m_birds.emplace_back();
    }
}
