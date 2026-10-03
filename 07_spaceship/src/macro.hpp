#pragma once

#include <memory>
#include <vector>
#include <utility>

#include "command.hpp"

class MacroCommand: public ICommand {
    std::vector<std::unique_ptr<ICommand>> _commands;
public:
    // MacroCommand(std::vector<std::unique_ptr<ICommand>> commands) : _commands(commands) {};
    template<typename... Commands>
    explicit MacroCommand(Commands&&... commands) {
        _commands.reserve(sizeof...(Commands));

        (_commands.emplace_back(std::forward<Commands>(commands)), ...);
    }
    explicit MacroCommand(std::vector<std::unique_ptr<ICommand>> commands)
        : _commands(std::move(commands)){}

    void Execute() override {
        for (auto& command : _commands) {
            command->Execute();
        }
    };
};
