#pragma once
#include <vector>

class Solver {
    const double EPS = 1e-12;
public:
    std::vector<double> solve(const double& a, const double& b, const double& c);
};
