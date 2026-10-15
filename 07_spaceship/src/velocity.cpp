#include <cmath>
#include <numbers>
#include "velocity.hpp"

// ChangeVelocityAdapter
ChangeVelocityAdapter::ChangeVelocityAdapter(UniversalItem& obj): _obj(obj)
{};

Vector ChangeVelocityAdapter::getVelocity() const
{
    return _obj.getProperty<Vector>("Velocity");
}
void ChangeVelocityAdapter::setVelocity(const Vector& newVelocity)
{
    _obj.setProperty("Velocity", newVelocity);
};
int ChangeVelocityAdapter::getAngleDirection() const
{
    return _obj.getProperty<int>("AngleDirection");
};

// ChangeVelocityCommand
ChangeVelocityCommand::ChangeVelocityCommand(IChangebleVelocity& obj): _obj(obj)
{}
void ChangeVelocityCommand::Execute()
{
    Vector v = _obj.getVelocity();
    if (v.getDx() == 0 || v.getDy() == 0) {
        throw CommandException("No Velocity");
    }
    double angle = _obj.getAngleDirection() * std::numbers::pi / 180;
    int new_dx = (double)v.getDx() * std::cos(angle) - (double)v.getDy() * std::sin(angle);
    int new_dy = (double)v.getDy() * std::sin(angle) + (double)v.getDy() * std::cos(angle);
    _obj.setVelocity({new_dx, new_dy});
};
