#pragma once

#include "command.hpp"
#include "universal.hpp"

class IFuelUsing {
public:
    virtual int getFuel() const = 0;
    virtual void setFuel(int fuelVelocity) = 0;
    virtual int getFuelVelocity() const = 0;
    virtual ~IFuelUsing() = default;
};

class FuelUsingAdapter: public IFuelUsing {
    UniversalItem& _obj;
public:
    FuelUsingAdapter(UniversalItem& obj) : _obj(obj) {};
    int getFuel() const override {
        return _obj.getProperty<int>("Fuel");
    };
    void setFuel(int fuelVelocity) override {
        return _obj.setProperty("Fuel", fuelVelocity);
    };
    int getFuelVelocity() const override {
        return _obj.getProperty<int>("FuelVelocity");
    };
    ~FuelUsingAdapter() override = default;
};

class CheckFuelCommand: public ICommand {
    IFuelUsing& _obj;
public:
    CheckFuelCommand(IFuelUsing& obj): _obj(obj) {};

    void Execute() override {
        if (_obj.getFuelVelocity() > _obj.getFuel()) {
            throw CommandException("Exception in CheckFuelCommand");
        }
    };
    ~CheckFuelCommand() override = default;
};

class BurnFuelCommand: public ICommand {
    IFuelUsing& _obj;
public:
    BurnFuelCommand(IFuelUsing& obj): _obj(obj) {};

    void Execute() override {
        _obj.setFuel(_obj.getFuel() - _obj.getFuelVelocity());
    };
    ~BurnFuelCommand() override = default;
};
