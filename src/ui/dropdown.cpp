#include "dropdown.hpp"

#include "application.hpp"
#include "raylib.h"

using namespace ui;

Dropdown::Element::Element(std::string_view text, fnOnPress fnOnPress)
    : m_text(text), m_fnOnPress(fnOnPress) {}

void Dropdown::Element::onRender(utils::Rect<u32> rect) const {
    Color primary = m_hovering ? BLUE : DARKGRAY;
    Color secondary = m_hovering ? WHITE : BLUE;

    DrawRectangle(rect.left, rect.top, rect.right, rect.bottom, primary);
    DrawText(m_text.c_str(), rect.left + 6, rect.top + 6, rect.bottom - 6, secondary);
    DrawRectangleLinesEx({ (f32) rect.left, (f32) rect.top, (f32) rect.right, (f32) rect.bottom }, 4, secondary);
}

Dropdown::Dropdown(utils::Rect<u32> rect, fnOnNewSelected fnOnNewSelected)
    : m_rect(rect), m_fnOnNewSelected(fnOnNewSelected) {}

void Dropdown::onRender() const {
    Color primary = m_hovering && !m_expanded ? DARKBLUE : LIGHTGRAY;
    Color secondary = m_hovering && !m_expanded ? LIGHTGRAY : DARKBLUE;

    DrawRectangle(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom, primary);
    DrawText(m_elements[m_selected].getText().data(), m_rect.left + 6, m_rect.top + 6, m_rect.bottom - 6, secondary);
    DrawRectangleLinesEx({ (f32) m_rect.left, (f32) m_rect.top, (f32) m_rect.right, (f32) m_rect.bottom }, 4, secondary);

    if (m_expanded) {
        for (u32 i = 0; i < m_elements.size(); i++) {
            utils::Rect<u32> rect = m_rect;
            rect.top += rect.bottom * (i + 1);

            m_elements[i].onRender(rect);
        }
    }
}

bool Dropdown::onUpdate(u32 mouseX, u32 mouseY, bool mouseReleased, Application* pApplication) {
    bool usedMouse = FALSE;
    m_hovering = mouseX >= m_rect.left && mouseX <= m_rect.left + m_rect.right && mouseY >= m_rect.top && mouseY <= m_rect.top + m_rect.bottom;

    for (u32 i = 0; i < m_elements.size(); i++) {
        auto& ele = m_elements[i];
        utils::Rect<u32> rect = m_rect;
        rect.top += rect.bottom * (i + 1);

        ele.setHovering(m_expanded && mouseX >= rect.left && mouseX <= rect.left + rect.right && mouseY >= rect.top && mouseY <= rect.top + rect.bottom);

        if (ele.isHovering()) {
            if (mouseReleased) {
                if (ele.getFnOnPress() != nullptr)
                    ele.getFnOnPress()(*this, pApplication);
                m_selected = i;
                m_expanded = FALSE;

                m_fnOnNewSelected(ele, *this, pApplication);
                return TRUE;
            }

            usedMouse = TRUE;
        }
    }

    if (mouseReleased) {
        if (m_hovering && m_expanded) {
            m_expanded = FALSE;
        } else if (m_hovering && !m_expanded) {
            m_expanded = TRUE;
        } else if (!m_hovering && m_expanded) {
            m_expanded = FALSE;
        }
    }
    return usedMouse;
}
