#include "button_handler.hpp"

namespace ui {

void ButtonHandler::onUpdate(u32 mouseX, u32 mouseY, bool mouseDown, Application* pApplication) {
    for (auto& button : m_buttons) {
        button.onUpdate(mouseX, mouseY, mouseDown, pApplication);
    }
}

void ButtonHandler::onRender() const {
    for (auto& button : m_buttons) {
        button.onRender();
    }
}

void ButtonHandler::addButton(utils::Rect<u32> rect, std::string text, fnOnPress fnOnPress, fnOnHover fnOnHover) {
    m_buttons.emplace_back(rect, text, fnOnPress, fnOnHover);
}

} // namespace ui