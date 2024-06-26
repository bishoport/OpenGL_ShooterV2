#pragma once
#include <cmath>
#include <string>
#include <limits>
class Vector2d
{
public:
    struct HashFunction
    {
        size_t operator()(const Vector2d& vector) const;
    };
    double X;
    double Y;
    Vector2d();
    Vector2d(const Vector2d& val);
    Vector2d(double xVal, double yVal);
    void Negate();
    double DistanceTo(Vector2d& val) const;
    void Normalize();
    Vector2d Normalized() const;
    double Dot(Vector2d val) const;
    double DistanceSquared(Vector2d val) const;
    Vector2d operator - (const Vector2d& right) const;
    Vector2d operator + (const Vector2d& right) const; // Marcar como const
    Vector2d operator * (const Vector2d& right) const; // Marcar como const
    Vector2d operator * (double scalar) const;
    bool operator == (const Vector2d& right) const; // Marcar como const
    bool operator != (const Vector2d& right) const; // Marcar como const
    Vector2d& operator += (const Vector2d& right); // Marcar como const
    Vector2d& operator *= (const Vector2d& right); // Marcar como const
    Vector2d& operator *= (double right);
    Vector2d& operator = (const Vector2d& right);
    bool operator < (const Vector2d& right) const;
    bool Equals(Vector2d val) const;
    std::string ToString() const;
    static Vector2d Empty();
    size_t hash() const;
    bool IsEmpty() const;
};
