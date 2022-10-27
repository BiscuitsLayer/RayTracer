#pragma once

#include <iostream>

namespace rt {
    template <class T>
    struct Vector2 {
    public:
        Vector2(void) : X(0), Y(0) {};
        Vector2(T _X, T _Y) : X(_X), Y(_Y) {};

        bool    operator!=(Vector2<T> const& other) const {
            return (X != other.X || Y != other.Y);
        }

        Vector2<T> operator+(Vector2<T> const& right) const {
            return Vector2<T>(X + right.X, Y + right.Y);
        }

        Vector2<T> operator-(Vector2<T> const& right) const {
            return Vector2<T>(X - right.X, Y - right.Y);
        }

        Vector2<T> operator*(T const& other) const {
            return Vector2<T>(X * other, Y * other);
        }

        Vector2<T> operator/(T const& other) const {
            return Vector2<T>(X / other, Y / other);
        }

        T   X;
        T   Y;
    };

    template <class T>
    std::ostream& operator<<(std::ostream& out, const Vector2<T>& v) {
        out << "(" << v.X << ", " << v.Y << ")";
        return out;
    }
}  // namespace rt
