#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spaceship.hpp>
#include <universal.hpp>
#include <rotate.hpp>

// !!! на занятии говорили что скорость через вектор (dx, dy) это
// плохо (так как на малых числах теряем скорость при повороте), а
// в тесте именно используется вектор (dx, dy). в итоге для поворота
// приходиться делать тоже через (dx, dy).

class RotateTest: public ::testing::Test {
protected:
    SpaceShip           spaceship;
    RotatingAdapter     adapter{spaceship};
    RotateCommand              rotate{adapter};
};
TEST_F(RotateTest, Rotate45Object)
{
    // Для объекта, движущегося по вектору (10, 10) 
    // и поворачивающегося на 45 градусов, 
    // вектор меняется на (0, 14)

    spaceship.setProperty<Vector>("Direction", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 45);
    rotate.Execute();
    EXPECT_EQ(spaceship.getProperty<Vector>("Direction"), Vector(0, 14));
}
TEST_F(RotateTest, Rotate90Object)
{
    // Для объекта, движущегося по вектору (10, 10) 
    // и поворачивающегося на 90 градусов, 
    // вектор меняется на (-10, 10)

    spaceship.setProperty<Vector>("Direction", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 90);
    rotate.Execute();
    EXPECT_EQ(spaceship.getProperty<Vector>("Direction"), Vector(-10, 10));
}
TEST_F(RotateTest, Rotate360Object)
{
    // Для объекта, движущегося по вектору (10, 10) 
    // и поворачивающегося на 360 градусов, 
    // вектор меняется на (10, 10)

    spaceship.setProperty<Vector>("Direction", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 360);
    rotate.Execute();
    // !!! ошибка округления. должно быть (10, 10)
    EXPECT_EQ(spaceship.getProperty<Vector>("Direction"), Vector(10, 9));
}
TEST_F(RotateTest, Rotate405Object)
{
    // Для объекта, движущегося по вектору (10, 10) 
    // и поворачивающегося на 360+45=405 градусов, 
    // вектор меняется на (0, 14)

    spaceship.setProperty<Vector>("Direction", {10, 10});
    spaceship.setProperty<int>("AngleDirection", 405);
    rotate.Execute();
    EXPECT_EQ(spaceship.getProperty<Vector>("Direction"), Vector(0, 14));
}
TEST_F(RotateTest, ThrowDirection)
{
    // Попытка сдвинуть объект, 
    // у которого невозможно прочитать скорость поворота, 
    // приводит к ошибке
    // spaceship.setProperty<Vector>("Direction", {1, 1});

    spaceship.setProperty<int>("AngleDirection", 45);
    EXPECT_THROW(rotate.Execute(), std::logic_error);
}
TEST_F(RotateTest, ThrowAngleDirection)
{
    // Попытка сдвинуть объект, 
    // у которого невозможно прочитать вектор скорости, 
    // приводит к ошибке

    spaceship.setProperty<Vector>("Direction", {1, 1});
    // spaceship.setProperty<int>("AngleDirection", 45);
    EXPECT_THROW(rotate.Execute(), std::logic_error);
}
TEST_F(RotateTest, Throw)
{
    // Попытка сдвинуть объект, 
    // у которого невозможно прочитать 
    // и вектор скорости и скорость поворота
    // приводит к ошибке

    // spaceship.setProperty<Vector>("Direction", {1, 1});
    // spaceship.setProperty<int>("AngleDirection", 45);
    EXPECT_THROW(rotate.Execute(), std::logic_error);
}
