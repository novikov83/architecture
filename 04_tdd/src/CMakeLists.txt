add_library(solver SHARED solver.cpp solver.hpp)

target_compile_features(solver PUBLIC cxx_std_20)
