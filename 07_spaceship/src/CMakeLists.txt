add_library(spaceship SHARED 
    spaceship.hpp
    vector.cpp vector.hpp
    point.cpp point.hpp
    universal.hpp
    move.cpp move.hpp
    rotate.cpp rotate.hpp
    spaceship.hpp
)

target_compile_features(spaceship PUBLIC cxx_std_20)
