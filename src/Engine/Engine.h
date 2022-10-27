#pragma once

#include <memory>
#include "../Camera/Camera.h"
#include "../Loader/AssimpLoader.h"
#include "../Vector/Vector2.h"
#include "../Vector/Vector3.h"
#include "Color.h"
#include "Tools.h"

namespace rt {
    class Engine {
    public:
        explicit    Engine(AssimpLoader const& loader);

        Engine(const Engine& engine) = default;

        Color                   Raytrace(Vector2<unsigned int> const& pixel);
        Vector2<unsigned int>   GetRes() const;
        Camera*                 GetCamera() { return &_camera; }

    private:
        AssimpLoader                        _loader;
        Camera                              _camera;
        std::vector<std::shared_ptr<Object>>_meshes;
        std::vector<std::shared_ptr<PointLight>> _lights;

        void                _pathtrace(Ray const& ray, unsigned int const& depth, Color & color);
        Intersection const  _intersect(Ray const& ray);
    };
}  // namespace rt
