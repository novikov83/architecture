#pragma once
#include <cmath>
#include "vector.hpp"
#include "universal.hpp"
#include "command.hpp"

// интерфейс для объекта, который в данный момент поворачивается
class IRotating {
public:
    virtual Vector  getDirection() const = 0;
    virtual void    setDirection(const Vector& v) = 0;
    virtual int     getAngleDirection() const = 0;

    virtual ~IRotating() = default;
};

// адаптер
class RotatingAdapter: public IRotating {
    UniversalItem& _obj;
public:
    RotatingAdapter(UniversalItem& obj);
    void    setDirection(const Vector& newVelocity) override;
    Vector  getDirection() const override;
    int     getAngleDirection() const override;

    ~RotatingAdapter() override = default;
};

// класс осуществляющий поворот
class RotateCommand: public ICommand {
    IRotating& _obj;
public:
    RotateCommand(IRotating& obj);
    void Execute() override;
    ~RotateCommand() override = default;
};
