#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include "ai/neuralnetwork.hpp"
#include "raylib.h"

class Pipe;

class Bird {
    friend class Game;

public:
    enum class Type {
        NORMAL = 0,
        DEAD = 1,
        LAST_BEST = 2,
        BEST = 3,
        SINGLE = 4,
    };

private:
    struct Vec {
        f64 x;
        f64 y;
    };

public:
    static constexpr const f64 RADIUS = 1.0 / 20.0;
    static constexpr const f64 X_OFFSET = 1.0 / 3.0;

public:
    Bird();
    Bird(const NeuralNetwork& brain);
    ~Bird() = default;

    void move(f64 deltaTime, const Pipe* pNearestPipe);
    void jump();
    void freefall(f64 deltaTime);
    void draw(const utils::Rect<i32>& gameScreen, Type type) const;
    void incrScore() noexcept;
    void reset();

    inline f64 getY() const noexcept { return m_y; }
    inline f64 getX() const noexcept { return m_x; }
    inline bool isDead() const noexcept { return m_dead; }
    inline u64 getScore() const noexcept { return m_score; }
    inline f64 getSpeed() const noexcept { return m_speed; }

private:
    bool hasCollided(const Pipe* pNearestPipe);
    static bool circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim);
    void fall(f64 deltaTime);

    // ---------------------------------- AI stuff ----------------------------------
public:
    void think(f64 holeY, f64 deltaHole);
    void createRandomNeuralNetwork();
    void getBrainFromFile(std::string_view file);
    f64 fitness() const noexcept;

    inline const NeuralNetwork& getBrain() const noexcept { return m_brain; }
    // ---------------------------------- AI stuff ----------------------------------

private:
    static constexpr const f64 ACCELERATION = -0.05;
    static constexpr const f64 JUMP_STRENGTH = 0.016;
    static constexpr const f64 MAX_SPEED = 1.5;

    f64 m_y = 0.5; // 0.0 < birdY < 1.0
    f64 m_speed = 0.0;

    f64 m_x = X_OFFSET;
    u64 m_score = 0;
    bool m_dead = FALSE;
    f64 m_rotation = 0.0;

    static bool s_firstInit;
    static Texture2D s_texture;
    static Texture2D s_textureDead;
    static Texture2D s_textureBest;
    static Texture2D s_textureLastBest;
    static Texture2D s_textureSingle;

    // ---------------------------------- AI stuff ----------------------------------
    NeuralNetwork m_brain;
    f64 m_survivedTime = 0.0;
    // ---------------------------------- AI stuff ----------------------------------
};
