#include <numbers>
#include "rotate.hpp"

// RotatingAdapter
RotatingAdapter::RotatingAdapter(UniversalItem& obj): _obj(obj)
{};

void RotatingAdapter::setVelocity(const Vector& newVelocity)
{
    _obj.setProperty("Velocity", newVelocity);
};

Vector RotatingAdapter::getVelocity() const
{
    return _obj.getProperty<Vector>("Velocity");
};

int RotatingAdapter::getAngleVelocity() const
{
    return _obj.getProperty<int>("AngleVelocity");
};

// Rotate
Rotate::Rotate(IRotating& obj): _obj(obj)
{};

void Rotate::Execute()
{
    Vector v = _obj.getVelocity();
    double angle = _obj.getAngleVelocity() * std::numbers::pi / 180;
    int new_dx = (double)v.getDx() * std::cos(angle) - (double)v.getDy() * std::sin(angle);
    int new_dy = (double)v.getDy() * std::sin(angle) + (double)v.getDy() * std::cos(angle);
    _obj.setVelocity({new_dx, new_dy});
};
