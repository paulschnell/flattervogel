#pragma once

#include "defines.hpp"
#include "button.hpp"
#include <vector>

namespace ui {

class ButtonHandler {
public:
    ButtonHandler() = default;
    ~ButtonHandler() = default;
    ButtonHandler(const ButtonHandler&) = delete;
    ButtonHandler operator=(const ButtonHandler&) = delete;

    void onUpdate(u32 mouseX, u32 mouseY, bool mouseDown, Application* pApplication);
    void onRender() const;
    void addButton(utils::Rect<u32> rect, std::string text, fnOnPress fnOnPress = nullptr, fnOnHover fnOnHover = nullptr);

private:
    std::vector<Button> m_buttons;
};

} // namespace ui
