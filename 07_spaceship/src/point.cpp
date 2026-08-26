#include "point.hpp"
#include <stdexcept>

Point::Point(int x, int y): _x{x}, _y{y}
{};

void Point::setX(const int x)
{
    _x = x;
};

void Point::setY(const int y)
{
    _y = y;
};

int Point::getX() const
{
    if (!_x.has_value()) {
        throw std::logic_error("x is not set");
    }
    return _x.value();
};

int Point::getY() const
{
    if (!_y.has_value()) {
        throw std::logic_error("y is not set");
    }
    return _y.value();
};

Point operator + (const Point& p, const Vector& v) {
    return Point{p.getX() + v.getDx(), p.getY() + v.getDy()};
}

bool operator == (const Point& lhs, const Point& rhs) {
    return lhs.getX() == rhs.getX() && lhs.getY() == rhs.getY();
}
