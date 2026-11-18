#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spaceship.hpp>
#include <vector.hpp>
#include <point.hpp>
#include <move.hpp>
#include <ioc.hpp>

#include "generated.hpp"
#if 0
class MoveWithGeneratedTest: public ::testing::Test {
protected:
    SpaceShip       spaceship;
    IMovingAdapter  adapter{spaceship}; // использование сгенерированного адаптера
    MoveCommand     move{adapter};
};
TEST_F(MoveWithGeneratedTest, MoveObject)
{
    // Для объекта, находящегося в точке (12, 5) и 
    // движущегося со скоростью (-7, 3) движение меняет 
    // положение объекта на (5, 8)
    spaceship.setProperty<Point>("Location", {12, 5});
    spaceship.setProperty<Vector>("Velocity", {-7, 3});
    move.Execute();
    EXPECT_EQ(spaceship.getProperty<Point>("Location"), Point(5, 8));
}

class MoveWithIoCTest: public ::testing::Test {
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

template<typename ARG>
class SetPropertyCommand: public ICommand
{
    UniversalItem*      _obj;
    std::string         _key;
    ARG                 _arg;
public:
    SetPropertyCommand(UniversalItem* obj, std::string key, ARG arg): 
    _obj(obj), _key(std::move(key)), _arg(std::move(arg)) 
    {}
    void Execute() override {
        _obj->setProperty(_key, _arg);
    }
};

// Заготовка для генерируемых адаптеров
// Работа через SpaceShip
// Это не эффективно так как адаптеры будут сгенерированы для 
// конкретного UniversalItem
TEST_F(MoveWithIoCTest, IoC_SetGet_1)
{
    _ioc.resolve<std::shared_ptr<ICommand>>("IoC.Register", "SpaceShip.getLocation", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 1) {throw std::invalid_argument("SpaceShip.getLocation expects (SpaceShip*)");}
        SpaceShip* obj = std::any_cast<SpaceShip*>(arg[0]);
        return std::make_shared<Point>(obj->getProperty<Point>("Location"));
    })->Execute();

    _ioc.resolve<ICommand>("IoC.Register", "SpaceShip.setLocation", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 2) {throw std::invalid_argument("SpaceShip.setLocation expects (SpaceShip*, Point&)");}
        // UniversalItem& obj = std::any_cast<UniversalItem&>(arg[0]);
        return std::shared_ptr<ICommand>(std::make_shared<SetPropertyCommand<Point>>(
            std::any_cast<SpaceShip*>(arg[0]),
            "Location",
            std::any_cast<Point>(arg[1])
        ));
    })->Execute();

    SpaceShip spaceship;
    _ioc.resolve<ICommand>("SpaceShip.setLocation", &spaceship, Point{3, 4})->Execute();

    auto p = _ioc.resolve<Point>("SpaceShip.getLocation", &spaceship);
    EXPECT_EQ(p->getX(), 3);
    EXPECT_EQ(p->getY(), 4);
}

// Заготовка для генерируемых адаптеров
// Работа через UniversalItem
TEST_F(MoveWithIoCTest, IoC_SetGet_2)
{
    _ioc.resolve<ICommand>("IoC.Register", "UniversalItem.getLocation", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 1) {throw std::invalid_argument("UniversalItem.getLocation expects (UniversalObject*)");}
        UniversalItem* obj = std::any_cast<UniversalItem*>(arg[0]);
        return std::make_shared<Point>(obj->getProperty<Point>("Location"));
    })->Execute();

    _ioc.resolve<ICommand>("IoC.Register", "UniversalItem.setLocation", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 2) {throw std::invalid_argument("UniversalItem.setLocation expects (UniversalItem*, Point)");}
        return std::shared_ptr<ICommand>(std::make_shared<SetPropertyCommand<Point>>(
            std::any_cast<UniversalItem*>(arg[0]),
            "Location",
            std::any_cast<Point>(arg[1])
        ));
    })->Execute();

    SpaceShip spaceship;
    UniversalItem universal = static_cast<UniversalItem>(spaceship);

    _ioc.resolve<ICommand>("UniversalItem.setLocation", &universal, Point{3, 4})->Execute();

    auto p = _ioc.resolve<Point>("UniversalItem.getLocation", &universal);
    EXPECT_EQ(p->getX(), 3);
    EXPECT_EQ(p->getY(), 4);
}
#endif
template<typename ARG>
class SetPropertyCommand: public ICommand
{
    UniversalItem*      _obj;
    std::string         _key;
    ARG                 _arg;
public:
    SetPropertyCommand(UniversalItem* obj, std::string key, ARG arg): 
    _obj(obj), _key(std::move(key)), _arg(std::move(arg)) 
    {}
    void Execute() override {
        _obj->setProperty(_key, _arg);
    }
};

class MoveWithGeneratedAndIoCest: public ::testing::Test {
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

    // Использование сгенерированного адаптера
    SpaceShip       spaceship;
    IMovingAdapter  adapter{_ioc, spaceship};
    MoveCommand     move{adapter};
};
TEST_F(MoveWithGeneratedAndIoCest, MoveObject)
{
    _ioc.resolve<std::shared_ptr<ICommand>>("IoC.Register", "UniversalItem.getVelocity", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 1) {throw std::invalid_argument("UniversalItem.getVelocity expects (UniversalObject*)");}
        UniversalItem* obj = std::any_cast<UniversalItem*>(arg[0]);
        return obj->getProperty<Vector>("Velocity");
    })->Execute();

    _ioc.resolve<std::shared_ptr<ICommand>>("IoC.Register", "UniversalItem.getLocation", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 1) {throw std::invalid_argument("UniversalItem.getLocation expects (UniversalObject*)");}
        UniversalItem* obj = std::any_cast<UniversalItem*>(arg[0]);
        return obj->getProperty<Point>("Location");
    })->Execute();

    _ioc.resolve<std::shared_ptr<ICommand>>("IoC.Register", "UniversalItem.setLocation", [](IoC& ioc, const Arg& arg){
        if (arg.size() != 2) {throw std::invalid_argument("UniversalItem.setLocation expects (UniversalItem*, Point)");}
        return std::shared_ptr<ICommand>(std::make_shared<SetPropertyCommand<Point>>(
            std::any_cast<UniversalItem*>(arg[0]),
            "Location",
            std::any_cast<Point>(arg[1])
        ));
    })->Execute();

    // Для объекта, находящегося в точке (12, 5) и 
    // движущегося со скоростью (-7, 3) движение меняет 
    // положение объекта на (5, 8)
    spaceship.setProperty<Point>("Location", {12, 5});
    spaceship.setProperty<Vector>("Velocity", {-7, 3});
    move.Execute();
    EXPECT_EQ(spaceship.getProperty<Point>("Location"), Point(5, 8));
}

// TEST_F(MoveWithGeneratedAndIoCest, CreateAdapter)
// {
//     _ioc.resolve<std::shared_ptr<ICommand>>("IoC.Register", "Adapter", [](IoC& ioc, const Arg& arg){
//         // if (arg.size() != 1) {throw std::invalid_argument("UniversalItem.getVelocity expects (UniversalObject*)");}
//         UniversalItem* obj = std::any_cast<UniversalItem*>(arg[1]);
//         return std::make_shared<IMovingAdapter>(ioc, obj);
//     })->Execute();

//     SpaceShip spaceship;
//     UniversalItem* universal = &spaceship;
//     auto adapter = _ioc.resolve<std::shared_ptr<IMoving>>("Adapter", typeid(IMoving), universal);
// }
