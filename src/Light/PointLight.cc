#include "PointLight.h"

namespace rt {

    PointLight::PointLight(Vector3<float> const& pos): _pos(pos), _color(0xffffffff), _intensity(1.f) {
    }
    
    PointLight::PointLight(Vector3<float> const& pos, Color const& color): _pos(pos) {
        _color = color;
    }

    Color const& PointLight::GetColor() const {
        return _color;
    }

    float const& PointLight::GetBrightness() const {
        return _intensity;
    }

    Vector3<float> PointLight::GetPos() const {
        return _pos;
    }
}  //namespace rt
