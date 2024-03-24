#include "pipe.hpp"

#include <random>
#include "raylib.h"
#include <iostream>

bool Pipe::s_firstInit = FALSE;
Texture2D Pipe::s_textureSeg;
Texture2D Pipe::s_textureTop;

Pipe::Pipe() {
    if (!s_firstInit) {
        s_firstInit = TRUE;
        s_textureSeg = LoadTexture("assets/pipe_segment.png");
        s_textureTop = LoadTexture("assets/pipe_top.png");
    }
}

Pipe::Pipe(f64 spawnX)
    : m_leftX(spawnX) {
}

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
    /*DrawRectangle(
        gameScreen.left + m_leftX * gameScreen.right,
        gameScreen.top + (1.0 - (m_holeY - HOLE_HEIGHT / 2.0)) * gameScreen.bottom,
        WIDTH * gameScreen.right,
        gameScreen.bottom - (1.0 - m_holeY) * gameScreen.bottom,
        GREEN);*/

    DrawTexturePro(
        s_textureSeg,
        Rectangle(0, 0, s_textureSeg.width, s_textureSeg.height),
        Rectangle(gameScreen.left + (m_leftX + 0.01) * gameScreen.right,
            gameScreen.top + (1.0 - (m_holeY - HOLE_HEIGHT / 2.0)) * gameScreen.bottom,
            gameScreen.right * (WIDTH - 0.02),
            gameScreen.bottom - (1.0 - m_holeY) * gameScreen.bottom),
        Vector2(0, 0),
        0.0,
        WHITE);

    DrawTextureEx(
        s_textureTop,
        Vector2(gameScreen.left + m_leftX * gameScreen.right,
            gameScreen.top + (1.0 - (m_holeY - HOLE_HEIGHT / 2.0)) * gameScreen.bottom),
        0.0,
        gameScreen.right * WIDTH / s_textureTop.width,
        WHITE);

    // Upper
    /*DrawRectangle(
        gameScreen.left + m_leftX * gameScreen.right,
        gameScreen.top,
        WIDTH * gameScreen.right,
        (1.0 - (m_holeY + HOLE_HEIGHT / 2.0)) * gameScreen.bottom,
        GREEN);*/

    DrawTexturePro(
        s_textureSeg,
        Rectangle(0, 0, s_textureSeg.width, s_textureSeg.height),
        Rectangle(gameScreen.left + (m_leftX + 0.01) * gameScreen.right,
            gameScreen.top,
            gameScreen.right * (WIDTH - 0.02),
            (1.0 - (m_holeY + HOLE_HEIGHT / 2.0)) * gameScreen.bottom),
        Vector2(0, 0),
        0.0,
        WHITE);

    DrawTexturePro(
        s_textureTop,
        Rectangle(0, 0, s_textureTop.width, s_textureTop.height),
        Rectangle(gameScreen.left + m_leftX * gameScreen.right,
            (1.0 - (m_holeY + HOLE_HEIGHT / 2.0)) * gameScreen.bottom - gameScreen.right * WIDTH / s_textureTop.width * s_textureTop.height,
            gameScreen.right * WIDTH,
            gameScreen.right * WIDTH / s_textureTop.width * s_textureTop.height),
        Vector2(0, 0),
        0.0,
        WHITE);
}
