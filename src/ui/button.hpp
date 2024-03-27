#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include <string>
#include <string_view>

namespace ui {
class Button;
} // namespace ui
class Application;

typedef void (*fnOnHover)(ui::Button&, Application*);
typedef void (*fnOnPress)(ui::Button&, Application*);

namespace ui {

class Button {
public:
    enum class State {
        UP,
        DOWN,
        HOVER
    };

public:
    Button(utils::Rect<u32> rect, std::string_view text, fnOnPress fnOnPress = nullptr, fnOnHover fnOnHover = nullptr);
    ~Button() noexcept = default;

    virtual void onRender() const;
    virtual void onUpdate(u32 mouseX, u32 mouseY, bool mouseDown, Application* pApplication);

    inline utils::Rect<f64>& getRect() const noexcept { return (utils::Rect<f64>&) m_rect; }
    inline void setText(std::string_view text) noexcept { m_text = text; }
    inline State getState() const noexcept { return m_state; }
    inline void setState(State state) noexcept { m_state = state; }
    inline fnOnPress getFnOnPress() const noexcept { return m_fnOnPress; }
    inline void setFnOnPress(fnOnPress fn) noexcept { m_fnOnPress = fn; }
    inline fnOnHover getFnOnHover() const noexcept { return m_fnOnHover; }
    inline void setFnOnHover(fnOnHover fn) noexcept { m_fnOnHover = fn; }

protected:
    bool inside(u32 x, u32 y) const noexcept;

private:
    utils::Rect<u32> m_rect;
    std::string m_text;
    State m_state = State::UP;

    fnOnPress m_fnOnPress;
    fnOnHover m_fnOnHover;
};

} // namespace ui
