#include "ui_handler.hpp"

namespace ui {

void UIHandler::onUpdate(u32 mouseX, u32 mouseY, bool mouseDown, bool mouseReleased, Application* pApplication) {
    bool usedMouse = FALSE;
    for (auto& ele : m_dropdowns) {
        usedMouse = ele.onUpdate(mouseX, mouseY, mouseReleased, pApplication);
        if (usedMouse)
            break;
    }

    if (!usedMouse) {
        for (auto& ele : m_buttons) {
            ele.onUpdate(mouseX, mouseY, mouseDown, pApplication);
        }
    }
}

void UIHandler::onRender() const {
    for (const auto& ele : m_buttons) {
        ele.onRender();
    }

    for (const auto& ele : m_dropdowns) {
        ele.onRender();
    }
}

void UIHandler::addButton(utils::Rect<u32> rect, std::string text, Button::fnOnPress fnOnPress, Button::fnOnHover fnOnHover) {
    m_buttons.emplace_back(rect, text, fnOnPress, fnOnHover);
}

void UIHandler::addDropdown(Dropdown& dropdown) {
    m_dropdowns.push_back(dropdown);
}

} // namespace ui
