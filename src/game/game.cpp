#include "game.hpp"

#include "raylib.h"
#include <cmath>
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
#ifdef PS_DEBUG
    if (!m_debugPause) {
#endif

        if (collided()) {
            onGameOver();
        }

        if (!m_gameOver) {
            m_bird.move(deltaTime);

            m_pipe0.move(deltaTime);
            m_pipe1.move(deltaTime);
        } else {
            m_bird.freefall(deltaTime);
        }

#ifdef PS_DEBUG
    }
#endif
}

void Game::onRender() {
    DrawRectangle(m_gameScreen.left, m_gameScreen.top, m_gameScreen.right, m_gameScreen.bottom, BLUE);

    m_pipe0.draw(m_gameScreen);
    m_pipe1.draw(m_gameScreen);

    m_bird.draw(m_gameScreen);

    // Clear left and right side of screen
    DrawRectangle(0, 0, m_gameScreen.left, GetScreenHeight(), RAYWHITE);
    DrawRectangle(
        m_gameScreen.left +m_gameScreen.right,
        0,
        GetScreenWidth() - m_gameScreen.right - m_gameScreen.left,
        GetScreenHeight(),
        RAYWHITE);

    // Draw Score
    DrawText(
        std::format("Score: {}", m_score).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.01 * GetScreenWidth(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);
    DrawText(
        std::format("Highscore: {}", m_highScore).c_str(),
        m_gameScreen.left + m_gameScreen.right + 0.01 * GetScreenWidth(),
        m_gameScreen.top + 0.075 * GetScreenHeight() + 0.01 * GetScreenHeight(),
        0.075 * GetScreenHeight(),
        LIGHTGRAY);

#ifdef PS_DEBUG
    if (m_renderCollisionInfo) {
        // Pipe
        DrawRectangleLines(
            m_gameScreen.left + m_debugInfoData.topPipe.left * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_debugInfoData.topPipe.top) * m_gameScreen.bottom,
            m_debugInfoData.topPipe.right * m_gameScreen.right,
            m_debugInfoData.topPipe.bottom * m_gameScreen.bottom,
            MAGENTA);

        DrawRectangleLines(
            m_gameScreen.left + m_debugInfoData.botPipe.left * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_debugInfoData.botPipe.top) * m_gameScreen.bottom,
            m_debugInfoData.botPipe.right * m_gameScreen.right,
            m_debugInfoData.botPipe.bottom * m_gameScreen.bottom,
            MAGENTA);

        // Nearest Point
        DrawCircle(
            m_gameScreen.left + m_debugInfoData.topNearestPoint.x * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_debugInfoData.topNearestPoint.y) * m_gameScreen.bottom,
            5,
            ORANGE);

        DrawCircle(
            m_gameScreen.left + m_debugInfoData.botNearestPoint.x * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_debugInfoData.botNearestPoint.y) * m_gameScreen.bottom,
            5,
            ORANGE);

        // Distance to Nearest Point
        DrawLine(
            m_gameScreen.left + Bird::X_OFFSET * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_bird.getY()) * m_gameScreen.bottom,
            m_gameScreen.left + m_debugInfoData.topNearestPoint.x * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_debugInfoData.topNearestPoint.y) * m_gameScreen.bottom,
            ORANGE);

        DrawLine(
            m_gameScreen.left + Bird::X_OFFSET * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_bird.getY()) * m_gameScreen.bottom,
            m_gameScreen.left + m_debugInfoData.botNearestPoint.x * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_debugInfoData.botNearestPoint.y) * m_gameScreen.bottom,
            ORANGE);

        // Distance to Ground
        DrawLine(
            m_gameScreen.left + Bird::X_OFFSET * m_gameScreen.right,
            m_gameScreen.top + (1.0 - m_bird.getY()) * m_gameScreen.bottom,
            m_gameScreen.left + Bird::X_OFFSET * m_gameScreen.right,
            m_gameScreen.top + m_gameScreen.bottom,
            ORANGE);

        // Height
        DrawRectangle(
            m_gameScreen.left + m_gameScreen.right,
            0,
            10,
            m_gameScreen.bottom / 4,
            BLACK);
        DrawRectangle(
            m_gameScreen.left + m_gameScreen.right,
            m_gameScreen.bottom / 4,
            10,
            m_gameScreen.bottom / 4,
            YELLOW);
        DrawRectangle(
            m_gameScreen.left + m_gameScreen.right,
            2 * m_gameScreen.bottom / 4,
            10,
            m_gameScreen.bottom / 4,
            BLACK);
        DrawRectangle(
            m_gameScreen.left + m_gameScreen.right,
            3 * m_gameScreen.bottom / 4,
            10,
            m_gameScreen.bottom / 4,
            YELLOW);
    }
