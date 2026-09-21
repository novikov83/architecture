#include <stdexcept>
#include "move.hpp"

// MovingAdapter
MovingAdapter::MovingAdapter(UniversalItem& obj): _obj(obj)
{};

void MovingAdapter::setLocation(const Point& newLocation)
{
    _obj.setProperty("Location", newLocation);
};

Point MovingAdapter::getLocation() const
{
    return _obj.getProperty<Point>("Location");
};

Vector MovingAdapter::getVelocity() const
{
    return _obj.getProperty<Vector>("Velocity");
};

MovingAdapter::~MovingAdapter()
{};

// MoveCommand
MoveCommand::MoveCommand(IMoving& obj): _obj{obj}
{};

void MoveCommand::Execute()
{
    Vector v = _obj.getVelocity();
    if (v.getDx() == 0 && v.getDy() == 0) {
        throw(std::logic_error(""));
    }
    _obj.setLocation(_obj.getLocation() + v);
};
