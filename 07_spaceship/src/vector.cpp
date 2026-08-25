#include "vector.hpp"
#include <stdexcept>

Vector::Vector(int dx, int dy): _dx{dx}, _dy{dy}
{};

void Vector::setDx(const int dx)
{
    _dx = dx;
};

void Vector::setDy(const int dy)
{
    _dy = dy;
};

int Vector::getDx() const
{
    if (!_dx.has_value()) {
        throw std::logic_error("dx is not set");
    }
    return _dx.value();
};

int Vector::getDy() const
{
    if (!_dy.has_value()) {
        throw std::logic_error("dy is not set");
    }
    return _dy.value();
};

bool operator == (const Vector& lhs, const Vector& rhs) {
    return lhs.getDx() == rhs.getDx() && lhs.getDy() == rhs.getDy();
}
