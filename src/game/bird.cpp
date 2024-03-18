#include "bird.hpp"

#include "raylib.h"

Bird::Bird() {}

void Bird::move(f64 deltaTime) {
    m_lastJump += deltaTime;
    if (m_lastJump > 0.5)
        m_lastJump = 0.5;
    m_birdY += speed();
}

void Bird::jump() {
    m_lastJump = -0.5;
}

f64 Bird::speed() {
    // out < 0 -> down
    // out > 0 -> up

    f64 out = -0.1 * (m_lastJump * m_lastJump);
    if (m_lastJump < 0.0) {
        out *= -0.5;
    }
    return out;
}

void Bird::draw(const utils::Rect<i32>& gameScreen) {
    DrawCircle(
        gameScreen.left + X_OFFSET * gameScreen.right,
        gameScreen.top + (1.0 - m_birdY) * gameScreen.bottom,
        RADIUS * gameScreen.right,
        YELLOW);
}

void Bird::freefall(f64 deltaTime) {
    move(deltaTime);
    if (m_birdY < 7.0 * RADIUS / 10.0) {
        m_birdY = 7.0 * RADIUS / 10.0;
    }
}
