#pragma once

#include "defines.hpp"
#include "button.hpp"
#include "dropdown.hpp"
#include <vector>

namespace ui {

class UIHandler {
public:
    UIHandler() = default;
    ~UIHandler() = default;
    UIHandler(const UIHandler&) = delete;
    UIHandler operator=(const UIHandler&) = delete;

    void onUpdate(u32 mouseX, u32 mouseY, bool mouseDown, bool mouseReleased, Application* pApplication);
    void onRender() const;
    void addButton(utils::Rect<u32> rect, std::string text, Button::fnOnPress fnOnPress = nullptr, Button::fnOnHover fnOnHover = nullptr);
    void addDropdown(Dropdown& dropdown);
    inline Dropdown& getDropdown(u32 id) { return m_dropdowns[id]; }

private:
    std::vector<Button> m_buttons;
    std::vector<Dropdown> m_dropdowns;
};

} // namespace ui
