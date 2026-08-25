#pragma once
#include <optional>

// вектор скорости в двухмерном пространстве
class Vector {
    std::optional<int> _dx; // дельта изменения координаты x за квант времени
    std::optional<int> _dy; // дельта изменения координаты y за квант времени

public:
    Vector() = default;
    Vector(int dx, int dy);
    void setDx(const int dx);
    void setDy(const int dy);
    int getDx() const;
    int getDy() const;
};
bool operator == (const Vector& lhs, const Vector& rhs);
