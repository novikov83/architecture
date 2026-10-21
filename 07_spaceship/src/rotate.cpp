#include <numbers>
#include "rotate.hpp"

// RotatingAdapter
RotatingAdapter::RotatingAdapter(UniversalItem& obj): _obj(obj)
{};

void RotatingAdapter::setDirection(const Vector& newVelocity)
{
    _obj.setProperty("Direction", newVelocity);
};

Vector RotatingAdapter::getDirection() const
{
    return _obj.getProperty<Vector>("Direction");
};

int RotatingAdapter::getAngleDirection() const
{
    return _obj.getProperty<int>("AngleDirection");
};

// RotateCommand
RotateCommand::RotateCommand(IRotating& obj): _obj(obj)
{};

void RotateCommand::Execute()
{
    Vector v = _obj.getDirection();
    double angle = _obj.getAngleDirection() * std::numbers::pi / 180;
    int new_dx = (double)v.getDx() * std::cos(angle) - (double)v.getDy() * std::sin(angle);
    int new_dy = (double)v.getDy() * std::sin(angle) + (double)v.getDy() * std::cos(angle);
    _obj.setDirection({new_dx, new_dy});
};
