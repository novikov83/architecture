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