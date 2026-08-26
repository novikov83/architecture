#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector.hpp>

class VectorTest : public ::testing::Test
{
protected:
    Vector v;
};

TEST_F(VectorTest, Throw)
{
    // чтение не инициализированного Vector
    EXPECT_THROW(v.getDx(), std::logic_error);
    EXPECT_THROW(v.getDy(), std::logic_error);
}

TEST_F(VectorTest, SetGet)
{
    // установка и чтение значений
    v.setDx(111);
    EXPECT_EQ(v.getDx(), 111);
    v.setDy(222);
    EXPECT_EQ(v.getDy(), 222);
}
