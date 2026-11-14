#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <command.hpp>
#include <ioc.hpp>

#include <thread>

// Тестовые классы для проверки зависимостей
class ILog {
public:
    virtual void log(const std::string& text) = 0;
};
class Log: public ILog {
    std::string _text;
public:
    void log (const std::string& text) override {
        _text.append(text);
    }
    std::string text() {
        return _text;
    }
};
class IService {
public:
    virtual std::string name() const = 0;
    virtual ~IService() = default;
};
class ServiceA: public IService {
public:
    std::string name() const override {
        return {"This is ServiceA"};
    }
    ~ServiceA() override = default;
};
class ServiceB: public IService {
    std::shared_ptr<ILog>   _log;
public:
    ServiceB(std::shared_ptr<ILog> log): _log(log) {}
    std::string name() const override {
        _log->log("This is ServiceB");
        return {"This is ServiceB"};
    }
    ~ServiceB() override = default;
};

class IoCTest: public ::testing::Test {
protected:
    // Создание Скоупа (родителя) с Фабрикой-регистратором
    std::shared_ptr<Scope> _root_scope = std::make_shared<Scope>("IoC.Register", [](IoC& ioc, const Arg& arg) -> std::any {
        if (arg.size() != 2) {throw std::invalid_argument("IoC.Register expects (std::string key, factory)");}
        return std::shared_ptr<ICommand>(std::make_shared<IoCRegisterCommand>(
            ioc, 
            std::any_cast<const std::string&>(arg[0]),
            std::any_cast<Factory>(arg[1])
        ));
    });
    // Создание IoC со Скоупом-родителем
    IoC     _ioc{_root_scope};
};

// Проверка обычной работы по созданию объектов
TEST_F(IoCTest, IoC_Ok_1)
{
    // Регистрация Фабрики по созданию объекта ServiceA
    _ioc.resolve<ICommand>("IoC.Register", "ServiceA", [](IoC& ioc){
        return std::make_shared<ServiceA>();
    })->Execute();

    // Создание объекта ServiceA
    auto service_a = _ioc.resolve<ServiceA>("ServiceA");
    EXPECT_EQ(service_a->name(), "This is ServiceA");

    // Регистрация Фабрики по созданию объекта Log
    _ioc.resolve<ICommand>("IoC.Register", "Log", [](IoC& ioc){
        return std::shared_ptr<ILog>(std::make_shared<Log>());
    })->Execute();

    // Регистрация Фабрики по созданию объекта ServiceB
    _ioc.resolve<ICommand>("IoC.Register", "ServiceB", [](IoC& ioc){
        return std::make_shared<ServiceB>(ioc.resolve<ILog>("Log"));
    })->Execute();

    // Создание объекта ServiceB, который требует объект Log
    auto service_b = _ioc.resolve<ServiceB>("ServiceB");
    EXPECT_EQ(service_b->name(), "This is ServiceB");
}

// Проверка создания нового Скоупа
TEST_F(IoCTest, IoC_NewScope_1)
{
    // Проверка, что в Скоупе только Фабрика-регистратор
    EXPECT_EQ(_root_scope->_factory.size(), 1);
    EXPECT_EQ(_root_scope->_factory.contains("IoC.Register"), true);

    // Регистрация Фабрики по созданию Скоупа
    _ioc.resolve<ICommand>("IoC.Register", "IoC.NewScope", [](IoC& ioc, const Arg& arg){
        return std::make_shared<Scope>(std::any_cast<std::shared_ptr<Scope>>(arg[0]));
    })->Execute();
    // Проверка, что Фабрика зарегистрировалась
    EXPECT_EQ(_root_scope->_factory.size(), 2);
    EXPECT_EQ(_root_scope->_factory.contains("IoC.NewScope"), true);

    // Создание обекта Скоуп
    auto scope_1 = _ioc.resolve<Scope>("IoC.NewScope", _root_scope);
    // Проверка, что Скоуп-родитель привязан
    EXPECT_EQ(scope_1->_parent, _root_scope);
    // Проверка, что в новом Скоупе нет зарегистрированных Фабрик
    EXPECT_EQ(scope_1->_factory.size(), 0);

    // Создание второго объекта Скоуп
    auto scope_2 = _ioc.resolve<Scope>("IoC.NewScope", _root_scope);
    // Проверка, что Скоуп-родитель привязан
    EXPECT_EQ(scope_2->_parent, _root_scope);
    // Проверка, что в новом Скоупе нет зарегистрированных Фабрик
    EXPECT_EQ(scope_2->_factory.size(), 0);
}

