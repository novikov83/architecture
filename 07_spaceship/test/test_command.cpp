#include <gtest/gtest.h>
#include <gmock/gmock.h>

// #include <unique>
#include <queue>
#include <thread>
#include <chrono>

#include <command.hpp>
#include <exception.hpp>
#include <handler.hpp>

using namespace std::chrono_literals;

/*
Задание:
Предположим, что все команды находятся в некоторой очереди. 
Обработка очереди заключается в чтении очередной команды из головы очереди 
и вызове метода Execute извлеченной команды. 
Метод Execute() может выбросить любое произвольное исключение.
Надо:
1. [Done] Обернуть вызов Команды в блок try-catch.
2. [Done] Обработчик catch должен перехватывать только самое базовое исключение.
3. [Done] Есть множество различных обработчиков исключений. Выбор подходящего 
обработчика исключения делается на основе экземпляра перехваченного 
исключения и команды, которая выбросила исключение.
4. [Done] Реализовать Команду, которая записывает информацию о выброшенном исключении в лог.
5. [Done] Реализовать обработчик исключения, который ставит Команду, пишущую в лог, в очередь Команд.
6. [Done] Реализовать Команду, которая повторяет Команду, выбросившую исключение.
7. [Done] Реализовать обработчик исключения, который ставит в очередь Команду - повторитель 
команды, выбросившей исключение.
8. [Done] С помощью Команд из пункта 4 и пункта 6 реализовать следующую обработку исключений:
при первом выбросе исключения повторить команду, при повторном выбросе исключения 
записать информацию в лог.
9. [Done] Реализовать стратегию обработки исключения - повторить два раза, потом записать 
в лог. Указание: создать новую команду, точно такую же, как в пункте 6. 
Тип этой команды будет показывать, что Команду не удалось выполнить два раза.
*/

// Фикстура для тестов
class CommandQueueTest : public ::testing::Test
{
protected:
    CommandQueue _queue;

    // метод вызываемый перед тестом
    void SetUp() override {
        _queue.Run();
    };

    // метод вызываемый после теста
    void TearDown() override {
        // дадим время поработать
        std::this_thread::sleep_for(1ms);

        _queue.Stop();
    };
};

// мок команды, которая что-то делает
class MockSendCommand : public ICommand
{
public:
    MOCK_METHOD(void, Execute, ());
};

// 5. Реализовать обработчик исключения, который ставит Команду, пишущую в лог, в очередь Команд.
TEST_F(CommandQueueTest, Test5)
{
    _queue.AddException<MockSendCommand, SendException, LogCommand>();

    auto command = std::make_unique<MockSendCommand>();

    // исключение бросается только первый раз
    // команда MockSendCommand должна вызываться два раза
    EXPECT_CALL(*command, Execute())
        .Times(1)
        .WillOnce(testing::Throw(SendException{""}));

    _queue.Push(std::move(command));
}

// 6. Реализовать Команду, которая повторяет Команду, выбросившую исключение.
// 7. Реализовать обработчик исключения, который ставит в очередь Команду - повторитель 
// команды, выбросившей исключение.
TEST_F(CommandQueueTest, Test6)
{
    _queue.AddException<MockSendCommand, SendException, FirstRepeatCommand>();

    auto command = std::make_unique<MockSendCommand>();

    // исключение бросается только первый раз
    // команда MockSendCommand должна вызываться два раза
    EXPECT_CALL(*command, Execute())
        .Times(2)
        .WillOnce(testing::Throw(SendException{""}))
        .WillOnce(testing::Return());

    _queue.Push(std::move(command));
}

// 8. С помощью Команд из пункта 4 и пункта 6 реализовать следующую обработку исключений:
// при первом выбросе исключения повторить команду, при повторном выбросе исключения 
// записать информацию в лог.
// 9. Реализовать стратегию обработки исключения - повторить два раза, потом записать 
// в лог. Указание: создать новую команду, точно такую же, как в пункте 6. 
// Тип этой команды будет показывать, что Команду не удалось выполнить два раза.
TEST_F(CommandQueueTest, OnceException)
{
    _queue.AddException<MockSendCommand, SendException, FirstRepeatCommand>();
    _queue.AddException<FirstRepeatCommand, SendException, SecondRepeatCommand>();
    _queue.AddException<SecondRepeatCommand, SendException, LogCommand>();

    auto command = std::make_unique<MockSendCommand>();

    // исключение бросается только первый раз
    // команда MockSendCommand должна вызываться два раза
    EXPECT_CALL(*command, Execute())
        .Times(2)
        .WillOnce(testing::Throw(SendException{""}))
        .WillOnce(testing::Return());

    _queue.Push(std::move(command));
}
TEST_F(CommandQueueTest, RepeatException)
{
    _queue.AddException<MockSendCommand, SendException, FirstRepeatCommand>();
    _queue.AddException<FirstRepeatCommand, SendException, SecondRepeatCommand>();
    _queue.AddException<SecondRepeatCommand, SendException, LogCommand>();

    auto command = std::make_unique<MockSendCommand>();

    // исключение бросается каждый раз
    // команда MockSendCommand должна вызываться три раза
    EXPECT_CALL(*command, Execute())
        .Times(3)
        .WillRepeatedly(testing::Throw(SendException{""}));

    _queue.Push(std::move(command));
}

// !!! такое пока не получилось реализовать
// чтобы на любую команду и/или любое исключение ставить в очередь конкретную команду
TEST_F(CommandQueueTest, AnyException)
{
    _queue.AddException<ICommand, IException, LogCommand>();

    auto command = std::make_unique<MockSendCommand>();

    // исключение бросается только первый раз
    EXPECT_CALL(*command, Execute())
        .Times(1)
        .WillOnce(testing::Throw(SendException{""}));

    _queue.Push(std::move(command));
}
