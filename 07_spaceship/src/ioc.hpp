#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <any>

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include "command.hpp"

// Замечание к реализации:
// 1. Пришлось сделать хранилище (Скоуп) публичным, чтобы можно было
// расширять функционал новымы Командами которые будут менять
// хранилище. Алтернативный вариант - сделать публичные методы 
// доступа к хранилищу, но это ограничивает дальнейшее расширение,
// так как накладывает ограничение в использовании этих методов.
// 2. Самую первую Фабрику - Команду регистрации в IoC надо
// передавать в конструкторе Споупа. Это ограничение С++ - нет
// возможности создать объект типа по его строковому названию.

class IoC;

// Arg - набор аргументов любого типа.
using Arg = std::vector<std::any>;

// Factory - Фабрика создающая объекты
// может принимать любое количество аргументов,
// в том числе и не принимать аргументы совсем.
using Factory = std::function<std::any(IoC&, const Arg&)>;

// Scope - это ассоциативное хранилище "Ключ - Значение"
// где:
// Ключ - это строковый идентификатор
// Значение - это Фабрика.
// Скоупы можно выстраивать в цепочки (односвязанные списки),
// чтобы можно было общие "вещи" выносить в "общий" Скоуп, и
// не дублировать в обычных Скоупах.
class Scope {
public:
    std::shared_ptr<Scope>                      _parent; // указатель не Скоуп-родитель
    std::unordered_map<std::string, Factory>    _factory; // хранилище Фабрик

public:
    // Данный конструктор используется для создания Скоупа и 
    // привязывании его к родительсткому Скоупу, в котором уже
    // должна быть Фабрика-регистратор
    explicit Scope(std::shared_ptr<Scope> parent): _parent(parent) {};

    // Так как нет отдельного метода по добавление элемента в хранилище,
    // то этот конструктор используется для создание родительской Фабрики-регистратора
    // Фабрика-регистратор - это Команда, которая умеет добавлять элемент в хранилище
    Scope(const std::string& key, Factory factory) {
        _factory[key] = std::move(factory);
    };
};

// IoC - контейнер создающий все объекты в приложении,
// при этом скрывающий детали создания объектов:
// способ создания (new, pool ...), конкретный тип объекта,
// аргументы (зависимости) конструктора.
// Для создания IoC в приложении необходим Скоуп с уже
// добавленной Фабрикой-регистратором.
// IoC в приложении должен быть один.
// Если Фабрика должна вернуть объект, то это лямбда,
// если Фабрика не должна возвращать результат, то это Команда.
class IoC {
public:
    // В каждом новом потоке надо создать свой Скоуп,
    // который связан с первоначальным Скоупом (родителем),
    // в котором есть Фабрика-регистратор
    inline static thread_local std::shared_ptr<Scope>  _scope = nullptr; // указатель на голову списка Скоупов

public:
    IoC(std::shared_ptr<Scope> scope) {
        _scope = std::move(scope);
    };

    // Единственный публичный метод для получения объекта
    // В соответствии с SOLID максимально универсальный
    // По ключу и любому количеству аргументов, возвращает
    // std::shared_ptr на объект нужного типа.
    template<typename T, typename ... ARG>
    std::shared_ptr<T> resolve(const std::string& key, ARG&& ... a) {

        // Преобразование аргументов resolve() в массив
        // аргументов типа std::any.
        Arg arg;
        arg.reserve(sizeof...(ARG));
        (arg.push_back(Pack(std::forward<ARG>(a))), ...);

        // Поиск по ключу Нужной фабрики
        std::any result;
        auto scope = _scope;
        while (scope) {
            auto it = scope->_factory.find(key);
            if (it == scope->_factory.end()) {
                scope = scope->_parent;
            }
            else {
                result = it->second(*this, arg);
                break;
            }
        }
        if (!scope) {
            throw std::runtime_error("IoC: factory with key '" + key + "' not found");
        }

        try {
            return std::any_cast<std::shared_ptr<T>>(std::move(result));
        }
        catch (const std::bad_any_cast&) {
            throw std::runtime_error(
                "IoC: factory with '" + key + "' is registered with a different type than "
                + typeid(T).name()
                + " (return std::shared_ptr of the exact requested type from the factory)");
        }
        catch (...) {
            throw std::runtime_error("IoC: Unknown exception");
        }

    };

private:
    template <class F>
    static Factory MakeFactory(F f) {
        return [f = std::move(f)](IoC& ioc, const Arg& arg) mutable -> std::any {
            if constexpr (std::is_invocable_v<F&, IoC&, const Arg&>) {
                return std::any(f(ioc, arg));
            }
            else {
                return std::any(f(ioc));
            }
        };
    }

    template <class V>
    static std::any Pack(V&& v) {
        using D = std::decay_t<V>;
        // if constexpr (std::is_convertible_v<D, std::string>) {
        if constexpr (std::is_convertible_v<V, std::string>) {
            return std::string(std::forward<V>(v));
        }
        else if constexpr (std::is_invocable_v<D&, IoC&> ||
                             std::is_invocable_v<D&, IoC&, const Arg&>) {
            return MakeFactory(D(std::forward<V>(v)));
        }
        else {
            return std::any(std::forward<V>(v));
        }
    }

};

// Команда для регистрации новой Фабрики в IoC
class IoCRegisterCommand: public ICommand {
    IoC&                _ioc;
    const std::string   _key;
    Factory             _factory;
public:
    IoCRegisterCommand(IoC& ioc, const std::string& key, Factory factory): 
    _ioc(ioc), _key(key), _factory(factory) {}

    void Execute() override {
        _ioc._scope->_factory[_key] = _factory;
    };
    ~IoCRegisterCommand() override = default;
};

// !!! использование команды более накладно, нежели просто лямбды
// по сути команда создается временно и нужна ради одной строчки в Execute()

// Команда по изменению текущего списка Скоупов в IoC
class IoCSetScopeCommand: public ICommand {
    IoC&                    _ioc;
    std::shared_ptr<Scope>  _scope;
public:
    IoCSetScopeCommand(IoC& ioc, std::shared_ptr<Scope> scope): 
    _ioc(ioc), _scope(scope) {}

    void Execute() override {
        _ioc._scope = _scope;
    };
    ~IoCSetScopeCommand() override = default;
};

