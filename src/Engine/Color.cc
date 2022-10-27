#include <algorithm>
#include "Color.h"

rt::Color::Color() {
    _color.hexcode = 0x00000000;
}

rt::Color::Color(std::uint32_t hexcode) {
    _color.hexcode = hexcode;
}

rt::Color::Color(const rt::Color_Component &component) : _color(component) {
}

rt::Color::Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    _color.rgba.r = red;
    _color.rgba.g = green;
    _color.rgba.b = blue;
    _color.rgba.a = 0;
}

rt::Color::Color(Vector3<float> const& vec) {
    _color.rgba.r = vec.X * 255;
    _color.rgba.g = vec.Y * 255;
    _color.rgba.b = vec.Z * 255;
    _color.rgba.a = 0;
}

rt::Color_Component rt::Color::GetColor() const {
    return _color;
}

void rt::Color::SetColor(const rt::Color_Component &component) {
    _color = component;
}

std::uint8_t const& rt::Color::GetRedComponent() const {
    return _color.rgba.r;
}

void rt::Color::SetRedComponent(std::uint8_t value) {
    _color.rgba.r = value;
}

std::uint8_t const& rt::Color::GetBlueComponent() const {
    return _color.rgba.b;
}

void rt::Color::SetBlueComponent(std::uint8_t value) {
    _color.rgba.b = value;
}

std::uint8_t const& rt::Color::GetGreenComponent() const {
    return _color.rgba.g;
}

void rt::Color::SetGreenComponent(std::uint8_t value) {
    _color.rgba.g = value;
}

rt::Color const& rt::Color::operator+=(rt::Color const& other) {
    _color.rgba.r = std::min(_color.rgba.r + other._color.rgba.r, 255);
    _color.rgba.g = std::min(_color.rgba.g + other._color.rgba.g, 255);
    _color.rgba.b = std::min(_color.rgba.b + other._color.rgba.b, 255);
    return *this;
}

rt::Color const rt::Color::operator*(float const& coef) {
    Color rtn;
    rtn._color.rgba.r = std::min(_color.rgba.r * coef, 255.f);
    rtn._color.rgba.g = std::min(_color.rgba.g * coef, 255.f);
    rtn._color.rgba.b = std::min(_color.rgba.b * coef, 255.f);
    return rtn;
}
