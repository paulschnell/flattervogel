#include "button.hpp"

#include "raylib.h"

namespace ui {

Button::Button(utils::Rect<u32> rect, std::string_view text, fnOnPress fnOnPress, fnOnHover fnOnHover)
    : m_rect(rect), m_text(text), m_fnOnPress(fnOnPress), m_fnOnHover(fnOnHover) {}

void Button::onRender() const {
    Color colorInner = Color();
    Color colorOuter = Color();
    switch (m_state) {
    case State::UP:
        colorInner = LIGHTGRAY;
        colorOuter = DARKGRAY;
        break;
    case State::DOWN:
        colorInner = DARKGRAY;
        colorOuter = ORANGE;
        break;
    case State::HOVER:
        colorInner = YELLOW;
        colorOuter = LIGHTGRAY;
        break;
    }

    DrawRectangle(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom, colorInner);
    DrawRectangleLinesEx({ (f32) m_rect.left, (f32) m_rect.top, (f32) m_rect.right, (f32) m_rect.bottom }, 4, colorOuter);

    DrawText(m_text.c_str(), (m_rect.left + m_rect.right / 2) - MeasureText(m_text.c_str(), m_rect.bottom - 6) / 2, m_rect.top + 6, m_rect.bottom - 6, colorOuter);
}

void Button::onUpdate(u32 mouseX, u32 mouseY, bool mouseDown, Application *pApplication) {
    if (inside(mouseX, mouseY)) {
        if (mouseDown) {
            if (m_state == State::HOVER && m_fnOnPress != nullptr) {
                m_fnOnPress(*this, pApplication);
            }
            m_state = State::DOWN;

        } else {
            if (m_state == State::UP && m_fnOnHover != nullptr) {
                m_fnOnHover(*this, pApplication);
            }
            m_state = State::HOVER;
        }

    } else {
        m_state = State::UP;
    }
}
bool Button::inside(u32 x, u32 y) const noexcept {
    return x >= m_rect.left && x <= m_rect.left + m_rect.right && y <= m_rect.top + m_rect.bottom && y >= m_rect.top;
}

} // namespace ui