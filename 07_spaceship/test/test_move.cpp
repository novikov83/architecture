#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spaceship.hpp>
#include <vector.hpp>
#include <point.hpp>
#include <move.hpp>

// !!! не получилось написать тесты до разработки основного кода
// без интерфейса и названий функций тест превращался в пустышку

// !!! на занятии говорили, что скорость через вектор (dx, dy) это
// плохо (так как на малых числах теряем скорость при повороте), а
// в тесте именно используется вектор (dx, dy). в итоге для поворота
// приходиться делать тоже через (dx, dy).

class MoveTest: public ::testing::Test {
protected:
    SpaceShip      spaceship;
    MovingAdapter   adapter{spaceship};
    Move            move{adapter};
};
TEST_F(MoveTest, MoveObject)
{
    // Для объекта, находящегося в точке (12, 5) и 
    // движущегося со скоростью (-7, 3) движение меняет 
    // положение объекта на (5, 8)
    spaceship.setProperty<Point>("Location", {12, 5});
    spaceship.setProperty<Vector>("Velocity", {-7, 3});
    move.Execute();
    EXPECT_EQ(spaceship.getProperty<Point>("Location"), Point(5, 8));
}
TEST_F(MoveTest, ThrowLocation)
{
    // Попытка сдвинуть объект, 
    // у которого невозможно прочитать положение в пространстве, 
    // приводит к ошибке
    spaceship.setProperty<Vector>("Velocity", {-7, 3});
    EXPECT_THROW(move.Execute(), std::logic_error);
}
TEST_F(MoveTest, ThrowVelocity)
{
    // Попытка сдвинуть объект, 
    // у которого невозможно прочитать значение мгновенной скорости, 
    // приводит к ошибке
    spaceship.setProperty<Point>("Location", {12, 5});
    EXPECT_THROW(move.Execute(), std::logic_error);
}
TEST_F(MoveTest, ThrowMoving)
{
    // Попытка сдвинуть объект,
    // у которого невозможно изменить положение в пространстве, 
    // приводит к ошибке
    // !!! Не понятно что такое "невозможно". Сделал тест для 
    // вектора скорости (0, 0)
    spaceship.setProperty<Point>("Location", {12, 5});
    spaceship.setProperty<Vector>("Velocity", {0, 0});
    EXPECT_THROW(move.Execute(), std::logic_error);
}
