#pragma once

// интерфейс для исключения
class IException {
    std::string _str;
public:
    IException(const std::string& str): _str(str) {};
    virtual std::string what() {
        return _str;
    };
    virtual ~IException() = default;
};

// пример исключения
class SendException: public IException {
public:
    using IException::IException;
    ~SendException() override = default;
};

// исключение для комманд
class CommandException: public IException {
public:
    using IException::IException;
    ~CommandException() override = default;
};
