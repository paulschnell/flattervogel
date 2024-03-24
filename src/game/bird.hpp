#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include "ai/neuralnetwork.hpp"

class Pipe;

class Bird {
    friend class Brain;

private:
    struct Vec {
        f64 x;
        f64 y;
    };

public:
    static constexpr const f64 RADIUS = 1.0 / 20.0;
    static constexpr const f64 X_OFFSET = 1.0 / 3.0;

public:
    Bird() = default;
    Bird(const NeuralNetwork& brain);
    ~Bird() = default;

    void move(f64 deltaTime, const Pipe* pNearestPipe);
    void jump();
    void freefall(f64 deltaTime);
    void draw(const utils::Rect<i32>& gameScreen) const;
    void incrScore() noexcept;

    void think(f64 holeY);

    inline f64 getY() const noexcept { return m_y; }
    inline f64 getX() const noexcept { return m_x; }
    inline bool isDead() const noexcept { return m_dead; }
    inline u64 getScore() const noexcept { return m_score; }

private:
    f64 speed() const;
    bool hasCollided(const Pipe* pNearestPipe);
    static bool circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim);

    // ---------------------------------- AI stuff ----------------------------------
public:
    void createRandomNeuralNetwork();
    f64 fitness() const noexcept;

    inline const NeuralNetwork& getBrain() const noexcept { return m_brain; }
    // ---------------------------------- AI stuff ----------------------------------

private:
    f64 m_y = 0.5; // 0.0 < birdY < 1.0
    f64 m_lastJump = 0.0;

    f64 m_x = X_OFFSET;
    u64 m_score = 0;
    bool m_dead = FALSE;

    // ---------------------------------- AI stuff ----------------------------------
    NeuralNetwork m_brain;
    f64 m_deadY = 0.5;
    f64 m_survivedTime = 0.0;
    // ---------------------------------- AI stuff ----------------------------------
};
