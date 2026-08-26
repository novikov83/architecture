#pragma once
#include <map>
#include <string>
#include <any>

// универсальный объект, который имеет свойства
class UniversalItem {
    std::map<std::string, std::any> _property;
public:
    template<typename T>
    T getProperty(const std::string& key) const {
        return any_cast<T>(_property.at(key));
    }

    template<typename T>
    void setProperty(const std::string& key, T value) {
        _property[key] = std::move(value);
    }
    virtual ~UniversalItem() = default;
};
