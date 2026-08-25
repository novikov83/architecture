#pragma once
#include <optional>
#include "vector.hpp"

// координата в двухмерном пространстве
class Point {
    std::optional<int> _x; // координата по оси x
    std::optional<int> _y; // координата по оси y
public:
    Point() = default;
    Point(int x, int y);

    void setX(const int x);
    void setY(const int y);
    int getX() const;
    int getY() const;

};
Point operator + (const Point& p, const Vector& v);
bool operator == (const Point& lhs, const Point& rhs);
