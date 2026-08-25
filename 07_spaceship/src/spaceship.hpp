#pragma once
#include "universal.hpp"

// корабль
class SpaceShip: public UniversalItem {
public:
    ~SpaceShip() override = default;
};

// торпеда
class Torpedo: public UniversalItem {
public:
    ~Torpedo() override = default;
};

// метеорит
class Meteor: public UniversalItem {
public:
    ~Meteor() override = default;
};
