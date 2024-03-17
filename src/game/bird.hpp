#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"

class Bird {
public:
    static constexpr const f64 RADIUS = 1.0 / 20.0;
    static constexpr const f64 X_OFFSET = 1.0 / 3.0;

public:
    Bird();
    ~Bird() = default;

    void move(f64 deltaTime);
    void jump();
    void draw(const utils::Rect<i32>& gameScreen);
    void freefall(f64 deltaTime);

    inline f64 getY() const noexcept { return m_birdY; }

private:
    f64 speed();

private:
    f64 m_birdY = 0.5; // 0.0 < birdY < 1.0
    f64 m_lastJump = 0.0;
};
