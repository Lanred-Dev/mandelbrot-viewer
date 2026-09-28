#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <utility>

template <typename... Args>
class Signal;

template <typename... Args>
class SignalConnection
{
public:
    SignalConnection(Signal<Args...> *signal, unsigned int id, std::function<void(Args...)> callback)
        : signal(signal), id(id), callback(callback)
    {
    }

    ~SignalConnection() = default;

    unsigned int ID() const { return id; }

    void Use(Args... args) const
    {
        if (callback)
        {
            callback(std::forward<Args>(args)...);
        }
    }

private:
    Signal<Args...> *signal;
    unsigned int id;
    std::function<void(Args...)> callback;
};

template <typename... Args>
class Signal
{
public:
    Signal() = default;
    ~Signal() = default;

    SignalConnection<Args...> *Connect(std::function<void(Args...)> callback)
    {
        connections.emplace_back(this, nextId++, std::move(callback));
        return &connections.back();
    }

    void Disconnect(SignalConnection<Args...> *connection)
    {
        if (!connection)
            return;

        unsigned int targetId = connection->ID();
        connections.erase(std::remove_if(connections.begin(), connections.end(),
                                         [targetId](const SignalConnection<Args...> &item)
                                         { return item.ID() == targetId; }),
                          connections.end());
    }

    void Emit(Args... args)
    {
        auto currentConnections = connections;
        for (const auto &connection : currentConnections)
        {
            connection.Use(args...);
        }
    }

private:
    unsigned int nextId = 0;
    std::vector<SignalConnection<Args...>> connections;
};
