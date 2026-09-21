#pragma once

#include <queue>
#include <unordered_map>
#include <typeindex>

#include "exception.hpp"
#include "command.hpp"

// обработчик исключений
// на основе "типа команды" и "типа исключения"
// конструирует новую команду и помещает ее в очередь команд
class ExceptionHandler {
    // ключ для ассоциативного контейнера "тип команды" + "тип исключения"
    using Key = std::pair<std::type_index, std::type_index>;

    // функтор для вычисления хэша по паре значений
    struct PairHash {
        std::size_t operator()(const Key& key) const {
            auto h1 = std::hash<std::type_index>{}(key.first);
            auto h2 = std::hash<std::type_index>{}(key.second);
            return h1 ^ (h2 << 1);
        }
    };

    // сигнатура новой команды, помещаемой в очередь
    using NewCommand = std::function<std::unique_ptr<ICommand>(std::unique_ptr<ICommand>)>;

    // ассоциативный контейнер
    std::unordered_map<Key, NewCommand, PairHash> _map;

public:
    // регистрация обработчика (новой команды) для "тип команды" + "тип исключения"
    template<typename COMMAND, typename EXCEPTION, typename NEW_COMMAND>
    void Add() {
        // std::cout << "Add: command '" << typeid(COMMAND).name() << "', exception '" << typeid(EXCEPTION).name() << "'" << std::endl;

        _map[Key(typeid(COMMAND), typeid(EXCEPTION))] =
                    [](std::unique_ptr<ICommand> prev_command) -> std::unique_ptr<ICommand>
        {
            return std::make_unique<NEW_COMMAND>(std::move(prev_command));
        };
    };

    std::type_index GetType(const ICommand& command) {
        // вынес в отдельную функцию, чтобы компилятор не выдавал warning:
        // expression with side effects will be evaluated despite being used as an operand to 'typeid' [-Wpotentially-evaluated-expression]
        // если использовать: auto it = _map.find(Key(typeid(*command), typeid(exception)));
        return typeid(command);
    }

    // непосредтвенно обработчик исключения
    std::unique_ptr<ICommand> Handle(std::unique_ptr<ICommand> command, const IException& exception) {
        if (!command) {
            return nullptr;
        }
        // std::cout << "Handle: command '" << GetType(*command).name() << "', exception '" << typeid(exception).name() << "'" << std::endl;
        auto it = _map.find(Key(GetType(*command), typeid(exception)));
        if (it == _map.end()) {
            return nullptr;
        }
        if (it->second) {
            return it->second(std::move(command));
        }
        return nullptr;
    };
};

// очередь комманд
// в отдельном потоке ждет новых команд и исполняет их
class CommandQueue {
    ExceptionHandler                        _exception_handler;
    std::queue<std::unique_ptr<ICommand>>   _queue;
    std::mutex                              _mutex;
    std::condition_variable                 _cv;
    std::atomic_bool                        _stopped{false};
    std::thread                             _thread;

public:
    // добавление новой команды в очередь
    void Push(std::unique_ptr<ICommand> command) {
        {
            std::lock_guard lock(_mutex);
            _queue.push(std::move(command));
        }
        _cv.notify_one();
    };

    // добавление обработчика для сочетания "тип команды" + "тип исключения"
    template<typename COMMAND, typename EXCEPTION, typename NEW_COMMAND>
    void AddException() {
        _exception_handler.Add<COMMAND, EXCEPTION, NEW_COMMAND>();
    };

    // запуск потока выполняющего команды из очереди
    void Run() {
        _thread = std::thread([this](){
            this->Worker();
        });
    };

    // остановка потока выполняющего команды
    void Stop() {
        {
            std::lock_guard lock(_mutex);
            _stopped = true;
        }
        Push(std::make_unique<StopCommand>());
        _thread.join();
    }

private:
    void Worker() {
        ExceptionHandler handler;
        while (!_stopped) {
            std::unique_ptr<ICommand> command;

            {
                std::unique_lock lock(_mutex);

                // ожидание появления новой команды
                _cv.wait(lock, [this] {
                    return !_queue.empty();
                });

                command = std::move(_queue.front());
                _queue.pop();
            }

            try {
                command->Execute();
            }
            catch (const IException& ex) {
                auto new_command = _exception_handler.Handle(std::move(command), ex);
                if (new_command) {
                    _queue.push(std::move(new_command));
                }
            }
        }
    };
};
