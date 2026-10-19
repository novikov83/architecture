#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spaceship.hpp>
#include <macro.hpp>
#include <velocity.hpp>
#include <macro.hpp>
#include <rotate.hpp>

// ChangeVelocityCommand
// 5. Реализовать команду для модификации вектора 
// мгновенной скорости при повороте. Необходимо учесть, 
// что не каждый разворачивающийся объект движется.
class ChangeVelocityTest: public ::testing::Test {
protected:
    SpaceShip               spaceship;
    ChangeVelocityAdapter   adapter{spaceship};
    ChangeVelocityCommand   command{adapter};
};
TEST_F(ChangeVelocityTest, ChangeVelocityOk)
{
    // проверка изменения вектора скорости, когда объект движется
    spaceship.setProperty<Vector>("Velocity", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 90);
    EXPECT_NO_THROW(command.Execute());
    EXPECT_EQ(spaceship.getProperty<Vector>("Velocity"), Vector(-10, 10));
}
TEST_F(ChangeVelocityTest, ChangeVelocityErr)
{
    // проверка изменения вектора скорости, когда не объект движется
    spaceship.setProperty<Vector>("Velocity", {0, 0});
    spaceship.setProperty<int>("AngleDirection", 90);
    EXPECT_THROW(command.Execute(), CommandException);
}

// RotateAndChangeVelocityCommand
// 6. Реализовать команду поворота, которая еще и меняет 
// вектор мгновенной скорости, если есть.
class RotateAndChangeVelocityTest: public ::testing::Test {
protected:
    SpaceShip               spaceship;
};
TEST_F(RotateAndChangeVelocityTest, RotateAndChangeVelocityOK)
{
    // проверка изменения вектора скорости, когда объект движется
    spaceship.setProperty<Vector>("Direction", {10, 10});
    spaceship.setProperty<Vector>("Velocity", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 90);

    RotatingAdapter         rotating_adapter{spaceship};
    ChangeVelocityAdapter   changevelocity_adapter{spaceship};

    MacroCommand macro_command{
        std::make_unique<RotateCommand>(rotating_adapter), 
        std::make_unique<ChangeVelocityCommand>(changevelocity_adapter)
    };

    EXPECT_NO_THROW(macro_command.Execute());
    EXPECT_EQ(spaceship.getProperty<Vector>("Direction"), Vector(-10, 10));
    EXPECT_EQ(spaceship.getProperty<Vector>("Velocity"), Vector(-10, 10));
}

TEST_F(RotateAndChangeVelocityTest, RotateAndChangeVelocityOK_2)
{
    // проверка изменения вектора скорости, когда объект движется
    spaceship.setProperty<Vector>("Direction", {20, 20});
    spaceship.setProperty<Vector>("Velocity", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 90);

    RotatingAdapter         rotating_adapter{spaceship};
    ChangeVelocityAdapter   changevelocity_adapter{spaceship};

    MacroCommand macro_command{
        std::make_unique<RotateCommand>(rotating_adapter), 
        std::make_unique<ChangeVelocityCommand>(changevelocity_adapter)
    };

    EXPECT_NO_THROW(macro_command.Execute());
    EXPECT_EQ(spaceship.getProperty<Vector>("Direction"), Vector(-20, 20));
    EXPECT_EQ(spaceship.getProperty<Vector>("Velocity"), Vector(-10, 10));
}
