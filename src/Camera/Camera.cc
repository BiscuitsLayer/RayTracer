#include <cstdlib>
#include <cmath>
#include "Camera.h"
#include "../Engine/Constant.h"

namespace rt {
    Camera::Camera(): _pos(Vector3<float>(0, 0, 0)), _c1(Vector3<float>(1, 0, 0)),
        _c2(Vector3<float>(0, 1, 0)), _c3(Vector3<float>(0, 0, 1)),
        _gen(std::random_device()()), _dis(0.f, 1.f) {
        generateScreen();
    }

    Ray const Camera::GenerateRay(Vector2<unsigned int> const &pos) {
        #ifndef RT_TESTING_ENV
        float Rx = _dis(_gen);
        float Ry = _dis(_gen);
        #else
        float Rx = 0.0f;
        float Ry = 0.0f;
        #endif
        Vector3<float> direction = (_screenCorner + _c1 * ((pos.X + Rx) / _screenRes.X) * _screenSize.X - _c2 * ((pos.Y + Ry) / _screenRes.Y) * _screenSize.Y - _pos) * _screenDist;
        direction.Normalize();
        Ray ray(_pos, direction);
        return ray;
    }

    Vector3<float> const& Camera::GetPos(void) const {
        return _pos;
    }

    void Camera::MoveForward(void) {
        _pos = _pos - _c3 * _vStep;
        generateScreen();
    }

    void Camera::MoveLeft(void) {
        _pos = _pos - _c1 * _hStep;
        generateScreen();
    }

    void Camera::MoveBack(void) {
        _pos = _pos + _c3 * _vStep;
        generateScreen();
    }

    void Camera::MoveRight(void) {
        _pos = _pos + _c1 * _hStep;
        generateScreen();
    }

    void Camera::TurnLeft(void) {
        Vector3<float> _c1_old = _c1;
        Vector3<float> _c3_old = _c3;
        _c3 = _c3_old + _c1_old * 0.05;
        _c1 = _c1_old - _c3_old * 0.05;
        _c3.Normalize();
        _c1.Normalize();
        generateScreen();
    }

    void Camera::TurnRight(void) {
        Vector3<float> _c1_old = _c1;
        Vector3<float> _c3_old = _c3;
        _c3 = _c3_old - _c1_old * 0.05;
        _c1 = _c1_old + _c3_old * 0.05;
        _c3.Normalize();
        _c1.Normalize();
        generateScreen();
    }

    Vector2<unsigned int> const& Camera::GetRes(void) const {
        return _screenRes;
    }

   void Camera::SetMatrix(Vector3<float> const& c1, Vector3<float> const& c2,
        Vector3<float> const& c3, Vector3<float> const& pos) {
            _c1 = c1;
            _c2 = c2;
            _c3 = c3;
            _pos = pos;
            generateScreen();
    }

   void Camera::generateScreen() {
        _screenDist = 0.5f;
        _screenRes = Vector2<unsigned int>(Constant::DefaultScreenWidth, Constant::DefaultScreenHeight);
        float screenWidth = 2.f * std::tan((Constant::DefaultScreenFOV / 2.f) * static_cast<float>(Constant::PI) / 180.f) * _screenDist;
        _screenSize = Vector2<float>(screenWidth, screenWidth * _screenRes.Y / _screenRes.X);
        _screenCorner = _pos + _c3 * (-1.f) - _c1 * (_screenSize.X / 2.f) + _c2 * (_screenSize.Y / 2.f);
   }
}  // namespace rt
