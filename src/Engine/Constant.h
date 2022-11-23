#pragma once

#include <cstdlib>

namespace rt {
    struct Constant {
        static constexpr float Epsilon = 0.000001f;
        static constexpr float MinDist = 0.001f;
        static constexpr float PI = 3.1415926;
        static const unsigned int DefaultScreenWidth = 1200;
        static const unsigned int DefaultScreenHeight = 800;
        static const unsigned int DefaultScreenFOV = 90;
    };
}  // namespace rt