#endif
}

void Game::birdJump() {
    m_bird.jump();
}

void Game::reset() {
    m_gameOver = FALSE;
    m_score = 0;
    m_bird = Bird();
    m_pipe0 = Pipe();
    m_pipe1 = Pipe(1.6);
    m_pNearestPipe = &m_pipe0;

    onInit();
}

void Game::onInit() {
    m_pipe0.randomHoleY();
    m_pipe1.randomHoleY();
}

void Game::onGameOver() {
    m_gameOver = TRUE;
    if (m_score > m_highScore) {
        m_highScore = m_score;
    }
}

bool Game::collided() {
    if (m_bird.getY() <= 0.0 + Bird::RADIUS) {
        return TRUE;
    }

    // switch to nearest pipe (if last ist cleared)
    if (m_pNearestPipe->getLeftX() + Pipe::WIDTH < Bird::X_OFFSET - Bird::RADIUS) {
        if (m_pNearestPipe == &m_pipe0)
            m_pNearestPipe = &m_pipe1;
        else
            m_pNearestPipe = &m_pipe0;

        m_score += 1;
    }

    // going above screen:
    if (m_bird.getY() > 1.0 && m_pNearestPipe->getLeftX() <= Bird::X_OFFSET + Bird::RADIUS) {
        return TRUE;
    }

    Vec bird = {
        Bird::X_OFFSET,
        m_bird.getY()
    };

    // Top
    Vec topPipe = {
        m_pNearestPipe->getLeftX(),
        1.0
    };
    Vec topPipeDim = {
        Pipe::WIDTH,
        1.0 - m_pNearestPipe->getHoleY() - Pipe::HOLE_HEIGHT / 2.0
    };

    // Bottom
    Vec botPipe = {
        m_pNearestPipe->getLeftX(),
        m_pNearestPipe->getHoleY() - Pipe::HOLE_HEIGHT / 2.0
    };
    Vec botPipeDim = {
        Pipe::WIDTH,
        m_pNearestPipe->getHoleY() - Pipe::HOLE_HEIGHT / 2.0
    };

#ifdef PS_DEBUG
    if (m_renderCollisionInfo) {
        m_debugInfoData.topPipe = { topPipe.x, topPipe.y, topPipeDim.x, topPipeDim.y };
        m_debugInfoData.botPipe = { botPipe.x, botPipe.y, botPipeDim.x, botPipeDim.y };
    }
#endif

    return circleRectCollision(bird, Bird::RADIUS, topPipe, topPipeDim) || circleRectCollision(bird, Bird::RADIUS, botPipe, botPipeDim);
}

bool Game::circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim) {
    Vec nearest = circlePos;

    if (rectPos.x > circlePos.x)
        nearest.x = rectPos.x;
    else if (rectPos.x + rectDim.x < circlePos.x)
        nearest.x = rectPos.x + rectDim.x;

    if (rectPos.y < circlePos.y)
        nearest.y = rectPos.y;
    else if (rectPos.y - rectDim.y > circlePos.y)
        nearest.y = rectPos.y - rectDim.y;

#ifdef PS_DEBUG
    if (!m_debugInfoData.topNearestPointSet) {
        m_debugInfoData.topNearestPoint = nearest;
        m_debugInfoData.topNearestPointSet = TRUE;
    } else {
        m_debugInfoData.botNearestPoint = nearest;
        m_debugInfoData.topNearestPointSet = FALSE;
    }
#endif

    f64 dist = std::sqrt(std::pow(nearest.x - circlePos.x, 2) + std::pow(nearest.y - circlePos.y, 2));

    // 0.01 for "smoother user experience"
    return dist + 0.01 < circleR;
}
