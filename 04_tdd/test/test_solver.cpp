#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <solver.hpp>

using ::testing::AllOf;
using ::testing::SizeIs;
using ::testing::Contains;
using ::testing::DoubleNear;
using ::testing::Values;

class SolverTest : public ::testing::Test
{
protected:
    Solver s;
};

TEST_F(SolverTest, NoRoots)
{
    // проверка что корней нет
    EXPECT_TRUE(s.solve(1.0, 0.0, 1.0).empty());
}

TEST_F(SolverTest, TwoRoots)
{
    // проверка что есть два корня 1 и -1
    EXPECT_THAT(
        s.solve(1.0, 0.0, -1.0),
        AllOf (
            SizeIs(2),
            Contains(1),
            Contains(-1)
        )
    );
}

TEST_F(SolverTest, OneRoot)
{
    // проверка что есть один корень -1
    EXPECT_THAT(
        s.solve(1.0, 2.0, 1.0),
        AllOf (
            SizeIs(1),
            Contains(-1)
        )
    );
}

TEST_F(SolverTest, ZeroA)
{
    // проверка что при a = 0 выбрасывается исключение
    EXPECT_THROW(
        s.solve(0.0, 2.0, 3.0),
        std::invalid_argument
    );
}

TEST_F(SolverTest, SmallOneRoot)
{
    // проверка что маленькие корни тоже вычисляются
    EXPECT_THAT(
        s.solve(0.0001, 0.0002, 0.0001),
        AllOf (
            SizeIs(1),
            Contains(DoubleNear(-0.00000001, 1e-9))
        )
    );
}

TEST_F(SolverTest, SmallTwoRoot)
{
    // проверка что маленькие корни тоже вычисляются
    EXPECT_THAT(
        s.solve(0.0001, 0.0, -0.0001),
        AllOf (
            SizeIs(2),
            Contains(DoubleNear( 0.00000001, 1e-9)),
            Contains(DoubleNear(-0.00000001, 1e-9))
        )
    );
}

class SolverNaNTest : 
    public SolverTest,
    public ::testing::WithParamInterface<std::tuple<double, double, double>>
{};
TEST_P(SolverNaNTest, ForNaN)
{
    // проверка что при нечисловых коэффицентах выбрасывается исключение
    const auto [a, b, c] = GetParam();
    EXPECT_THROW(
        s.solve(a, b, c),
        std::invalid_argument
    );
}
INSTANTIATE_TEST_SUITE_P(
    NaN,
    SolverNaNTest,
    Values(
        std::make_tuple(std::nan(""), 2.0, 3.0),
        std::make_tuple(1.0, std::nan(""), 3.0),
        std::make_tuple(1.0, 2.0, std::nan(""))
    )
);