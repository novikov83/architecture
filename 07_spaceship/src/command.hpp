#pragma once

#include <typeindex>

#include "exception.hpp"
// #include "common.hpp"

// интерфейс для объекта, который является командой
class ICommand {
public:
    virtual void Execute() = 0;
    virtual ~ICommand() = default;
};

// пример команды, которая что-то куда-то отправляет
// и потенциально может кинуть исключение
class SendCommand: public ICommand {
public:
    explicit SendCommand() {};
    void Execute() override {
        // throw SendException("Error sending date");
    };
    ~SendCommand() override = default;
};

// команда "повторитель"
class FirstRepeatCommand: public ICommand {
    std::unique_ptr<ICommand> _prev_command;
public:
    explicit FirstRepeatCommand(std::unique_ptr<ICommand> command): 
        _prev_command(std::move(command)) {};
    void Execute() override {
        _prev_command->Execute();
    };
    ~FirstRepeatCommand() override = default;
};

// команда "повторитель" с другим типом
class SecondRepeatCommand: public ICommand {
    std::unique_ptr<ICommand> _prev_command;
public:
    explicit SecondRepeatCommand(std::unique_ptr<ICommand> command): 
        _prev_command(std::move(command)) {};
    void Execute() override {
        _prev_command->Execute();
    };
    ~SecondRepeatCommand() override = default;
};

#include <fstream>
// команда "логгер"
// ??? как в логгере узнать первую команду которая упала с исключением?
// ??? как в логгере узнать параметры первой команды для более детальной распечатки?
class LogCommand: public ICommand {
    std::unique_ptr<ICommand>   _prev_command;
    std::ofstream               _file;

    std::type_index GetType(const ICommand& command) {
        // вынес в отдельную функцию, чтобы компилятор не выдавал warning:
        // expression with side effects will be evaluated despite being used as an operand to 'typeid' [-Wpotentially-evaluated-expression]
        // если использовать: auto it = _map.find(Key(typeid(*command), typeid(exception)));
        return typeid(command);
    }
public:
    explicit LogCommand(std::unique_ptr<ICommand> command): 
        _prev_command(std::move(command)) {
        _file.open("LogCommand.log", std::ios::app);
    };
    void Execute() override {
        _file << "Execution of command '" << GetType(*_prev_command).name() << "' failed" << std::endl;
    };
    ~LogCommand() override = default;
};

// сервисная команда для остановки обработчика команд
class StopCommand: public ICommand {
public:
    explicit StopCommand() {};
    void Execute() override {};
    ~StopCommand() override = default;
};
