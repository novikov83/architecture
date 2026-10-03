#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spaceship.hpp>
#include <fuel.hpp>
#include <macro.hpp>
#include <move.hpp>

// CheckFuelCommand
// 1. Реализовать класс CheckFuelCommand и тесты к нему.
class CheckFuelTest: public ::testing::Test {
protected:
    SpaceShip           spaceship;
    FuelUsingAdapter    adapter{spaceship};
    CheckFuelCommand    command{adapter};
};
TEST_F(CheckFuelTest, CheckFuelOk)
{
    // проверка изменения топлива, когда топлива хватает 
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 10);
    EXPECT_NO_THROW(command.Execute());
}
TEST_F(CheckFuelTest, CheckFuelErr)
{
    // проверка изменения топлива, когда топлива не хватает 
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 11);
    EXPECT_THROW(command.Execute(), CommandException);
}

// BurnFuelCommand
// 2. Реализовать класс BurnFuelCommand и тесты к нему.
class BurnFuelTest: public ::testing::Test {
protected:
    SpaceShip           spaceship;
    FuelUsingAdapter    adapter{spaceship};
    BurnFuelCommand     command{adapter};
};
TEST_F(BurnFuelTest, BurnFuelOk)
{
    // проверка изменения топлива, когда топлива хватает 
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 10);
    EXPECT_NO_THROW(command.Execute());
    EXPECT_EQ(spaceship.getProperty<int>("Fuel"), 0);
}
TEST_F(BurnFuelTest, BurnFuelOver)
{
    // проверка изменения топлива, когда топлива не хватает 
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 11);
    EXPECT_NO_THROW(command.Execute());
    EXPECT_EQ(spaceship.getProperty<int>("Fuel"), -1);
}

// CheckFuelCommand + BurnFuelCommand
// 3. Реализовать простейшую макрокоманду и тесты к ней. 
// Здесь простейшая - это значит, что при выбросе исключения 
// вся последовательность команд приостанавливает свое выполнение, 
// а макрокоманда выбрасывает CommandException.
class CheckBurnFuelTest: public ::testing::Test {
protected:
    SpaceShip           spaceship;
    FuelUsingAdapter    adapter{spaceship};
    // CheckFuelCommand    check_command{adapter};
    // BurnFuelCommand     burn_command{adapter};
};
TEST_F(CheckBurnFuelTest, CheckBurnFuelOk)
{
    // проверка и сжигание топлива, когда топлива хватает 
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 10);

    MacroCommand macro_command{
        std::make_unique<CheckFuelCommand>(adapter), 
        std::make_unique<BurnFuelCommand>(adapter)
    };

    EXPECT_NO_THROW(macro_command.Execute());
    EXPECT_EQ(spaceship.getProperty<int>("Fuel"), 0);
}
TEST_F(CheckBurnFuelTest, CheckBurnFuelErr)
{
    // проверка и сжигание топлива, когда топлива не хватает 
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 11);

    MacroCommand macro_command{
        std::make_unique<CheckFuelCommand>(adapter), 
        std::make_unique<BurnFuelCommand>(adapter)
    };

    EXPECT_THROW(macro_command.Execute(), CommandException);
    EXPECT_EQ(spaceship.getProperty<int>("Fuel"), 10);
}
// 4. Реализовать команду движения по прямой с расходом топлива, 
// используя команды с предыдущих шагов.
TEST_F(CheckBurnFuelTest, CheckMoveBurnFuelOk)
{
    // проверка, движение и сжигание топлива, когда топлива хватает 
    spaceship.setProperty<Point>("Location", {1, 1});
    spaceship.setProperty<Vector>("Velocity", {2, 0});
    spaceship.setProperty<int>("Fuel", 10);
    spaceship.setProperty<int>("FuelVelocity", 10);

    std::vector<std::unique_ptr<ICommand>> commands;
    commands.emplace_back(std::make_unique<CheckFuelCommand>(adapter));
    MovingAdapter    move_adapter{spaceship};
    commands.emplace_back(std::make_unique<MoveCommand>(move_adapter));
    commands.emplace_back(std::make_unique<BurnFuelCommand>(adapter));
    
    MacroCommand macro_command{std::move(commands)};

    EXPECT_NO_THROW(macro_command.Execute());
    EXPECT_EQ(spaceship.getProperty<int>("Fuel"), 0);
    EXPECT_EQ(spaceship.getProperty<Point>("Location"), Point(3, 1));
}
