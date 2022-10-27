#pragma once

#include <cstdint>
#include "../Vector/Vector3.h"

namespace rt {
    union Color_Component {
        std::uint32_t   hexcode;
        struct rgbaTag {
            std::uint8_t    a;
            std::uint8_t    b;
            std::uint8_t    g;
            std::uint8_t    r;
        } rgba;
    };

    class Color {
    public:
        explicit    Color();
        explicit    Color(std::uint32_t hexcode);
        explicit    Color(Color_Component const& component);
        explicit    Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
        explicit    Color(Vector3<float> const& vec);

        Color_Component     GetColor() const;
        void                SetColor(Color_Component const& component);
        std::uint8_t const& GetRedComponent() const;
        void                SetRedComponent(std::uint8_t value);
        std::uint8_t const& GetBlueComponent() const;
        void                SetBlueComponent(std::uint8_t value);
        std::uint8_t const& GetGreenComponent() const;
        void                SetGreenComponent(std::uint8_t value);

        Color const& operator+=(Color const& other);
        Color const operator*(float const& coef);

    private:
        Color_Component _color;
    };
}