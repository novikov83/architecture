#pragma once
#include "point.hpp"
#include "vector.hpp"
#include "universal.hpp"

// интерфейс для объекта, который в данный момент движется прямолинейно
class IMoving {
public:
    virtual void    setLocation(const Point& p) = 0;
    virtual Point   getLocation() const = 0;
    virtual Vector  getVelocity() const = 0;

    virtual ~IMoving() = default;
};

// адаптер
class MovingAdapter: public IMoving {
    UniversalItem& _obj;
public:
    MovingAdapter(UniversalItem& obj);

    void    setLocation(const Point& newLocation) override;
    Point   getLocation() const override;
    Vector  getVelocity() const override;

    ~MovingAdapter() override;
};

// класс выполняющий перемещение
class Move {
    IMoving& _obj;
public:
    Move(IMoving& obj);

    void Execute();
};
