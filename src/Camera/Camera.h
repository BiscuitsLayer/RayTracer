#pragma once

#include <random>
#include <array>
#include "../Vector/Vector3.h"
#include "../Vector/Vector2.h"
#include "../Engine/Tools.h"

namespace rt {
class Camera {
 public:
   Camera();

   Ray const  GenerateRay(Vector2<unsigned int> const &pos);
   
   Vector3<float> const&                  GetPos(void) const;

   void                                   MoveForward(void);
   void                                   MoveLeft(void);
   void                                   MoveBack(void);
   void                                   MoveRight(void);

   void                                   TurnLeft(void);
   void                                   TurnRight(void);

   Vector2<unsigned int> const&           GetRes(void) const;
   void                                   SetMatrix(Vector3<float> const& pos, Vector3<float> const& c1, Vector3<float> const& c2, Vector3<float> const& c3);
 
 private:
   Vector3<float>                         _pos;
   Vector3<float>                         _c1;
   Vector3<float>                         _c2;
   Vector3<float>                         _c3;
   Vector2<unsigned int>                  _screenRes;
   Vector2<float>                         _screenSize;
   Vector3<float>                         _screenCorner;
   float                                  _screenDist;
   std::mt19937                           _gen;
   std::uniform_real_distribution<float>  _dis;

   float                                  _vStep = 0.5f;
   float                                  _hStep = 0.2f;


   void     generateScreen();
};
}  // namespace rt
