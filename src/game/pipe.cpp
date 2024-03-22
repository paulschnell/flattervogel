#include "pipe.hpp"

#include <random>
#include "raylib.h"
#include <iostream>

Pipe::Pipe(f64 spawnX)
    : m_leftX(spawnX) {}

void Pipe::move(f64 deltaTime) {
    m_leftX -= SPEED * deltaTime;

    if (m_leftX < 0.0 - WIDTH) {
        m_leftX = 1.0;
        randomHoleY();
    }
}

void Pipe::randomHoleY() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<f64> dist(MIN_PIPE_HEIGHT, 1.0 - MIN_PIPE_HEIGHT);
    m_holeY = dist(rng);
}

void Pipe::draw(const utils::Rect<i32>& gameScreen) const {
    // Lower
    DrawRectangle(
        gameScreen.left + m_leftX * gameScreen.right,
        gameScreen.top + (1.0 - (m_holeY - HOLE_HEIGHT / 2.0)) * gameScreen.bottom,
        WIDTH * gameScreen.right,
        gameScreen.bottom - (1.0 - m_holeY) * gameScreen.bottom,
        GREEN);

    // Upper
    DrawRectangle(
        gameScreen.left + m_leftX * gameScreen.right,
        gameScreen.top,
        WIDTH * gameScreen.right,
        (1.0 - (m_holeY + HOLE_HEIGHT / 2.0)) * gameScreen.bottom,
        GREEN);   
}
