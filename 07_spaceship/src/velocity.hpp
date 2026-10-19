#pragma once
#include "vector.hpp"
#include "universal.hpp"
#include "command.hpp"

// интерфейс для объекта, который может менять вектор скорости
class IChangebleVelocity {
public:
    virtual Vector  getVelocity() const = 0;
    virtual int     getAngleDirection() const = 0;
    virtual void    setVelocity(const Vector& v) = 0;

    virtual ~IChangebleVelocity() = default;
};

// адаптер
class ChangeVelocityAdapter: public IChangebleVelocity {
    UniversalItem& _obj;
public:
    ChangeVelocityAdapter(UniversalItem& obj);

    Vector  getVelocity() const override;
    int     getAngleDirection() const override;
    void    setVelocity(const Vector& v) override;

    ~ChangeVelocityAdapter() override = default;
};

// класс выполняющий изменение вектора скорости
class ChangeVelocityCommand: public ICommand {
    IChangebleVelocity& _obj;
public:
    ChangeVelocityCommand(IChangebleVelocity& obj);

    void Execute() override;
    ~ChangeVelocityCommand() override = default;
};
