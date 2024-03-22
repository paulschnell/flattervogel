#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"

class Pipe {
public:
    static constexpr const f64 SPEED = 0.25;
    static constexpr const f64 WIDTH = 0.2; // 0.0 < WIDTH < 1.0
    static constexpr const f64 HOLE_HEIGHT = 0.25; // 0.0 < HOLE_HEIGHT < 1.0
    static constexpr const f64 MIN_PIPE_HEIGHT = 0.2; // 0.0 < MIN_PIPE_HEIGHT < 1.0

public:
    Pipe() = default;
    Pipe(f64 spawnX);
    ~Pipe() = default;

    void move(f64 deltaTime);
    void randomHoleY();
    void draw(const utils::Rect<i32>& gameScreen) const;

    inline f64 getLeftX() const noexcept { return m_leftX; }
    inline f64 getHoleY() const noexcept { return m_holeY; }

private:
    f64 m_leftX = 1.0; // left side
    f64 m_holeY = 0.0; // MIN_PIPE_HEIGHT + HOLE_HEIGHT / 2 < holeY < 1.0 - MIN_PIPE_HEIGHT - HOLE_HEIGHT / 2
};
