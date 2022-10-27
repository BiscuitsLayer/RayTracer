#pragma once

#include "../Engine/Color.h"
#include "../Vector/Vector3.h"

namespace rt {
    class PointLight {
     public:
        PointLight(Vector3<float> const& pos);
        PointLight(Vector3<float> const& pos, Color const& color);

        Vector3<float>          GetPos() const;
        Color const&            GetColor() const;
        float const&            GetBrightness() const;

     private:
        Vector3<float>  _pos;
        Color   _color;
        float   _intensity;
    };
}