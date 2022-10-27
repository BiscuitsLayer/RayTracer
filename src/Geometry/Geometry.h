#pragma once

#include <vector>
#include "../Vector/Vector3.h"
#include "../Vector/Vector3.h"
#include "../Engine/Tools.h"

namespace rt
{
   class Vertex {
     public:
        Vertex(Vector3<float> const& pos);

        Vector3<float> const&   GetPos() const;
        Vector3<float> const&   GetNormal() const;
        void                    SetNormal(Vector3<float> const& normal);

     private:
        Vector3<float>  _pos;
        Vector3<float>  _normal;
    };
   class Triangle
   {
   public:
      Triangle(Vertex const &v1, Vertex const &v2, Vertex const &v3);
      Triangle(Vertex const &v1, Vertex const &v2, Vertex const &v3, Vector3<float> const &diffuseColor);

      Intersection const Intersect(Ray const &ray);

      Vertex const &GetV1() const;
      Vertex const &GetV2() const;
      Vertex const &GetV3() const;
      Vector3<float> const &GetNormal() const;

   private:
      Vertex _v1;
      Vertex _v2;
      Vertex _v3;
      Vector3<float> _normal;
      Vector3<float> _edge1;
      Vector3<float> _edge2;
      Vector3<float> _diffuseColor;

      void generateCharacteristics();
   };
   class Object
   {
   public:
      Object(std::vector<Triangle> const &triangles, Vector3<float> const &diffuseColor);

      Intersection const Intersect(Ray const &ray);

   private:
      std::vector<Triangle> _triangles;
      Vector3<float> _diffuseColor;
   };
} // namespace rt
