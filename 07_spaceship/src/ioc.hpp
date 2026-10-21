#pragma once

#include <unordered_map>
#include <functional>
#include <memory>

#include "command.hpp"

class IoC {
    using Function = std::function<void(IoC&)>;

    std::unordered_map<std::string, Function> _map;
public:
    template<typename T>
    void reg(const std::string& key, std::function<std::shared_ptr<T>(IoC&)> func) {
        _map[key] = [func](IoC& ioc){
            return func(ioc);
        };
    };

    template<typename T>
    std::shared_ptr<T> resolve(const std::string& key) {
        auto it = _map.find(key);
        if (it == _map.end()) {
            throw std::runtime_error("Key not found");
        }
        return std::static_pointer_cast<T>(it->second(*this));
    };
};

class IoCRegisterCommand: public ICommand {
public:
//    IoCRegisterCommand(IMoving& obj);

    void Execute() override {

    };
    ~IoCRegisterCommand() override = default;
};
