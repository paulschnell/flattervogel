#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"

class Pipe;

class Bird {
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
    ~Bird() = default;

    void move(f64 deltaTime, const Pipe* pNearestPipe);
    void jump();
    void draw(const utils::Rect<i32>& gameScreen) const;
    void incrScore() noexcept;

    inline f64 getY() const noexcept { return m_y; }
    inline bool isDead() const noexcept { return m_dead; }

private:
    f64 speed() const;
    bool hasCollided(const Pipe* pNearestPipe);
    static bool circleRectCollision(Vec circlePos, f64 circleR, Vec rectPos, Vec rectDim);

private:
    f64 m_y = 0.5; // 0.0 < birdY < 1.0
    f64 m_lastJump = 0.0;

    f64 m_x = X_OFFSET;
    u64 m_score = 0;
    bool m_dead = FALSE;
};
