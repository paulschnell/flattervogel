#include "bird.hpp"

#include "raylib.h"
#include "pipe.hpp"
#include <cmath>
#include <random>
#include <iostream>

bool Bird::s_firstInit = FALSE;
Texture2D Bird::s_texture;
Texture2D Bird::s_textureDead;
Texture2D Bird::s_textureBest;

Bird::Bird() {
    if (!s_firstInit) {
        s_firstInit = TRUE;
        s_texture = LoadTexture("assets/bird.png");
        s_textureDead = LoadTexture("assets/bird_dead.png");
        s_textureBest = LoadTexture("assets/bird_best.png");
    }
}

Bird::Bird(const NeuralNetwork& brain)
    : m_brain(brain) {
}

void Bird::move(f64 deltaTime, const Pipe* pNearestPipe) {
    m_speed = speed();

    if (m_dead) {
        m_x -= deltaTime * Pipe::SPEED;
        freefall(deltaTime);
        return;
    }

    m_lastJump += deltaTime;
    if (m_lastJump > 0.5)
        m_lastJump = 0.5;

    m_y += m_speed;

    m_rotation = m_speed * -4000;
    if (m_rotation > 90) m_rotation = 90;
    if (m_rotation < -90) m_rotation = -90;

    if (hasCollided(pNearestPipe)) {
        m_dead = TRUE;
    } else {
        m_survivedTime += deltaTime;
    }
}

void Bird::jump() {
    m_lastJump = -0.5;
}

void Bird::freefall(f64 deltaTime) {
    m_lastJump += deltaTime;
    m_y += m_speed;

    m_rotation = m_speed * -4000;
    if (m_rotation > 90) m_rotation = 90;
    if (m_rotation < -90) m_rotation = -90;

    if (m_y < 7.0 * RADIUS / 10.0) {
        m_y = 7.0 * RADIUS / 10.0;
    }
}

void Bird::draw(const utils::Rect<i32>& gameScreen, bool best) const {
    const Texture2D& tex = best ? s_textureBest : (!m_dead ? s_texture : s_textureDead);
//    f64 scale = gameScreen.right * 0.009;
    f64 scale = gameScreen.right / tex.width * RADIUS * 3;

    DrawTexturePro(
        tex,
        Rectangle(0, 0, tex.width, tex.height),
        Rectangle(gameScreen.left + (m_x) * gameScreen.right,
            gameScreen.top + (1.0 - (m_y)) * gameScreen.bottom,
            tex.width * scale, tex.height * scale),
        Vector2(tex.width * scale / 2.0, tex.height * scale / 2.0),
        m_rotation,
        WHITE);

    /*DrawCircle(
        gameScreen.left + m_x * gameScreen.right,
        gameScreen.top + (1.0 - m_y) * gameScreen.bottom,
        RADIUS * gameScreen.right,
        !m_dead ? YELLOW : GRAY);*/
}

void Bird::incrScore() noexcept {
    m_score++;
}

void Bird::think(f64 holeY, f64 deltaHole) {
    std::vector<f64> input = { m_y, holeY, deltaHole, m_speed };
    f64 out = m_brain.calc(input)[0];
    if (out > 0.5) {
        jump();
    }
}

f64 Bird::speed() const {
    // out < 0 -> down
    // out > 0 -> up

    f64 out = -0.1 * std::pow(m_lastJump, 2);
    if (m_lastJump < 0.0) {
        out *= -0.5;
    }
    return out;
}

bool Bird::hasCollided(const Pipe* pNearestPipe) {
    if (m_y <= 0.0 + Bird::RADIUS) {
        return TRUE;
    }

    // going above screen:
    if (m_y > 1.0 && pNearestPipe->getLeftX() <= Bird::X_OFFSET + Bird::RADIUS) {
        return TRUE;
    }

    Vec bird = {
        Bird::X_OFFSET,
        m_y
    };

    // Top
    Vec topPipe = {
        pNearestPipe->getLeftX(),
        1.0
    };
    Vec topPipeDim = {
        Pipe::WIDTH,
        1.0 - pNearestPipe->getHoleY() - Pipe::HOLE_HEIGHT / 2.0
    };

    // Bottom
    Vec botPipe = {
        pNearestPipe->getLeftX(),
        pNearestPipe->getHoleY() - Pipe::HOLE_HEIGHT / 2.0
    };
    Vec botPipeDim = {
        Pipe::WIDTH,
        pNearestPipe->getHoleY() - Pipe::HOLE_HEIGHT / 2.0
    };

    return circleRectCollision(bird, Bird::RADIUS, topPipe, topPipeDim) || circleRectCollision(bird, Bird::RADIUS, botPipe, botPipeDim);
}

bool Bird::circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim) {
    Vec nearest = circlePos;

    if (rectPos.x > circlePos.x)
        nearest.x = rectPos.x;
    else if (rectPos.x + rectDim.x < circlePos.x)
        nearest.x = rectPos.x + rectDim.x;

    if (rectPos.y < circlePos.y)
        nearest.y = rectPos.y;
    else if (rectPos.y - rectDim.y > circlePos.y)
        nearest.y = rectPos.y - rectDim.y;

    f64 dist = std::sqrt(std::pow(nearest.x - circlePos.x, 2) + std::pow(nearest.y - circlePos.y, 2));

    // 0.01 for "smoother user experience"
    return dist + 0.01 < circleR;
}

// ---------------------------------- AI stuff ----------------------------------
f64 Bird::fitness() const noexcept {
    return (f64) (m_survivedTime);
}

void Bird::createRandomNeuralNetwork() {
    m_brain.addLayer(2, 4);
    m_brain.addLayer(1);
}
// ---------------------------------- AI stuff ----------------------------------