// Проверка по установке нового Скоупа в IoC
TEST_F(IoCTest, IoC_SetScope_1)
{
    // Регистрация Фабрикb по созданию Скоупа
    _ioc.resolve<ICommand>("IoC.Register", "IoC.NewScope", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 1) {throw std::invalid_argument("IoC.NewScope expects (std::shared_ptr<Scope>)");}
        return std::make_shared<Scope>(std::any_cast<std::shared_ptr<Scope>>(arg[0]));
    })->Execute();

    // Создание объектов Скоуп
    auto scope_1 = _ioc.resolve<Scope>("IoC.NewScope", _root_scope);

    // Регистрация Фабрикb по установке Скоупа
    _ioc.resolve<ICommand>("IoC.Register", "IoC.SetScope", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 1) {throw std::invalid_argument("IoC.SetScope expects (std::shared_ptr<Scope>)");}
        return std::shared_ptr<ICommand>(std::make_shared<IoCSetScopeCommand>(ioc, std::any_cast<std::shared_ptr<Scope>>(arg[0])));
    })->Execute();

    // Установка нового Скоупа
    _ioc.resolve<ICommand>("IoC.SetScope", scope_1)->Execute();
    // Проверка что Скоуп поменялся
    EXPECT_EQ(_ioc._scope->_factory.size(), 0);
    EXPECT_EQ(_ioc._scope->_parent, _root_scope);
}

// Проверка получения текущего Скоупа
TEST_F(IoCTest, IoC_GetScope_1)
{
    // Регистрация Фабрики по получению текущего Скоупа из IoC
    EXPECT_NO_THROW(_ioc.resolve<ICommand>("IoC.Register", "IoC.GetScope", [](IoC& ioc){
        return ioc._scope;
    })->Execute());

    // Получение текущего Скоупа
    auto scope = _ioc.resolve<Scope>("IoC.GetScope");
    EXPECT_EQ(scope->_parent, nullptr);
    EXPECT_EQ(scope->_factory.size(), 2);
    EXPECT_EQ(scope->_factory.contains("IoC.Register"), true);
    EXPECT_EQ(scope->_factory.contains("IoC.GetScope"), true);
}

class IoCTestThread: public ::testing::Test {
protected:
    std::shared_ptr<Scope> _root_scope = std::make_shared<Scope>("IoC.Register", [](IoC& ioc, const Arg& arg) -> std::any {
        if (arg.size() != 2) {throw std::invalid_argument("IoC.Register expects (std::string key, factory)");}
        return std::shared_ptr<ICommand>(std::make_shared<IoCRegisterCommand>(
            ioc, 
            std::any_cast<const std::string&>(arg[0]),
            std::any_cast<Factory>(arg[1])
        ));
    });
    IoC     _ioc{_root_scope};

protected:
    void SetUp() override {
        EXPECT_NO_THROW(_ioc.resolve<ICommand>("IoC.Register", "IoC.NewScope", [](IoC& ioc, const Arg& arg){
            return std::make_shared<Scope>(std::any_cast<std::shared_ptr<Scope>>(arg[0]));
        })->Execute());

        EXPECT_NO_THROW(_ioc.resolve<ICommand>("IoC.Register", "IoC.SetScope", [](IoC& ioc, const Arg& arg){
            if (arg.size() != 1) {throw std::invalid_argument("IoC.SetScope expects (std::shared_ptr<Scope>)");}
            return std::shared_ptr<ICommand>(std::make_shared<IoCSetScopeCommand>(ioc, std::any_cast<std::shared_ptr<Scope>>(arg[0])));
        })->Execute());

        EXPECT_NO_THROW(_ioc.resolve<ICommand>("IoC.Register", "IoC.GetScope", [](IoC& ioc){
            return ioc._scope;
        })->Execute());
    };
};
using namespace std::chrono_literals;
// Проверка работы в многопоточном приложении
// Создаются два потока и создают свои Скоупы,
// в которых регистрируют свои зависимости
TEST_F(IoCTestThread, Test_1)
{
    std::shared_ptr<Scope> scope;

    std::thread t1([&](){
        IoC     _ioc{_root_scope};
        scope = _ioc.resolve<Scope>("IoC.NewScope", _root_scope);
        _ioc.resolve<ICommand>("IoC.SetScope", scope)->Execute();

        _ioc.resolve<ICommand>("IoC.Register", "ServiceA", [](IoC& ioc){
            return std::make_shared<ServiceA>();
        })->Execute();

        for (auto i : {1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            auto service_a = _ioc.resolve<ServiceA>("ServiceA");
            EXPECT_EQ(service_a->name(), "This is ServiceA");
            std::this_thread::sleep_for(1ms);
        }
    });

    std::thread t2([&](){
        IoC     _ioc{_root_scope};
        scope = _ioc.resolve<Scope>("IoC.NewScope", _root_scope);
        _ioc.resolve<ICommand>("IoC.SetScope", scope)->Execute();

        _ioc.resolve<ICommand>("IoC.Register", "Log", [](IoC& ioc){
            return std::shared_ptr<ILog>(std::make_shared<Log>());
        })->Execute();

        _ioc.resolve<ICommand>("IoC.Register", "ServiceB", [](IoC& ioc){
            return std::make_shared<ServiceB>(ioc.resolve<ILog>("Log"));
        })->Execute();

        for (auto i : {1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            auto service_b = _ioc.resolve<ServiceB>("ServiceB");
            EXPECT_EQ(service_b->name(), "This is ServiceB");
            std::this_thread::sleep_for(1ms);
        }
    });

    t1.join();
    t2.join();
}
