#pragma once

#include "../Vector/Vector3.h"

namespace rt {
    struct Ray {
        Ray(): Origin(), Direction() {};
        Ray(Vector3<float> const& origin, Vector3<float> const& dir): Origin(origin), Direction(dir) {};

        Vector3<float>  Origin;
        Vector3<float>  Direction;
    };

    struct Intersection {
        Intersection(): Intersect(false), Point(), Dist(-1), Normal() {};
        Intersection(bool intersect, Vector3<float> const& point, float const& dist, Vector3<float> const& normal, Vector3<float> const& diffuseColor): Intersect(intersect), Point(point), Dist(dist), Normal(normal), DiffuseColor(diffuseColor) {};

        bool            Intersect;
        Vector3<float>  Point;
        float           Dist;
        Vector3<float>  Normal;
        Vector3<float>  DiffuseColor;
    };
}  // namespace rt
