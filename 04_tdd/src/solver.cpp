#include <vector>
#include <cmath>
#include <stdexcept>
#include "solver.hpp"

std::vector<double> Solver::solve(const double& a, const double& b, const double& c) {
    if (std::abs(a) < EPS) {
        throw std::invalid_argument("a");
    }
    if (std::isnan(a)) {
        throw std::invalid_argument("a");
    }
    if (std::isnan(b)) {
        throw std::invalid_argument("b");
    }
    if (std::isnan(c)) {
        throw std::invalid_argument("c");
    }

    double d = b * b - 4 * a * c;
    if (std::abs(d) < EPS) {
        d = 0.0;
    }
    if (d < 0.0) {
        return {};
    }
    else if (d > 0.0) {
        return {
            (-b + std::sqrt(d)) / 2 * a,
            (-b - std::sqrt(d)) / 2 * a
        };
    }
    else if (d == 0.0) { // d == 0.0
        return {-b / 2 * a};
    }

    return {};
}
