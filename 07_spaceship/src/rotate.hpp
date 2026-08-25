#pragma once
#include "vector.hpp"
#include "universal.hpp"

// интерфейс для объекта, который в данный момент поворачивается
class IRotating {
public:
    virtual Vector  getVelocity() const = 0;
    virtual void    setVelocity(const Vector& v) = 0;
    virtual int     getAngleVelocity() const = 0;

    virtual ~IRotating() = default;
};

// адаптер
class RotatingAdapter: public IRotating {
    UniversalItem& _obj;
public:
    RotatingAdapter(UniversalItem& obj);
    void    setVelocity(const Vector& newVelocity) override;
    Vector  getVelocity() const override;
    int     getAngleVelocity() const override;

    ~RotatingAdapter() override = default;
};

// класс осуществляющий поворот
class Rotate {
    IRotating& _obj;
public:
    Rotate(IRotating& obj);
    void Execute();
};
