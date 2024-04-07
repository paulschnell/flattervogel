#include "bird.hpp"

#include "raylib.h"
#include "pipe.hpp"
#include <cmath>

bool Bird::s_firstInit = FALSE;
Texture2D Bird::s_texture;
Texture2D Bird::s_textureDead;
Texture2D Bird::s_textureBest;
Texture2D Bird::s_textureLastBest;
Texture2D Bird::s_textureSingle;

Bird::Bird() {
    if (!s_firstInit) {
        s_firstInit = TRUE;
        s_texture = LoadTexture("assets/bird.png");
        s_textureDead = LoadTexture("assets/bird_dead.png");
        s_textureBest = LoadTexture("assets/bird_best.png");
        s_textureLastBest = LoadTexture("assets/bird_lastbest.png");
        s_textureSingle = LoadTexture("assets/bird_single.png");
    }
}

Bird::Bird(const NeuralNetwork& brain)
    : m_brain(brain) {
}

void Bird::move(f64 deltaTime, const Pipe* pNearestPipe) {
    if (m_dead) {
        m_x -= deltaTime * Pipe::SPEED;
        freefall(deltaTime);
        return;
    }

    fall(deltaTime);

    m_rotation = m_speed * -4000;
    if (m_rotation > 90)
        m_rotation = 90;
    if (m_rotation < -90)
        m_rotation = -90;

    if (hasCollided(pNearestPipe)) {
        m_dead = TRUE;
    } else {
        m_survivedTime += deltaTime;
    }
}

void Bird::jump() {
    m_speed = JUMP_STRENGTH;
}

void Bird::freefall(f64 deltaTime) {
    fall(deltaTime);

    m_rotation = m_speed * -4000;
    if (m_rotation > 90)
        m_rotation = 90;
    if (m_rotation < -90)
        m_rotation = -90;

    if (m_y < 7.0 * RADIUS / 10.0) {
        m_y = 7.0 * RADIUS / 10.0;
    }
}

void Bird::draw(const utils::Rect<i32>& gameScreen, Type type) const {
    if (m_dead)
        type = Type::DEAD;

    Texture2D* pTex;
    switch (type) {
    case Type::NORMAL:
        pTex = &s_texture;
        break;
    case Type::DEAD:
        pTex = &s_textureDead;
        break;
    case Type::LAST_BEST:
        pTex = &s_textureLastBest;
        break;
    case Type::BEST:
        pTex = &s_textureBest;
        break;
    case Type::SINGLE:
        pTex = &s_textureSingle;
        break;
    }
    f64 scale = (f32) gameScreen.right / pTex->width * RADIUS * 3;

    DrawTexturePro(
        *pTex,
        Rectangle(0, 0, pTex->width, pTex->height),
        Rectangle(gameScreen.left + (m_x) *gameScreen.right,
            gameScreen.top + (1.0 - (m_y + 0.008)) * gameScreen.bottom,
            pTex->width * scale, pTex->height * scale),
        Vector2(pTex->width * scale / 2.0, pTex->height * scale / 2.0),
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

void Bird::reset() {
    m_y = 0.5;
    m_speed = 0.0;
    m_x = X_OFFSET;
    m_score = 0;
    m_dead = FALSE;
    m_rotation = 0.0;
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

void Bird::fall(f64 deltaTime) {
    m_speed += ACCELERATION * deltaTime;
    if (std::abs(m_speed) > MAX_SPEED * deltaTime)
        m_speed = MAX_SPEED * (m_speed > 0 ? 1.0 : -1.0) * deltaTime;
    m_y += m_speed;
}

// ---------------------------------- AI stuff ----------------------------------
void Bird::think(f64 holeY, f64 deltaHole) {
    std::vector<f64> input = { m_y, holeY, deltaHole, m_speed };
    f64 out = m_brain.calc(input)[0];
    if (out > 0.5) {
        jump();
    }
}

void Bird::createRandomNeuralNetwork() {
    m_brain = NeuralNetwork();
    // medium
    m_brain.addLayer(2, 4);
    m_brain.addLayer(1);

    // small
    // m_brain.addLayer(1, 4);

    // large
    // m_brain.addLayer(4, 4);
    // m_brain.addLayer(2);
    // m_brain.addLayer(1);
}

void Bird::getBrainFromFile(std::string_view file) {
    m_brain = NeuralNetwork(file);
}

f64 Bird::fitness() const noexcept {
    return (f64) (m_survivedTime);
}
// ---------------------------------- AI stuff ----------------------------------
