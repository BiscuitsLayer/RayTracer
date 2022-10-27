#include <iostream>
#include <vector>
#include <thread>
#include "Engine.h"
#include "../Light/PointLight.h"

namespace rt {
    Engine::Engine(AssimpLoader const &loader) : _loader(loader), _camera(loader.GetCameraFromScene()) {
        _meshes = loader.GetMeshesFromScene();
        _lights = loader.GetLightsFromScene();
    }

    Color Engine::Raytrace(const rt::Vector2<unsigned int> &pixel) {
        Color color = Color();
        Ray ray = _camera.GenerateRay(pixel);
        Intersection inter = _intersect(ray);
        if (inter.Intersect) {
            for (size_t i = 0; i < _lights.size(); ++i) {
                Vector3<float> lightDir = _lights[i]->GetPos() - inter.Point;
                lightDir.Normalize();
                Intersection interLight = _intersect(Ray(inter.Point, lightDir));
                if (!interLight.Intersect ||
                    interLight.Dist > (_lights[i]->GetPos() - inter.Point).Norm()) {
                    float angle = lightDir.Angle(inter.Normal);
                    if (angle > 90.f) {
                        angle = 180.f - angle;
                    }
                    color += (Color(inter.DiffuseColor) * ((-1.f / 90.f) * angle + 1.f));
                }
            }
        }
        return color;
    }

    /*void Engine::_pathtrace(Ray const& ray, unsigned int const& depth, Color & color) {

    }*/

    Intersection const Engine::_intersect(Ray const& ray) {
        Intersection rtn;
        float min = -1;

        for (size_t i = 0; i < _meshes.size(); ++i) {
            Intersection inter = _meshes[i]->Intersect(ray);
            if (inter.Intersect) {
                if (min == -1 || inter.Dist < min) {
                    min = inter.Dist;
                    rtn = inter;
                }
            }
        }
        return rtn;
    }

    Vector2<unsigned int> Engine::GetRes() const {
        return _camera.GetRes();
    }
}  // namespace rt
