#pragma once

#include "defines.hpp"
#include "utils/rect.hpp"
#include <string>
#include <string_view>
#include <vector>

class Application;

namespace ui {

class Dropdown {
public:
    class Element;

    typedef void (*fnOnNewSelected)(Element& newSelected, Dropdown& dropdown, Application* pApplication);
    typedef void (*fnOnPress)(ui::Dropdown& dropdown, Application*);

    class Element {
    public:
        Element(std::string_view text, fnOnPress fnOnPress = nullptr);
        ~Element() noexcept = default;

        void onRender(utils::Rect<u32> rect) const;

        inline fnOnPress getFnOnPress() const noexcept { return m_fnOnPress; }
        inline void setFnOnPress(fnOnPress fnOnPress) noexcept { m_fnOnPress = fnOnPress; }
        inline bool isHovering() const noexcept { return m_hovering; }
        inline void setHovering(bool hovering) noexcept { m_hovering = hovering; }
        inline std::string_view getText() const noexcept { return m_text; }

    private:
        std::string m_text;
        fnOnPress m_fnOnPress;
        bool m_hovering = FALSE;
    };

public:
    Dropdown(utils::Rect<u32> rect, fnOnNewSelected fnOnNewSelected);
    ~Dropdown() noexcept = default;

    void onRender() const;
    bool onUpdate(u32 mouseX, u32 mouseY, bool mouseReleased, Application* pApplication);

    inline void addElement(std::string_view name, fnOnPress fnOnPress = nullptr) { m_elements.emplace_back(name, fnOnPress); }
    inline const Element& getLastElement() const noexcept { return m_elements.back(); }

    inline void setSelected(u32 selected) noexcept { m_selected = selected; }
    inline u32 getSelected() const noexcept { return m_selected; }
    inline u32 getNumElements() const noexcept { return m_elements.size(); }

private:
    utils::Rect<u32> m_rect;
    bool m_expanded = FALSE;
    bool m_hovering = FALSE;
    std::vector<Element> m_elements;
    u32 m_selected = 0;

    fnOnNewSelected m_fnOnNewSelected;
};

} // namespace ui
