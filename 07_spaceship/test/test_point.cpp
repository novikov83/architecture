#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <point.hpp>

class PointTest : public ::testing::Test
{
protected:
    Point p;
};

TEST_F(PointTest, Throw)
{
    // чтение не инициализированного Point
    EXPECT_THROW(p.getX(), std::logic_error);
    EXPECT_THROW(p.getY(), std::logic_error);
}

TEST_F(PointTest, SetGet)
{
    // установка и чтение значений
    p.setX(111);
    EXPECT_EQ(p.getX(), 111);
    p.setY(222);
    EXPECT_EQ(p.getY(), 222);
}
