#include "../Engine/Constant.h"
#include "Geometry.h"

namespace rt {
    Vertex::Vertex(Vector3<float> const& pos) : _pos(pos) {
    }

    Vector3<float> const&   Vertex::GetPos() const {
        return _pos;
    }

    Vector3<float> const&   Vertex::GetNormal() const {
        return _normal;
    }

    void Vertex::SetNormal(Vector3<float> const& normal) {
        _normal = normal;
    }

    Triangle::Triangle(Vertex const& v1, Vertex const& v2,
    Vertex const& v3): _v1(v1), _v2(v2), _v3(v3) {
        this->generateCharacteristics();
    }

    Triangle::Triangle(Vertex const& v1, Vertex const& v2,
    Vertex const& v3, Vector3<float> const& diffuseColor): _v1(v1), _v2(v2), _v3(v3) {
        _diffuseColor = diffuseColor;
        this->generateCharacteristics();
    }

    Intersection const Triangle::Intersect(Ray const& ray) {
        Intersection ret;
        Vector3<float> pvec = ray.Direction.Cross(_edge2);
        float det = _edge1.Dot(pvec);
        if (det > -Constant::Epsilon && det < Constant::Epsilon) {
            return ret;
        }
        Vector3<float> tvec = ray.Origin - _v1.GetPos();
        float u = tvec.Dot(pvec) / det;
        if (u < 0.f || u > 1.f) {
            return ret;
        }
        Vector3<float> qvec = tvec.Cross(_edge1);
        float v = ray.Direction.Dot(qvec) / det;
        if (v < 0.f || u + v > 1.f) {
            return ret;
        }
        float t = _edge2.Dot(qvec) / det;
        if (t < Constant::MinDist) {
            return ret;
        }
        ret.Intersect = true;
        ret.Point = ray.Origin + ray.Direction * t;
        ret.Dist = t;
        if (_v1.GetNormal() != Vector3<float>()) {
            ret.Normal = _v1.GetNormal() * (1 - u - v) + _v2.GetNormal() * u + _v3.GetNormal() * v;
        } else {
            ret.Normal = _normal;
        }
        ret.DiffuseColor = _diffuseColor;
        return ret;
    }

   Vertex const& Triangle::GetV1() const {
       return _v1;
   }

   Vertex const& Triangle::GetV2() const {
       return _v2;
   }

   Vertex const& Triangle::GetV3() const {
       return _v3;
   }

   Vector3<float> const& Triangle::GetNormal() const {
       return _normal;
   }

    void Triangle::generateCharacteristics() {
        _edge1 = _v2.GetPos() - _v1.GetPos();
        _edge2 = _v3.GetPos() - _v1.GetPos();
        _normal = _edge1.Cross(_edge2);
        _normal.Normalize();
    }

    Object::Object(std::vector<Triangle> const& triangles, Vector3<float> const& diffuseColor) : _triangles(triangles) {
        _diffuseColor = diffuseColor;
    }

    Intersection const Object::Intersect(Ray const& ray) {
        Intersection intersection = Intersection();
        Intersection inter;
        float min = -1;
        for (std::size_t i = 0; i < _triangles.size(); ++i) {
            inter = _triangles[i].Intersect(ray);
            if (inter.Intersect) {
                if (min == -1 || inter.Dist < min) {
                    min = inter.Dist;
                    intersection = inter;
                }
            }
        }
        intersection.DiffuseColor = _diffuseColor;
        return intersection;
    }  
}  // namespace rt
